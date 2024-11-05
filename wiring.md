# Wemos D1 Mini MIDI Tempo Controller Wiring

### Voedingsaansluitingen:
- **Wemos D1 Mini Voeding:**
  - **3.3V pin** op de Wemos D1 Mini levert stroom aan alle componenten die 3.3V nodig hebben.
  - **GND pin** op de Wemos D1 Mini verbindt met alle GND-pinnen van de componenten.

### I2C Expander (MCP23X17):
- **MCP23X17 I2C Pinnen:**
  - **SDA (Data-lijn):** Verbind met **GPIO4** (D2) op de Wemos D1 Mini via een externe pullup-weerstand van 10kΩ naar 3.3V.
  - **SCL (Klok-lijn):** Verbind met **GPIO5** (D1) op de Wemos D1 Mini via een externe pullup-weerstand van 10kΩ naar 3.3V.
  - **VCC Pin:** Verbind met de **3.3V** pin op de Wemos D1 Mini.
  - **GND Pin:** Verbind met de **GND** pin op de Wemos D1 Mini.
  - **RESET Pin:** Verbind met de **3.3V** pin (voor altijd actieve modus).
  - **A0, A1, A2 Pinnen:** Verbind met **GND** voor het instellen van het MCP23X17 I2C-adres op 0x20.

### EEPROM (27C256 via I2C):
- **SDA (Data-lijn):** Verbind met **GPIO4** (D2) op de Wemos D1 Mini via een externe pullup-weerstand van 10kΩ naar 3.3V.
- **SCL (Klok-lijn):** Verbind met **GPIO5** (D1) op de Wemos D1 Mini via een externe pullup-weerstand van 10kΩ naar 3.3V.
- **VCC Pin:** Verbind met de **3.3V** pin op de Wemos D1 Mini.
- **GND Pin:** Verbind met de **GND** pin op de Wemos D1 Mini.
- **WP (Write Protect):** Verbind met **GND** om schrijven mogelijk te maken.
- **A0, A1, A2 Pinnen:** Verbind met **GND** voor adresselectie.

### Knoppen (Verbonden met MCP23X17 GPA Pinnen met externe pullup-weerstanden):
- **TAP Knop (Direct Verbonden met Wemos D1 Mini):**
  - Eén zijde van de knop verbindt met **GPIO0** (D3) op de Wemos D1 Mini (**TAP_BUTTON_PIN**).
  - De andere zijde van de knop verbindt met **GND**.
  - **Externe pullup-weerstand** van 10kΩ tussen **GPIO0 (D3)** en **3.3V**.

- **BPM_SET Knop (Verbonden met GPA1 op MCP23X17):**
  - Eén zijde van de knop verbindt met **GPA1** op de MCP23X17.
  - De andere zijde van de knop verbindt met **GND**.
  - **Externe pullup-weerstand** van 10kΩ tussen **GPA1** en **3.3V**.

- **BPM_PLUS Knop (Verbonden met GPA2 op MCP23X17):**
  - Eén zijde van de knop verbindt met **GPA2** op de MCP23X17.
  - De andere zijde van de knop verbindt met **GND**.
  - **Externe pullup-weerstand** van 10kΩ tussen **GPA2** en **3.3V**.

- **BPM_MINUS Knop (Verbonden met GPA3 op MCP23X17):**
  - Eén zijde van de knop verbindt met **GPA3** op de MCP23X17.
  - De andere zijde van de knop verbindt met **GND**.
  - **Externe pullup-weerstand** van 10kΩ tussen **GPA3** en **3.3V**.

- **STARTSTOP Knop (Verbonden met GPA4 op MCP23X17):**
  - Eén zijde van de knop verbindt met **GPA4** op de MCP23X17.
  - De andere zijde van de knop verbindt met **GND**.
  - **Externe pullup-weerstand** van 10kΩ tussen **GPA4** en **3.3V**.

- **CONTINUE Knop (Verbonden met GPA5 op MCP23X17):**
  - Eén zijde van de knop verbindt met **GPA5** op de MCP23X17.
  - De andere zijde van de knop verbindt met **GND**.
  - **Externe pullup-weerstand** van 10kΩ tussen **GPA5** en **3.3V**.

### LED's (Verbonden met MCP23X17 GPB Pinnen met stroombeperkende weerstanden):
- **Rode Beat-Indicatielampje (Verbonden met Wemos D1 Mini):**
  - De anode (+) van de rode LED verbindt met **GPIO16** (D0) op de Wemos D1 Mini.
  - De kathode (-) van de rode LED verbindt met **GND** via een **220Ω stroombeperkende weerstand**.

- **SET_LED (Verbonden met GPB1 op MCP23X17):**
  - De anode (+) van de LED verbindt met **GPB1** op de MCP23X17.
  - De kathode (-) verbindt met **GND** via een **220Ω stroombeperkende weerstand**.

- **STARTSTOP_LED (Verbonden met GPB4 op MCP23X17):**
  - De anode (+) van de LED verbindt met **GPB4** op de MCP23X17.
  - De kathode (-) verbindt met **GND** via een **220Ω stroombeperkende weerstand**.

- **CONT_LED (Verbonden met GPB5 op MCP23X17):**
  - De anode (+) van de LED verbindt met **GPB5** op de MCP23X17.
  - De kathode (-) verbindt met **GND** via een **220Ω stroombeperkende weerstand**.

### OLED-Display (SSD1306 I2C):
- **OLED SDA:** Verbind met **GPIO4** (D2) op de Wemos D1 Mini via een externe pullup-weerstand van 10kΩ naar 3.3V.
- **OLED SCL:** Verbind met **GPIO5** (D1) op de Wemos D1 Mini via een externe pullup-weerstand van 10kΩ naar 3.3V.
- **OLED VCC:** Verbind met **3.3V** pin op de Wemos D1 Mini.
- **OLED GND:** Verbind met **GND** pin op de Wemos D1 Mini.

### MIDI Chassis Connector:
- **Pin 4 van de MIDI Connector:** Verbind met **5V** via een **220Ω weerstand**.
- **Pin 5 van de MIDI Connector:** Verbind met **TX (GPIO1)** op de Wemos D1 Mini via een **220Ω weerstand**.
- **Pin 2 van de MIDI Connector:** Verbind met **GND** op de Wemos D1 Mini.
