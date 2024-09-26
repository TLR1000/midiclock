#include <MIDI.h>

// Constants
#define DEFAULT_TEMPO 120   // Standaard tempo in BPM als het niet verandert
#define MIDI_TX_PIN D3      // Pin gebruikt voor MIDI TX (uitgang)
#define LED_PIN D2          // Pin gebruikt voor de knipperende LED
#define BUTTON_PIN D1       // Pin waar de drukknop is aangesloten
#define PLUS_BUTTON_PIN D5  // Pin voor de plus-knop (BPM verhogen)
#define MINUS_BUTTON_PIN D6 // Pin voor de min-knop (BPM verlagen)
#define MIN_BPM 10          // Minimale BPM
#define MAX_BPM 1000        // Maximale BPM
#define DEBOUNCE_DELAY 50   // Debounce vertraging in milliseconden

// Variabelen
volatile bool buttonPressed = false;  // Geeft aan of de knop is ingedrukt
volatile unsigned long lastInterruptTime = 0;  // Tijd van de laatste interrupt om debounce te implementeren
unsigned long bpm = DEFAULT_TEMPO;    // Huidige BPM
bool beatInProgress = false;          // Geeft aan of we wachten op de tweede druk
unsigned long lastBeatTime = 0;       // Tijd van de eerste knopdruk

// MIDI Setup
MIDI_CREATE_DEFAULT_INSTANCE();

// Interrupt service routine (ISR) voor de knopdruk
void IRAM_ATTR handleButtonPress() {
  // Debouncing: zorg ervoor dat de knop niet meerdere keren wordt geregistreerd binnen een korte tijd
  unsigned long currentInterruptTime = millis();
  if (currentInterruptTime - lastInterruptTime > DEBOUNCE_DELAY) {  // 50 ms debounce
    buttonPressed = true;
    lastInterruptTime = currentInterruptTime;
  }
}

// Verstuur MIDI-klokcommando's
void sendMidiClock() {
  unsigned long interval = 60000 / bpm / 24;  // 24 MIDI-klokberichten per beat (standaard in MIDI)
  static unsigned long lastMidiTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastMidiTime >= interval) {
    MIDI.sendRealTime(midi::Clock);  // Verstuur MIDI-klokbericht
    lastMidiTime = currentTime;
  }
}

// LED knipperen op elke beat, behalve tijdens het programmeren van het tempo
void flashLed() {
  if (!beatInProgress) {  // Alleen knipperen als we niet bezig zijn met het programmeren van een beat
    static unsigned long lastBeatTime = 0;
    unsigned long beatInterval = 60000 / bpm;  // Tijd per beat in ms
    unsigned long ledOnDuration = beatInterval / 4;  // LED aan voor 1/4 van de beatduur
    unsigned long currentTime = millis();
    
    // Controleer of we aan het begin van een nieuwe beat zijn
    if (currentTime - lastBeatTime >= beatInterval) {
      // Zet de LED aan aan het begin van de beat
      digitalWrite(LED_PIN, HIGH);
      lastBeatTime = currentTime;  // Werk de tijd van de laatste beat bij
    }

    // Zet de LED uit na 1/4 van de beatduur
    if (currentTime - lastBeatTime >= ledOnDuration) {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

// Functie om het BPM in te stellen op basis van twee drukken
void adjustBPM() {
  if (buttonPressed) {
    buttonPressed = false;  // Reset de knopstatus

    // Als dit de eerste druk is, markeer het begin van de beat
    if (!beatInProgress) {
      lastBeatTime = millis();  // Sla de tijd van de eerste druk op
      beatInProgress = true;    // Zet beat-in-progress op actief
      digitalWrite(LED_PIN, HIGH);  // Zet de LED aan om aan te geven dat we wachten op de tweede druk
    } 
    // Als dit de tweede druk is, bereken het BPM en zet de LED weer uit
    else {
      unsigned long currentTime = millis();  // Tijd van de tweede druk
      unsigned long beatDuration = currentTime - lastBeatTime;  // Tijd tussen de twee drukken

      // Bereken het BPM
      bpm = 60000 / beatDuration;  // 60000 ms per minuut gedeeld door de tijd per beat

      // Zorg dat het BPM binnen de grenzen blijft
      if (bpm < MIN_BPM) bpm = MIN_BPM;
      if (bpm > MAX_BPM) bpm = MAX_BPM;  // Maximum BPM ingesteld op 1000

      // Zet de LED uit om aan te geven dat de tweede druk is geregistreerd en de BPM is ingesteld
      digitalWrite(LED_PIN, LOW);

      // Reset voor de volgende meting
      beatInProgress = false;
    }
  }
}

// Functie voor het verwerken van de plus- en min-knop
void checkFineTuneButtons() {
  // Controleer of de plusknop is ingedrukt
  if (digitalRead(PLUS_BUTTON_PIN) == LOW) {
    delay(DEBOUNCE_DELAY);  // Debounce
    if (digitalRead(PLUS_BUTTON_PIN) == LOW) {
      bpm++;  // Verhoog het BPM met 1
      if (bpm > MAX_BPM) bpm = MAX_BPM;  // Zorg dat het niet boven de MAX_BPM gaat
    }
  }

  // Controleer of de min-knop is ingedrukt
  if (digitalRead(MINUS_BUTTON_PIN) == LOW) {
    delay(DEBOUNCE_DELAY);  // Debounce
    if (digitalRead(MINUS_BUTTON_PIN) == LOW) {
      bpm--;  // Verlaag het BPM met 1
      if (bpm < MIN_BPM) bpm = MIN_BPM;  // Zorg dat het niet onder de MIN_BPM gaat
    }
  }
}

void setup() {
  // LED pin initialiseren
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED uit bij opstarten

  // Drukknop initialiseren met interne pull-up weerstand
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Fine-tune knoppen initialiseren
  pinMode(PLUS_BUTTON_PIN, INPUT_PULLUP);
  pinMode(MINUS_BUTTON_PIN, INPUT_PULLUP);

  // Interrupt instellen voor de knopdruk (falling edge, knop wordt ingedrukt)
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, FALLING);

  // MIDI initialiseren
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  // Continu MIDI-klokberichten versturen op basis van de BPM
  sendMidiClock();

  // Knipper LED op hetzelfde BPM-tempo
  flashLed();

  // Controleer of er een knopdruk heeft plaatsgevonden en pas het BPM aan
  adjustBPM();
  
  // Controleer of de plus- of min-knop is ingedrukt om het BPM te finetunen
  checkFineTuneButtons();
}
