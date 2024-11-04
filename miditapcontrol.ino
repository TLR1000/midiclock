#include <MIDI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MCP23X17.h>
#include <Fonts/FreeSansBold24pt7b.h>  // Include the larger font

// EEPROM Settings
#define EEPROM_I2C_ADDRESS 0x50  // Standard I2C address for 27C256 EEPROM

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_MCP23X17 mcp;  // I2C expander

// Constants
#define DEFAULT_TEMPO 120
#define MIDI_TX_PIN 13
#define RED_BEAT_INDICATOR_LED 16
#define TAP_BUTTON_PIN 0
#define MIN_BPM 10
#define MAX_BPM 1000
#define DEBOUNCE_DELAY 50

// MCP23X17 GPIO Pins
#define BPM_SET_BUTTON 1    // GPA1
#define BPM_PLUS_BUTTON 2   // GPA2
#define BPM_MINUS_BUTTON 3  // GPA3
#define SET_LED 9           // GPB1

// Define pins for STARTSTOP and CONT buttons and LEDs
#define STARTSTOP 4       // GPA4
#define CONT 5            // GPA5
#define STARTSTOP_LED 12  // GPB4 for START/STOP LED
#define CONT_LED 13       // GPB5 for CONT LED

// Variables
volatile bool tap_buttonPressed = false;
volatile unsigned long lastInterruptTime = 0;
uint16_t bpm = DEFAULT_TEMPO;  // Correcting BPM to uint16_t
uint16_t newBpm = DEFAULT_TEMPO;
bool beatInProgress = false;
unsigned long lastBeatTime = 0;
bool bpmSet = false;
bool bpmUpdated = false;
bool setButtonEnabled = false;  // To control when the set button is active

// Global variables for state
bool playing = false;  // Keeps track of whether the system is playing or stopped
bool startStopButtonStateIsStart = true;   // Tracks whether the STARTSTOP button should send START or STOP
bool isContEnabled = false;  // Tracks whether the CONT button is enabled

// MIDI Setup
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);

// Function to read BPM from EEPROM (two-byte addressing)
uint16_t readBpmFromEEPROM() {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write(0);  // MSB address byte
  Wire.write(0);  // LSB address byte
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_I2C_ADDRESS, 2);  // Read two bytes for the BPM value

  if (Wire.available() == 2) {
    uint16_t storedBpm = Wire.read() << 8 | Wire.read();  // Combine the two bytes
    if (storedBpm >= MIN_BPM && storedBpm <= MAX_BPM) {
      return storedBpm;
    }
  }
  return DEFAULT_TEMPO;  // If the value is invalid or out of range, return default BPM
}

// Function to write BPM to EEPROM (two-byte addressing)
void writeBpmToEEPROM(uint16_t bpmToStore) {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write(0);                  // MSB address byte
  Wire.write(0);                  // LSB address byte
  Wire.write(bpmToStore >> 8);    // Write the high byte
  Wire.write(bpmToStore & 0xFF);  // Write the low byte
  Wire.endTransmission();
}

// Function to enable the set button and turn on the blue LED
void enableSetButton() {
  setButtonEnabled = true;          // Enable the set button
  mcp.digitalWrite(SET_LED, HIGH);  // Turn on blue LED
}

// Function to handle tap button presses
void adjustBPMWithTapButton() {
  if (tap_buttonPressed) {
    tap_buttonPressed = false;

    if (!beatInProgress) {
      lastBeatTime = millis();
      beatInProgress = true;
      digitalWrite(RED_BEAT_INDICATOR_LED, HIGH);  // Red LED stays on after first tap
      displayWaitingForSecondTap();
    } else {
      unsigned long currentTime = millis();
      unsigned long beatDuration = currentTime - lastBeatTime;
      newBpm = 60000 / beatDuration;

      if (newBpm < MIN_BPM) newBpm = MIN_BPM;
      if (newBpm > MAX_BPM) newBpm = MAX_BPM;

      beatInProgress = false;
      bpmUpdated = true;                          // Internal BPM is updated
      digitalWrite(RED_BEAT_INDICATOR_LED, LOW);  // Turn off the red LED after the second tap
      enableSetButton();                          // Enable the set button (blue LED turns on)
      displayCurrentBPM();
    }
  }
}

void handleSetButton() {
  if (setButtonEnabled && !mcp.digitalRead(BPM_SET_BUTTON)) {
    bpm = newBpm;                    // Confirm the new BPM for the MIDI clock
    bpmSet = true;                   // Ensure the MIDI clock updates immediately with the new BPM
    writeBpmToEEPROM(bpm);           // Save the new BPM to EEPROM
    setButtonEnabled = false;        // Disable the set button after confirming and saving
    mcp.digitalWrite(SET_LED, LOW);  // Turn off the blue LED for the set button
  }
}

// Function to handle the + and - buttons
void handleBpmButtons() {
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 200;  // Adjust as needed for stable readings

  unsigned long currentTime = millis();

  if (currentTime - lastDebounceTime > debounceDelay) {
    // Handle + button: Immediately update MIDI clock and enable set button
    if (!mcp.digitalRead(BPM_PLUS_BUTTON)) {
      newBpm = min((int)newBpm + 1, MAX_BPM);  // Increment BPM
      bpmUpdated = true;
      bpm = newBpm;         // Update the current BPM for the MIDI clock
      bpmSet = true;        // Ensure MIDI clock updates immediately
      enableSetButton();    // Enable the set button
      displayCurrentBPM();  // Update OLED display
      lastDebounceTime = currentTime;
    }

    // Handle - button: Immediately update MIDI clock and enable set button
    if (!mcp.digitalRead(BPM_MINUS_BUTTON)) {
      newBpm = max((int)newBpm - 1, MIN_BPM);  // Decrement BPM
      bpmUpdated = true;
      bpm = newBpm;         // Update the current BPM for the MIDI clock
      bpmSet = true;        // Ensure MIDI clock updates immediately
      enableSetButton();    // Enable the set button
      displayCurrentBPM();  // Update OLED display
      lastDebounceTime = currentTime;
    }
  }
}

// Handle STARTSTOP button functionality
void handleStartStopButton() {
  if (!mcp.digitalRead(STARTSTOP)) {  // Button is pressed
    if (!playing) {  // If not playing, START the playback
      // Send MIDI Start
      MIDI.sendRealTime(midi::Start);
      Serial.println("MIDI Start sent");

      // Update the state
      playing = true;  // Now the system is playing
      startStopButtonStateIsStart = false; // Button function switches to STOP
      mcp.digitalWrite(STARTSTOP_LED, LOW);  // Turn off the LED because function is now STOP
      
      // CONT button should be disabled while playing
      isContEnabled = false;
      mcp.digitalWrite(CONT_LED, LOW);  // Turn off CONT LED
    } else {
      // Send MIDI Stop
      MIDI.sendRealTime(midi::Stop);
      Serial.println("MIDI Stop sent");

      // Update the state
      playing = false; // System is now stopped
      startStopButtonStateIsStart = true;  // Button function switches back to START
      mcp.digitalWrite(STARTSTOP_LED, HIGH);  // Turn on the LED because function is now START

      // CONT button should be enabled when stopped
      isContEnabled = true;
      mcp.digitalWrite(CONT_LED, HIGH);  // Turn on CONT LED
    }
    delay(200);  // Debounce delay
  }
}

// Handle CONT button functionality
void handleContButton() {
  if (isContEnabled && !mcp.digitalRead(CONT)) {  // Button is pressed and CONT is enabled
    // Send MIDI Continue
    MIDI.sendRealTime(midi::Continue);
    Serial.println("MIDI Continue sent");

    // Update the state
    playing = true;  // Now the system is playing after CONTINUE
    startStopButtonStateIsStart = false; // STARTSTOP button function should be STOP now
    mcp.digitalWrite(STARTSTOP_LED, LOW);  // Turn off STARTSTOP LED (function is STOP)

    // CONT button should be disabled while playing
    isContEnabled = false;
    mcp.digitalWrite(CONT_LED, LOW);  // Turn off CONT LED
    delay(200);  // Debounce delay
  }
}

// Send MIDI Clock if BPM has been set
void sendMidiClock() {
  unsigned long interval = 60000 / bpm / 24;
  static unsigned long lastMidiTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastMidiTime >= interval) {
    MIDI.sendRealTime(midi::Clock);
    lastMidiTime = currentTime;
  }
}

// Flash LED for each beat
void flashLed() {
  if (!beatInProgress) {
    unsigned long beatInterval = 60000 / bpm;
    unsigned long ledOnDuration = beatInterval / 10;
    unsigned long currentTime = millis();

    if (currentTime - lastBeatTime >= beatInterval) {
      digitalWrite(RED_BEAT_INDICATOR_LED, HIGH);
      lastBeatTime = currentTime;
    }

    if (currentTime - lastBeatTime >= ledOnDuration) {
      digitalWrite(RED_BEAT_INDICATOR_LED, LOW);
    }
  }
}

// Display current BPM on OLED
void displayCurrentBPM() {
  display.clearDisplay();
  display.setFont(&FreeSansBold24pt7b);  // Set larger font
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 40);  // Adjust Y position for larger font
  display.print(newBpm);
  display.display();
}

// Display '---' while waiting for second tap
void displayWaitingForSecondTap() {
  display.clearDisplay();
  display.setFont(&FreeSansBold24pt7b);  // Set larger font
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 40);  // Adjust Y position for larger font
  display.print("- - -");
  display.display();
}

void IRAM_ATTR handleTapButtonPress() {
  unsigned long currentInterruptTime = millis();
  if (currentInterruptTime - lastInterruptTime > DEBOUNCE_DELAY) {
    tap_buttonPressed = true;
    lastInterruptTime = currentInterruptTime;
  }
}

void setup() {
  pinMode(RED_BEAT_INDICATOR_LED, OUTPUT);
  digitalWrite(RED_BEAT_INDICATOR_LED, LOW);
  pinMode(TAP_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TAP_BUTTON_PIN), handleTapButtonPress, FALLING);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true)
      ;  // Halt if display fails
  }

  if (!mcp.begin_I2C()) {  // Initialize MCP23017 over I2C
    while (true)
      ;  // Halt if MCP23017 fails to initialize
  }

  // Set the button pins as inputs with pull-ups
  mcp.pinMode(BPM_SET_BUTTON, INPUT);
  mcp.pinMode(BPM_PLUS_BUTTON, INPUT);
  mcp.pinMode(BPM_MINUS_BUTTON, INPUT);

  mcp.digitalWrite(BPM_SET_BUTTON, HIGH);    // Enable pull-up
  mcp.digitalWrite(BPM_PLUS_BUTTON, HIGH);   // Enable pull-up
  mcp.digitalWrite(BPM_MINUS_BUTTON, HIGH);  // Enable pull-up

  // Set LED as output
  mcp.pinMode(SET_LED, OUTPUT);
  mcp.digitalWrite(SET_LED, LOW);

  // Set up STARTSTOP and CONT buttons and LEDs
  mcp.pinMode(STARTSTOP, INPUT);
  mcp.pinMode(CONT, INPUT);
  mcp.digitalWrite(STARTSTOP, HIGH);  // Enable pull-up for STARTSTOP button
  mcp.digitalWrite(CONT, HIGH);       // Enable pull-up for CONT button

  mcp.pinMode(STARTSTOP_LED, OUTPUT);
  mcp.pinMode(CONT_LED, OUTPUT);

  // Initially, CONT button is disabled and CONT_LED is off
  mcp.digitalWrite(CONT_LED, LOW);

  // Initially, STARTSTOP_LED is on because the function is START
  mcp.digitalWrite(STARTSTOP_LED, HIGH);

  // Read the stored BPM value from EEPROM on startup
  Wire.begin();  // Initialize I2C
  bpm = readBpmFromEEPROM();
  newBpm = bpm;  // Set newBpm to the stored value

  // Set bpmSet to true and ensure the MIDI clock is updated
  bpmSet = true;
  sendMidiClock();  // Immediately update MIDI clock with the BPM from EEPROM

  Serial.begin(31250);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  displayCurrentBPM();
}

void loop() {
  sendMidiClock();           // Continually send MIDI clock based on BPM
  flashLed();                // Flash LED based on the current BPM
  adjustBPMWithTapButton();  // Handle tap button
  handleBpmButtons();        // Handle + and - buttons
  handleSetButton();         // Handle Set button to confirm the BPM
  handleStartStopButton();
  handleContButton();
}
