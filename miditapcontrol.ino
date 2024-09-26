#include <MIDI.h>

// Constants
#define DEFAULT_TEMPO 120  // Default tempo in BPM if not changed
#define MIDI_TX_PIN D3     // Pin used for MIDI TX
#define LED_PIN D2         // Pin used for flashing LED

// Variables
unsigned long bpm = DEFAULT_TEMPO;
bool ledOn = false;  // State of the LED

// MIDI Setup
MIDI_CREATE_DEFAULT_INSTANCE();

// Send MIDI Clock Command
void sendMidiClock() {
  unsigned long interval = 60000 / bpm / 24;  // 24 MIDI clock messages per beat
  static unsigned long lastMidiTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastMidiTime >= interval) {
    MIDI.sendRealTime(midi::Clock);
    Serial.println("MIDI Clock sent");
    lastMidiTime = currentTime;
  }
}

//Flash led on each beat
void flashLed() {
  static unsigned long lastBeatTime = 0;
  unsigned long beatInterval = 60000 / bpm;  // Time per beat in ms
  unsigned long ledOnDuration = beatInterval / 4;  // LED on for 1/4 of the beat duration
  unsigned long currentTime = millis();
  
  // Check if the current time is at the beginning of a new beat
  if (currentTime - lastBeatTime >= beatInterval) {
    // Turn the LED on at the start of the beat
    digitalWrite(LED_PIN, HIGH);
    lastBeatTime = currentTime;  // Update the last beat time
  }

  // Turn the LED off after one-quarter of the beat duration
  if (currentTime - lastBeatTime >= ledOnDuration) {
    digitalWrite(LED_PIN, LOW);
  }
}


void setup() {
  Serial.begin(115200);

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Start with LED off

  // Initialize MIDI
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Print initial BPM
  Serial.print("Initial BPM: ");
  Serial.println(bpm);
}

void loop() {
  // Continuously send MIDI clock messages based on the BPM
  sendMidiClock();

  // Flash LED at the same BPM rate
  flashLed();
}
