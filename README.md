# Korte versie gebruik van het midi clock controller pedal board:   
TAP wordt gebruikt om de nieuwe waarde in te stellen.   
SET maakt de nieuw ingestelde waarde actief op MIDI.

# Uitgebreide versie gebruik van het midi clock controller pedal board:   

## About  
Dit project is een MIDI Tempo Controller gebaseerd op een Arduino microcontroller (in dit geval een Wemos D1 Mini). Het apparaat stuurt MIDI-kloksignalen uit via een MIDI DIN-aansluiting, waarmee andere MIDI-apparaten gesynchroniseerd kunnen worden. Het tempo (BPM) kan worden ingesteld met een tap-knop en fijn worden aangepast met plus- en minknoppen. De huidige BPM wordt weergegeven op een 128x32 OLED-display, terwijl een LED visuele feedback geeft bij elke beat. De USB-C chassis connector zorgt voor gemakkelijke voeding en programmering van het apparaat.

## Onderdelen en Functies  
tft-scherm: Dit scherm toont het huidige BPM (beats per minute).  
TAP knop: Hiermee stel je het tempo in door twee opeenvolgende drukken. De eerste druk markeert het begin van een beat en de tweede druk markeert de volgende beat, waarna het apparaat het nieuwe tempo berekent.   
SET knop: Hiermee maak je de BPM ingestelde waarde actief voor de MIDI out clock signalen.
Plusknop: Verhoogt het BPM met één bij elke druk. Je kunt de knop ook ingedrukt houden om tempo met 1 bpm tegelijk te verhogen.  
Minknop: Verlaagt het BPM met één bij elke druk. Je kunt de knop ook ingedrukt houden om tempo met 1 bpm tegelijk te verhogen.   
Rode LED: De LED knippert op het ritme van het ingestelde tempo en blijft aan na de eerste tap van de tapknop om aan te geven dat het apparaat wacht op de tweede tap.  

## Basic operation
Inschakelen en Opstarten  
Bij het inschakelen van de MIDI Tempo Controller wordt het laatst ingestelde BPM automatisch geladen uit het geheugen en weergegeven op het tft-scherm.

## Instellen van het Tempo  
 
1. Gebruik de knop TAP:  
Druk één keer op TAP om het begin van een beat te markeren. Het scherm toont - - - en de rode led brandt continue om aan te geven dat het apparaat wacht op de tweede tik.   
Druk nogmaals op de tapknop om het begin van de volgende beat te markeren.   
Het apparaat berekent dan het BPM en toont het resultaat op het tft-scherm. Zolang SET nog niet is ingedrukt, kan het tempo onbeperkt gewijzigd worden voordat het actief wordt.

2. Gebruik de knop SET:   
Om het nieuwe tempo via MIDI uit te sturen dient SET ingedrukt te worden. De blauwe led licht op indien er een nieuw tempo gekozen is dat nog niet op midi actief is.
De MIDI Tempo Controller stuurt automatisch MIDI-kloksignalen naar je aangesloten MIDI-apparaten, zodat ze synchroon lopen met het ingestelde BPM.
Het ingestelde tempo wordt opgeslagen in het geheugen zodat de controller er de volgende keer mee opstart als beginwaarde.
 
Fijn Afstellen van het BPM:  
Gebruik de plus- en minknoppen om het tempo handmatig aan te passen:
Plusknop: Verhoogt het BPM met één stap per druk.
Minknop: Verlaagt het BPM met één stap per druk.

# Visuele Feedback  
Het tft-scherm toont altijd het huidige interne BPM. Als je bezig bent met het instellen van het tempo via de tapknop, toont het - - - totdat de tweede tik is geregistreerd.
De LED knippert volgens het ingestelde BPM, en blijft na de eerste tik van de tapknop constant branden om aan te geven dat het apparaat wacht op de tweede tik.

