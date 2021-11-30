# Introduction And Processes

### Hva gjør Operativsystemet?

1. Gir pent grensesnitt 
2. Styrer ressursene


## Pointer
- Er en variabel som inneholder en addresse isteden for et tall

``` 
// Eksempel på en peker

INT i = 3  // Opprettes plass i minnet og verdien 3 legges der
Adressen = 0x7FA1 
i finnes ikke, brukes bare i språket i = adressen 0x7FA1


INT *p er en peker // Initsiert peker
p = &i // verdien til p settes til 0x7FA1 (adressen til i)
i // = 3 verdien til i
&i // = 0x7FA1 adressen til i

*p ligger på egen addresse (eks: 0x7F22)
&p = 0x7F22
p = 0x7FA1
*p = 3


```












## OS design goals

#### Virtialisation
- create abstractions

#### Performance
- minimize overhead
- Overhead = ressursbruk
- OS etterstreber så lite overhead som mulig

#### Security
- protect/isolate applications

#### Reliability
- stability / pålitelighet

#### Energy-efficient
- environmentally friendly
- Bruke minst mulig strøm


## Historie

### Før 1970
- 1940 -55 (Flyttet ledninger/direkte maskinkode)
- 1955-65 (Hullkort)
- 1965-70 (Multics, IBM OS/360(the mainframe))

### Unix
- kom 1969
- nedskalert versjon av MULTICS
- POSIX standardisering av UNIX standarden
- 1987 Linus Torvalds laget Linux (utgitt i 1991)


### Windows
- IBM solgte PCer med MS-DOS (Disk Operating System) starten av 80 tallet
- DOS/windows fra 85-95 (kombinasjon)
- Windows95/98/Me fra 95 - 2000
- WindowsNT, 2000, XP, Vista, 7, 8, 10 fra 93-
- SERVER: WindowsNT, 2000, 2003, 2008, 2012, 2016, 2019 fra 93-



### Processes
- En prosess er et kjørende program
- All informasjonen som finnes om et kjørende program


#### Policy vs Mechanisms
- Skille mellom policy om mekanisme
- Linux og Windows gjør dette forskjellig

##### mekanisme:
- OS gjør at program får skifte på å kjøre på CPU
- mekanismen er at flere kan få kjøre
- Kunne la flere program kjøre på CPU

##### policy:
- policy er hvilken program som skal få kjøre
- skal et program ha høyere prioritet ?
- lik prioritet?
- root høyere prioritet ?
- reglene er policy


#### Starte en prosess (Kort oppsummert)
1. OS må hente programmet fra disken
2. OS må lage plass i minnet og legge programmet der
3. OS må reservere plass til data som programmet skal bruke
(kan være på heap eller på stack)
4. hente inn noen biblioteker 
(standardbiblioteker som er delt i minnet)
5. Når lastet inn i minnet vet OS at på addressen der main() merket er skal lastet inn i PC (Program Counter) registeret
6. Så begynner CPU å kjøre å laste inn instruksjonen fra programmet i minnet

#### En prosess har en tilstand
- Lagret i PCB

### Ready que - Running / Ready / Blocked
- Prosesser vil enten bruke CPU, eller så venter de 
- Running "R" (Kjører på CPU)
- Ready "R" (Venter på å slippe til)
- Blocked "S (sleep)" (Har ingenting å gjøre / venter på en kondisjon)







