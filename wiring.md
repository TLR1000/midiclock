# Wemos D1 Mini MIDI Tempo Controller Wiring

This guide provides the wiring instructions for connecting the Wemos D1 Mini to an OLED display, LED, buttons, and MIDI OUT for the MIDI tempo controller project.

## 1. MIDI OUT Wiring:
- **TX Pin (D7)** on the **Wemos D1 Mini**:
  - Connect the **TX pin (D7)** through a **220Ω resistor** to **Pin 5** of the **MIDI DIN connector**.
- **Pin 4** of the **MIDI DIN connector**:
  - Connect this to **5V** through another **220Ω resistor**.
- **Pin 2** of the **MIDI DIN connector**:
  - Connect this directly to **GND** (Ground) of the **Wemos D1 Mini**.

**Wiring Diagram for MIDI OUT**:


## 2. OLED Display (128x32 SSD1306):
- **SDA Pin** of the OLED (Data):
  - Connect to **D2** (GPIO 4) of the **Wemos D1 Mini**.
- **SCL Pin** of the OLED (Clock):
  - Connect to **D1** (GPIO 5) of the **Wemos D1 Mini**.
- **VCC Pin** of the OLED:
  - Connect to **3.3V** pin of the **Wemos D1 Mini**.
- **GND Pin** of the OLED:
  - Connect to **GND** of the **Wemos D1 Mini**.

**Wiring Summary for OLED Display**:



## 3. Buttons:
- **Tempo Button** (Tap Button):
  - One side of the button connects to **D3 (GPIO 0)**.
  - The other side of the button connects to **GND**.
  - Uses **INPUT_PULLUP** in code, so no external pull-up resistor is needed.
- **Plus Button** (Increase BPM):
  - One side of the button connects to **D5 (GPIO 14)**.
  - The other side of the button connects to **GND**.
- **Minus Button** (Decrease BPM):
  - One side of the button connects to **D6 (GPIO 12)**.
  - The other side of the button connects to **GND**.

**Wiring Summary for Buttons**:



## 4. LED:
- **LED Anode (+)**:
  - Connect through a **220Ω resistor** to **D0 (GPIO 16)** of the **Wemos D1 Mini**.
- **LED Cathode (-)**:
  - Connect directly to **GND**.

**Wiring Summary for LED**:



## 5. Power Supply:
- **5V Pin** of the **Wemos D1 Mini**:
  - Use this for powering the **MIDI OUT** circuit.
- **GND**:
  - Common ground for all components (OLED, buttons, MIDI, LED).

---

## Summary:
- **TX Pin (D7)** is used for sending MIDI data.
- The **OLED** is connected through **SDA (D2)** and **SCL (D1)**.
- Buttons are connected to **D3**, **D5**, and **D6**, with pull-ups enabled in software.
- **LED** is connected to **D0** through a **220Ω resistor**.
- MIDI output is wired with appropriate resistors to ensure correct signal levels.

