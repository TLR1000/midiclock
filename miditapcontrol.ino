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
  Wire.write(0);  // MSB address byte
  Wire.write(0);  // LSB address byte
  Wire.write(bpmToStore >> 8);  // Write the high byte
  Wire.write(bpmToStore & 0xFF);  // Write the low byte
  Wire.endTransmission();
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
    while (true); // Halt if display fails
  }

  if (!mcp.begin_I2C()) {   // Initialize MCP23017 over I2C
    while (true); // Halt if MCP23017 fails to initialize
  }

  // Set the button pins as inputs with pull-ups
  mcp.pinMode(BPM_SET_BUTTON, INPUT);
  mcp.pinMode(BPM_PLUS_BUTTON, INPUT);
  mcp.pinMode(BPM_MINUS_BUTTON, INPUT);

  mcp.digitalWrite(BPM_SET_BUTTON, HIGH);  // Enable pull-up
  mcp.digitalWrite(BPM_PLUS_BUTTON, HIGH); // Enable pull-up
  mcp.digitalWrite(BPM_MINUS_BUTTON, HIGH); // Enable pull-up

  // Set LED as output
  mcp.pinMode(SET_LED, OUTPUT);
  mcp.digitalWrite(SET_LED, LOW);

  // Read the stored BPM value from EEPROM on startup
  Wire.begin();  // Initialize I2C
  bpm = readBpmFromEEPROM();
  newBpm = bpm;  // Set newBpm to the stored value

  Serial.begin(31250);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  displayCurrentBPM();
}

void loop() {
  sendMidiClock();
  flashLed();
  adjustBPM();
  handleBpmButtons();
}

// Send MIDI Clock if BPM has been set
void sendMidiClock() {
  if (bpmSet) {
    unsigned long interval = 60000 / bpm / 24;
    static unsigned long lastMidiTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastMidiTime >= interval) {
      MIDI.sendRealTime(midi::Clock);
      lastMidiTime = currentTime;
    }
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

// Adjust BPM based on tap button presses
void adjustBPM() {
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
      bpmUpdated = true;
      setButtonEnabled = true;  // Enable the set button after second tap
      mcp.digitalWrite(SET_LED, HIGH);  // Blue LED is lit when set button is enabled
      digitalWrite(RED_BEAT_INDICATOR_LED, LOW); // Turn off the red LED after the second tap
      displayCurrentBPM();
    }
  }
}

void handleBpmButtons() {
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 200; // Adjust as needed for stable readings

  unsigned long currentTime = millis();

  if (currentTime - lastDebounceTime > debounceDelay) {
    if (!mcp.digitalRead(BPM_PLUS_BUTTON)) {
      newBpm = min((int)newBpm + 1, MAX_BPM);  // Cast newBpm to int for min
      bpmUpdated = true;
      displayCurrentBPM();
      lastDebounceTime = currentTime;
    }
    if (!mcp.digitalRead(BPM_MINUS_BUTTON)) {
      newBpm = max((int)newBpm - 1, MIN_BPM);  // Cast newBpm to int for max
      bpmUpdated = true;
      displayCurrentBPM();
      lastDebounceTime = currentTime;
    }
    if (setButtonEnabled && !mcp.digitalRead(BPM_SET_BUTTON) && bpmUpdated) {  // Only allow the set button after the second tap
      bpm = newBpm; // Set new BPM
      bpmSet = true;
      bpmUpdated = false;
      setButtonEnabled = false;  // Disable the set button after it's pressed
      mcp.digitalWrite(SET_LED, LOW); // Turn off blue LED
      writeBpmToEEPROM(bpm);  // Store the new BPM in EEPROM
      lastDebounceTime = currentTime;
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
