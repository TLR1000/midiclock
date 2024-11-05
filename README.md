# Korte versie gebruik van het midi clock controller pedal board:   
- De midi clock controller start op met de laatst opgeslagen waarde.   
- De controller synchroniseert alle aangesloten apparaten continue met een MIDI Out klok signaal.
- **TAP** wordt gebruikt om de nieuwe BPM waarde in te stellen.   
- **SET** maakt de nieuw ingestelde BPM waarde actief op MIDI Out en slaat de waarde op.   
- **+BPM**/**-BPM** past direct de MIDI Out BPM waarde aan, SET slaat de waarde op.
- **START/STOP** start of stopt het afspelen van aangesloten apparaten.   
- **CONTINUE** laat aangesloten apparaten afspelen hervatten indien ze gestopt staan.   

  
# Uitgebreide versie gebruik van het midi clock controller pedal board:   

## About  
Dit project is een MIDI Tempo Controller gebaseerd op een Arduino microcontroller (in dit geval een Wemos D1 Mini). Het apparaat stuurt MIDI-kloksignalen uit via een MIDI DIN-aansluiting, waarmee andere MIDI-apparaten gesynchroniseerd kunnen worden. Het tempo (BPM) kan worden ingesteld met een tap-knop en fijn worden aangepast met plus- en minknoppen. De huidige BPM wordt weergegeven op een 128x32 OLED-display, terwijl LED's visuele feedback geven voor tempo en status van knoppen. De USB-C chassis connector zorgt voor gemakkelijke voeding en programmering van het apparaat.

## Onderdelen en Functies  
tft-scherm: Dit scherm toont het huidige BPM (beats per minute).  
TAP knop: Hiermee stel je het tempo in door twee opeenvolgende drukken. De eerste druk markeert het begin van een beat en de tweede druk markeert de volgende beat, waarna het apparaat het nieuwe tempo berekent.   
SET knop: Hiermee maak je de BPM ingestelde waarde actief voor de MIDI out clock signalen. Een blauwe led geeft aan dat de knop enabled is.  
Plusknop: Verhoogt het BPM met één bij elke druk. Je kunt de knop ook ingedrukt houden om tempo met 1 bpm tegelijk te verhogen.  
Minknop: Verlaagt het BPM met één bij elke druk. Je kunt de knop ook ingedrukt houden om tempo met 1 bpm tegelijk te verhogen.   
Rode LED: De LED knippert op het ritme van het ingestelde tempo en blijft aan na de eerste tap van de tapknop om aan te geven dat het apparaat wacht op de tweede tap.  

## Basic operation
Inschakelen en Opstarten  
Bij het inschakelen van de MIDI Tempo Controller wordt het laatst ingestelde BPM automatisch geladen uit het geheugen en weergegeven op het tft-scherm. Er is geen aan/uit schakelaar, simpelweg stroom op de usb-C ingang zet het apparaat aan.

## Instellen van het Tempo  
 
### Gebruik de knop TAP:  
1. TAP
Druk één keer op TAP om het begin van een beat te markeren. Het scherm toont - - - en de rode led brandt continue om aan te geven dat het apparaat wacht op de tweede tap.   
Druk nogmaals op de tapknop om het begin van de volgende beat te markeren.   
Het apparaat berekent dan het BPM en toont het resultaat op het tft-scherm. Zolang SET nog niet is ingedrukt, kan het tempo onbeperkt gewijzigd worden voordat het actief wordt.  
Wanneer een nieuw tempo gekozen is zal de led boven SET branden om aan te geven dat de SET optie actief is.
2. SET   
Om het nieuwe tempo via MIDI Out te sturen dient SET ingedrukt te worden. De blauwe led licht op indien er een nieuw tempo gekozen is dat nog niet op midi actief is.
De MIDI Tempo Controller stuurt automatisch MIDI-kloksignalen naar je aangesloten MIDI-apparaten, zodat ze synchroon lopen met het ingestelde BPM.
Het ingestelde tempo wordt opgeslagen in het geheugen zodat de controller met deze waarde opstart nadat hij uitgezet is.
 
### Fijn Afstellen van het BPM:  
Gebruik de plus- en minknoppen om het tempo fijnmazig aan te passen:   
Plusknop: Verhoogt het BPM met één stap per druk.   
Minknop: Verlaagt het BPM met één stap per druk.   
  
Let op: in tegenstelling tot TAP zijn de verhogingen/verlagingen van het tempo direct actief op MIDI Out.   
SET slaat deze waarde op in het geheugen zodat de controller met deze waarde opstart nadat hij uitgezet is.    
Wanneer een nieuw tempo gekozen is zal de led boven SET branden om aan te geven dat de SET optie actief is.  

## Start, Stop en Continue

### Gebruik de knop START/STOP:  
De STARTSTOP knop heeft twee functies:

**START**: Stuurt een MIDI START-commando naar de aangesloten MIDI-apparatuur om het afspelen te beginnen.      
**STOP**: Stuurt een MIDI STOP-commando om de aangesloten MIDI-apparatuur te stoppen, terwijl de kloksignalen blijven doorlopen om synchronisatie te behouden.   
De led boven de knop licht op wanneer START commando beschikbaar is.   
Is de led uit dan is de functie van de knop een Midi STOP commando.  

### Gebruik de knop CONTINUE:  
De **CONTINUE** knop stuurt een MIDI CONTINUE-commando naar de aangesloten apparatuur om het afspelen te hervatten vanaf het punt waar het was gestopt. Dit commando is alleen beschikbaar nadat een STOP-commando is verzonden.
De led boven de knop licht op wanneer CONTINUE commando beschikbaar is.    

NB: De controller stuurt **altijd** een MIDI Out klok signaal uit om de klokken van alle aangesloten apparaten te synchroniseren, ook bij gebruik van Stop, Start of Continue.   


