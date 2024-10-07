#include <MIDI.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBold18pt7b.h>  // Include a custom font

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Constants
#define DEFAULT_TEMPO 120     // Default BPM
#define MIDI_TX_PIN D7        // TX pin used for MIDI communication
#define LED_PIN D0            // Pin for flashing LED (GPIO16)
#define BUTTON_PIN D3         // Pin for the tempo button (GPIO0)
#define PLUS_BUTTON_PIN D5    // Pin for plus button (GPIO14)
#define MINUS_BUTTON_PIN D6   // Pin for minus button (GPIO12)
#define MIN_BPM 10            // Min BPM
#define MAX_BPM 1000          // Max BPM
#define DEBOUNCE_DELAY 50     // Debounce delay
#define EEPROM_BPM_ADDRESS 0  // EEPROM address for BPM storage

// Variables
volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0;
unsigned long bpm = DEFAULT_TEMPO;
bool beatInProgress = false;
unsigned long lastBeatTime = 0;
unsigned long lastButtonPressTime = 0;
const unsigned long buttonDebounceDelay = 50;

// MIDI Setup
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);  // Use hardware Serial for MIDI

void IRAM_ATTR handleButtonPress() {
  unsigned long currentInterruptTime = millis();
  if (currentInterruptTime - lastInterruptTime > DEBOUNCE_DELAY) {
    buttonPressed = true;
    lastInterruptTime = currentInterruptTime;
  }
}

// Send MIDI Clock
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
    static unsigned long lastBeatTime = 0;
    unsigned long beatInterval = 60000 / bpm;
    unsigned long ledOnDuration = beatInterval / 4;
    unsigned long currentTime = millis();

    if (currentTime - lastBeatTime >= beatInterval) {
      digitalWrite(LED_PIN, HIGH);
      lastBeatTime = currentTime;
    }

    if (currentTime - lastBeatTime >= ledOnDuration) {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

// Adjust BPM based on button presses
void adjustBPM() {
  if (buttonPressed) {
    buttonPressed = false;

    if (!beatInProgress) {
      lastBeatTime = millis();
      beatInProgress = true;
      digitalWrite(LED_PIN, HIGH);
      displayWaitingForSecondTap();
    } else {
      unsigned long currentTime = millis();
      unsigned long beatDuration = currentTime - lastBeatTime;
      bpm = 60000 / beatDuration;

      if (bpm < MIN_BPM) bpm = MIN_BPM;
      if (bpm > MAX_BPM) bpm = MAX_BPM;

      digitalWrite(LED_PIN, LOW);
      beatInProgress = false;
      saveBPMToEEPROM();
      displayCurrentBPM();
    }
  }
}

// Fine-tune buttons for adjusting BPM
void checkFineTuneButtons() {
  unsigned long currentTime = millis();

  if (digitalRead(PLUS_BUTTON_PIN) == LOW && currentTime - lastButtonPressTime > buttonDebounceDelay) {
    bpm++;
    if (bpm > MAX_BPM) bpm = MAX_BPM;
    lastButtonPressTime = currentTime;
    saveBPMToEEPROM();
    displayCurrentBPM();
  }

  if (digitalRead(MINUS_BUTTON_PIN) == LOW && currentTime - lastButtonPressTime > buttonDebounceDelay) {
    bpm--;
    if (bpm < MIN_BPM) bpm = MIN_BPM;
    lastButtonPressTime = currentTime;
    saveBPMToEEPROM();
    displayCurrentBPM();
  }
}

// Load BPM from EEPROM
void loadBPMFromEEPROM() {
  int storedBPM = EEPROM.read(EEPROM_BPM_ADDRESS) | (EEPROM.read(EEPROM_BPM_ADDRESS + 1) << 8);
  if (storedBPM >= MIN_BPM && storedBPM <= MAX_BPM) {
    bpm = storedBPM;
  } else {
    bpm = DEFAULT_TEMPO;
  }
}

// Save BPM to EEPROM
void saveBPMToEEPROM() {
  EEPROM.write(EEPROM_BPM_ADDRESS, bpm & 0xFF);
  EEPROM.write(EEPROM_BPM_ADDRESS + 1, (bpm >> 8) & 0xFF);
}

// Display current BPM on OLED
void displayCurrentBPM() {
  display.clearDisplay();
  display.setFont(&FreeSansBold18pt7b);  // Set the custom font
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  display.print(bpm);
  display.display();
}

// Function to display '---' on OLED while waiting for the second tap
void displayWaitingForSecondTap() {
  display.clearDisplay();
  display.setFont(&FreeSansBold18pt7b);  // Set the custom font
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  display.print("- - -");  // Display placeholder while waiting for the second tap
  display.display();
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(PLUS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MINUS_BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true)
      ;
  }

  loadBPMFromEEPROM();
  displayCurrentBPM();

  Serial.begin(31250);  // Initialize MIDI at 31250 baud using the TX pin
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  sendMidiClock();
  flashLed();
  adjustBPM();
  checkFineTuneButtons();
}
