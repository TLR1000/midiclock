#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <MIDI.h>
#include <Encoder.h>

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// MIDI setup
MIDI_CREATE_DEFAULT_INSTANCE();

// Encoder setup
Encoder myEnc(2, 3);  // CLK -> D2, DT -> D3
int buttonPin = 4;    // Drukknop van de encoder
int footswitchPin = 5;  // Footswitch
int lastEncPosition = 0;
bool settingProgram = true;  // true = programma instellen, false = kanaal instellen

// MIDI variabelen
int midiChannel = 1;  // Default kanaal is 1
int programNumber = 0;  // Default programma is 0

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Drukknop met interne pull-up
  pinMode(footswitchPin, INPUT_PULLUP);  // Footswitch met interne pull-up

  // Start MIDI en display
  MIDI.begin(MIDI_CHANNEL_OFF);  // Alleen MIDI-uit
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // I2C adres van OLED
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Initialiseer display
  updateDisplay();
}

void loop() {
  // Lees de encoder rotatie
  long newPosition = myEnc.read() / 4;  // Verlaag resolutie
  if (newPosition != lastEncPosition) {
    if (settingProgram) {
      programNumber = constrain(newPosition, 0, 127);  // Programma instellen tussen 0 en 127
    } else {
      midiChannel = constrain(newPosition + 1, 1, 16);  // Kanaal instellen tussen 1 en 16
    }
    updateDisplay();
    lastEncPosition = newPosition;
  }

  // Controleer op encoder drukknop om te wisselen tussen programma en kanaal instellen
  if (digitalRead(buttonPin) == LOW) {
    settingProgram = !settingProgram;  // Wissel modus
    delay(200);  // Debounce delay
  }

  // Controleer op footswitch voor het versturen van MIDI Program Change
  if (digitalRead(footswitchPin) == LOW) {
    MIDI.sendProgramChange(programNumber, midiChannel);
    delay(200);  // Debounce delay
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  if (settingProgram) {
    display.print("Set Program: ");
  } else {
    display.print("Set Channel: ");
  }
  display.setCursor(0, 20);
  display.print("Program: ");
  display.print(programNumber);
  display.setCursor(0, 40);
  display.print("Channel: ");
  display.print(midiChannel);
  display.display();
}
