Dit project is een MIDI Tempo Controller gebaseerd op een Arduino microcontroller (in dit geval een Wemos D1 Mini). Het apparaat stuurt MIDI-kloksignalen uit via een MIDI DIN-aansluiting, waarmee andere MIDI-apparaten gesynchroniseerd kunnen worden. Het tempo (BPM) kan worden ingesteld met een tap-knop en fijn worden aangepast met plus- en minknoppen. De huidige BPM wordt weergegeven op een 128x32 OLED-display, terwijl een LED visuele feedback geeft bij elke beat. De USB-C chassis connector zorgt voor gemakkelijke voeding en programmering van het apparaat.

## Onderdelen en Functies  
tft-scherm: Dit scherm toont het huidige BPM (beats per minute).
Tapknop: Hiermee stel je het tempo in door twee opeenvolgende drukken. De eerste druk markeert het begin van een beat en de tweede druk markeert de volgende beat, waarna het apparaat het nieuwe tempo berekent.
Plusknop: Verhoogt het BPM met één bij elke druk.
Minknop: Verlaagt het BPM met één bij elke druk.
LED: De LED knippert op het ritme van het ingestelde tempo en blijft aan na de eerste tik van de tapknop om aan te geven dat het apparaat wacht op de tweede tik.

## Basic operation
1. Inschakelen en Opstarten  
Bij het inschakelen van de MIDI Tempo Controller wordt het laatst ingestelde BPM automatisch geladen uit het geheugen (EEPROM) en weergegeven op het tft-scherm.
2. Instellen van het Tempo  
Gebruik de tapknop (D3):  
Druk één keer op de tapknop om het begin van een beat te markeren. Het tft-scherm toont - - - om aan te geven dat het apparaat wacht op de tweede tik.
Druk nogmaals op de tapknop om het begin van de volgende beat te markeren. Het apparaat berekent dan het BPM en toont het resultaat op het tft-scherm. De LED gaat nu knipperen in het tempo van het BPM.
3. Fijn Afstellen van het BPM  
Gebruik de plus- en minknoppen (D5 en D6) om het tempo handmatig aan te passen:
Plusknop (D5): Verhoogt het BPM met één stap per druk.
Minknop (D6): Verlaagt het BPM met één stap per druk.
4. Visuele Feedback  
Het tft-scherm toont altijd het huidige BPM. Als je bezig bent met het instellen van het tempo via de tapknop, toont het - - - totdat de tweede tik is geregistreerd.
De LED knippert volgens het ingestelde BPM, en blijft na de eerste tik van de tapknop constant branden om aan te geven dat het apparaat wacht op de tweede tik.
5. Synchronisatie met MIDI-apparaten   
De MIDI Tempo Controller stuurt automatisch MIDI-kloksignalen naar je aangesloten MIDI-apparaten, zodat ze synchroon lopen met het ingestelde BPM.
6. Opslag en Geheugen (EEPROM)   
Het laatst ingestelde BPM wordt opgeslagen in het EEPROM-geheugen van de controller. Dit BPM wordt automatisch geladen bij het opstarten van het apparaat, zodat je niet elke keer het tempo opnieuw hoeft in te stellen.
7. Reset    
Schakel het apparaat uit en weer in om het opnieuw op te starten. Het opgeslagen BPM wordt dan geladen uit het geheugen.
