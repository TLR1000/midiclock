#include <Wire.h>
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcpSwitches;  // Voor schakelaars
Adafruit_MCP23X17 mcpLeds;      // Voor LEDs

void setup() {
  // Start de seriële verbinding (indien gewenst voor debugging)
  Serial.begin(9600);

  // Start de I2C verbinding
  Wire.begin();

  // Initialiseer MCP23017 voor de schakelaars (adres 0x20)
  mcpSwitches.begin(0);

  // Stel GPIOA (0-7) en GPIOB (8-15) in als input voor de schakelaars
  for (uint8_t i = 0; i < 16; i++) {
    mcpSwitches.pinMode(i, INPUT);
    mcpSwitches.pullUp(i, HIGH);  // Activeer interne pull-up weerstanden
  }

  // Initialiseer MCP23017 voor de LEDs (adres 0x21)
  mcpLeds.begin(1);

  // Stel GPIOA (0-7) en GPIOB (8-15) in als output voor de LEDs
  for (uint8_t i = 0; i < 16; i++) {
    mcpLeds.pinMode(i, OUTPUT);
    mcpLeds.digitalWrite(i, LOW); // Zorg dat de LEDs uit zijn bij de start
  }
}

void loop() {
  // Lees schakelaars en zet LEDs aan of uit
  for (uint8_t i = 0; i < 16; i++) {
    uint8_t buttonState = mcpSwitches.digitalRead(i);  // Lees de schakelaar

    if (buttonState == LOW) {  // Schakelaar ingedrukt (actief laag)
      mcpLeds.digitalWrite(i, HIGH);  // Zet corresponderende LED aan
    } else {
      mcpLeds.digitalWrite(i, LOW);   // Zet corresponderende LED uit
    }
  }
}
