# I/O and HDD / SSD

- All I/O må gjøres av OS
- Operativsystemet må inn i bildet når vi skal gjøre I/O
- Systemkall er en del av I/O
    - Skrive til disk open(fil)


### Addresser
- Minne addresser 
    - virtuelle  oversettes til fysiske av CPU (i MMU)

- I/O enhetene må også ha adresser for å nå dem
    - Må ha et tilsvarende addresserom

#### Buser
- Er ledninger
- Må settes sammen for å få best mulig ytelse
- PCIe Graphics
- Memory Interconnect
- PCIe 
- eSATA
- USB

```
     PCIe Graphics  Memory Interconnect
[Graphics] <--> [CPU] <--> [Memory]
                  ^
          PCIe    |       eSATA
[Network] <--> [I/O Chip] <--> [Disk]
                    ^
                    | USB

            [Keyboard][Mouse][etc..]
```

#### I/O enheter 
- Harddisk
- Keyboard
- Mouse
- Nettverkskort

    ```
    Eksempel på en I/O enhet:
    Ganske standard for alle I/O enheter
    _______________________________________
    | Registers: [Status] [Command] [Data] | Interface
    | _____________________________________|
    | Micro-controller (CPU)               |      
    | Memory (DRAM / SRAM or both)         | Internals
    | Other Hardware-specific Chips        |
    |______________________________________|
    ```
- Status: Register som jeg kan spørre om den er ledig
- Command: Register som jeg kan skrive kommandoen jeg ønsker å få utført inn i 
- Data: Stor Cache som vi kan skrive data inn i

#### Når vi skal prate med I/O enheten

#### Programmert I/O (PIO)
- Typisk interaksjon mellom OS og I/O enhet
- Polling
- Bra om ventetiden på I/O er lav

```
While (STATUS == BUSY)
       ; // wait until device is not busy
   Write data to DATA register
   Write command to COMMAND register
       (starts the device and executes the command)
   While (STATUS == BUSY)
       ; // wait until device is done with your request
```

#### Interrupt drevet I/O
- Bedre (lavere overhead)
- Isteden for at CPU venter(poller) på disk, så kjøres en interrupt og en context switch
- Bra om ventetiden på I/O er lang
- Context switch koster.

1. Programmert I/O: p = polling, 1 = process 1
```
CPU     [1][1][1][1][1][p][p][p][p][p][1][1][1][1][1]
Disk                   [1][1][1][1][1]
```

2. Interrupt drevet I/O: 1 = process 1, 2 = process 2
```
CPU     [1][1][1][1][1][2][2][2][2][2][1][1][1][1][1]
Disk                   [1][1][1][1][1]
```
- På denne måten kan CPU kjøre arbeid på process 2, mens disken gjør arbeidet for process 1 sin request.


### DMA (Direct Memory Access)
- Store I/O jobber bruker vi DMA kontrolleren
- Da outsourcer du hele jobben

- Eks: laste et stort bilde inn i minnet
    - Bruker da DMA kontrolleren
    - "Prat med disken og flytt disse dataene inn i minnet, og si ifra når du er ferdig"


### Metoder for device interaksjon:

- Hvordan kontakte en I/O enhet?
- 2 metoder:
    1. I/O instructions (isolated I/O):
    - Instruksjoner med et adresseområde basert på porter
    - (Likt TCP/UDP porter)
    - Antall portnummer i TCP/UDP = 2^16 = ca 65 000
    - 16 bits portnummer
    - sudo cat /proc/ioports  (Output:)
    ```
    0000-001f : dma1
  0020-0021 : PNP0001:00
    0020-0021 : pic1
  0040-0043 : timer0
  0050-0053 : timer1
  0060-0060 : keyboard
    ```

    2. Memory-mapped I/O
    - Bruke fysiske addresser
    - De som ikke er brukt av RAM
    - Og mappe disse til register i I/O-enheter
    - Så kan vi rebruke instruksjoner som "mov sudo cat /proc/iomem"
    - Kan bruke samme instruksjoner (assembly) som vanlig mot I/O når man har memory mapped I/O

```
Porter / MMap

Vitruelle       | Fysiske 
adresserommet   | addresserommet


        VA          PA
[CPU] ------>[PT]---------> [RAM]
  |                            |
i/o porter               memmapI/O
  |                            |
[I/O-enhet]             [I/O-enhet]
```

- mmap er en del av det fysiske adresserommet
    - Ikke virtuelle
    - OS spiser halvparten av det virtuelle, men ikke det fysiske
- Som regel bruker man begge



### Storage devices (Blokk-enheter) 
- På Linux: Alt som vi kan lagre data på, kalles blokk-enheter

- Address space: n sectors from 0..n-1

- Sectors are traditionally 512B, sometimes physically 4KB (but then emulate 512B)

- Unfortunately what sector, block and page means depends on the context, be aware!

```
$>cd /dev/
$>ls -l


brw-rw----  1 root   disk      8,   0 Dec  2 00:47 sda
brw-rw----  1 root   disk      8,   1 Dec  2 00:47 sda1
brw-rw----  1 root   disk      8,   2 Dec  2 00:47 sda2
brw-rw----  1 root   disk      8,   3 Dec  2 00:47 sda3
```

- Alt som har "b" forran er en blokk-enhet
- sda er disken
- sda1, 2 og 3 er partisjoner

### Hvordan ser OS lagringsenheter?

- Alle ser like ut Harddisk / SSD / usb etc...
- Ser en array med sektorer:

 0 1 2 3 4 5 6 7 8 9 ....
[ | | | | | | | | | | | | | | ]

- sektor 0, sektor 1, sektor 2, etc...

- Fra gammelt av var en sektor alltid 512b
- Nå er en sektor enten 512b eller 4KB

- "Jeg klarer å lagre 512b med data atomisk"


#### I/O Time: HDD Doing the Math (Må kunne!)
Ti/o = Tseek + Trotation + Ttransfer (= aksesstid)

Tiden det tar å hente noe data på en harddisk:

Tseek = Tiden som armen bruker på å bevege seg inn og ut på platen
Trotation = Tiden som platen bruker på å rotere til riktig sted
Ttransfer = Tiden disken bruker på å lese av dataene

```
Eksempel, en disk har 7200Rpm SATA

Iløpet av 60 sekunder, så roterer den 7200 ganger
60 må deles opp i 7200 runder pr sekund

En runde tar: 60 / 7200 = 0.0083333.. sekunder 
1 sekund = 1000millisekunder
0.0083333 * 1000 = 8.333 msec
Denne disken bruker 8,33 msec på å rotere platen en gang rundt

Gjennomsnittlig aksesstid er for random aksess = halvparten
8.33 /  2 = 4.151333...
Tseek + Trotation = 4.15
Ttransfer er uninteressant fordi det er snakk om mikrosekunder.
```

### HDD
- Lagres magnetisk
- Tregere enn SSD
- Lagrer data trygt i 10 år 
- Er ikke utdatert




### SSD (Solid State Drive)

- NB! i HDD kalles lagringsenheten for "Sektor" 
    - En blokk består av mange sektorer

- NB! i SSD kalles lagringsenheten for en "Page"
    - E blokk består av mange pages
    - Må ikke forveksles med en page i RAM!!
        - Du kan lett lese/skrive til en page og-
        - Du kan lett lese/skrive til en page som er tom
            - Men du kan ikke overskrive en page
            - Da må du først slette hele blokken!
                - Må derfor:
                1. kopiere ut alle andre data som er der 
                2. For så å slette hele blokken
                3. Så skrive til page inne i blokken

- I SSD lagrer du data som spenning i transistorer
- Må ha strøm tilkoblet for å lagre data trygt
- Kan miste data etter så lite som 1 år (uten strøm)
    - Kompenserer for dette problemet H.V.A. software
    - Siden hver I/O enhet er en "datamaskin" så trengs en-
    - Flash Translation Layer, for å unngå data slitasje
        - Hver gang vi skriver til samme fil, så skriver jeg 
        - til et nytt sted
- Flash Translation Layer:
    1. SSD har oversikt over ledige (tomme) blokker
    2. Det kommer en write forespørsel
    3. Kontroller velger en ledig blokk
    4. Samtidig ser den på alle blokkene med data på disken
    5. Så velger den den blokken som har minst antall valid pages
    6. Tar med de data som er valid i den blokken
    7. Skriver de til ledig blokk, sammen med write forespørselen
    8. Sletter blokken som bare har invalid date igjen
    9. Frigir den slettede blokken
    - Dette gjør at SSD frigir ledige blokker hele tiden
    - Og at data blir reinitsialisert som forhindrer wear
    - Litt ekstra skriving, men gjør wear leveling.
    - Jo større SSD, jo bedre PGA at du fordeler skrivingen på hele disken (siden du kan få wear etter 4000 skrivinger) så er det ikke noe problem pga wear leveling



- Lagres i celler (1 cell = en transistor)
 - 1 bit (0 / 1) = single-level cell (SLC)
 - 2 bit (00 / 01 /10 / 11) = multi-level cell (MLC)
 - 3 bit finnes også = triple-level cell (TLC)

- SLC har bedre performance, men er mye dyrere

#### Trim:
- Når man sletter en fil på disk, så kan man finne den igjen
- Fordi man ikke sletter, men markerer den som ledig (invalid)
- Trim forteller disk kontrolleren hva filsystemet har slettet
    - og hvilke tillhørende blokker på disk
    - Så disken kan frigjøre de blokkene
```
$>man fstrim
```

### RAID (Redundant Array Of Independant Disks)
- RAID er en Måte å kombinere disker på, så de ser ut som 1


##### RAID 0 Striping (ikke sikkerhet)
- 2 disker som oppfører seg som 1
- Ingen redundans, men dobbel kapasitet
- 2x 256GB disker = 1x 512 GB disk
- Ryker en disk, så ryker alt

 ______   ______
|  a1  | |  a2  |   ---> skriver i en stripe
|______| |______|
|  a3  | |  a4  |
|______| |______|      
|  a5  | |  a6  |
|______| |______|
|  a7  | |  a8  |
|______| |______|
|  ax  | |  ax  |
|______| |______|
|      | |      |
|______| |______|



##### RAID 1 Mirroring
- 2 disker som oppfører seg som en
- alt du skriver i a1 Disk 1 blir speilet i a1 Disk 2
- Har redundans (samma data lagret i hver blokk)
- 2x 256GB disker = 1x 256GB disk

Disk 1    Disk 2
 ______   ______
|  a1  | |  a1  |
|______| |______|
|  a2  | |  a2  |
|______| |______|      
|  a3  | |  a3  |
|______| |______|
|  a4  | |  a4  |
|______| |______|
|  ax  | |  ax  |
|______| |______|
|      | |      |
|______| |______|


##### RAID 5 Striping with parity spread across drives

- Kombinasjon
- Forsøker å få god leseytelse
- Ungå at alt blir tapt, om en disk ryker

- p = paritetsinformasjon informasjon som gjør at du kan rekonstruere data
- for at det ikke skal bli en disk som bare har paritetsinformasjon, så sprer man denne på alle disker

- Når en disk ryker i RAID 5, så tar det som regel veldig lang tid å rekonstruere (Siden diskene nå er så store)
    - Derfor har man nå vanlivis raid 6, som har 2 paritetsdisker

Disk 1    Disk 2   Disk 3    Disk 4  
 ______   ______   ______   ______
|  A1  | |  A2  | |  A3  | |  Ap  |
|______| |______| |______| |______|
|  B1  | |  B2  | |  Bp  | |  B3  |
|______| |______| |______| |______|     
|  C1  | |  Cp  | |  C2  | |  C3  |
|______| |______| |______| |______|
|  Dp  | |  D1  | |  D2  | |  D3  |
|______| |______| |______| |______|
|      | |      | |      | |      |
|______| |______| |______| |______|
|      | |      | |      | |      |
|______| |______| |______| |______|



##### JBOD (Just A Bunch Of Disks)
- Ingen RAID, bare om du ønsker å fylle en disk, så neste, så neste etc..
- Ser ut som en dis



#### Speed comparison SSD / HDD

<table>
<th>Device</th><th>Random</th><th>Sequential</th>
<tr>
<td></td>
<td>reads/writes (MB/s)</td>
<td>reads/writes (MB/s)</td>
</tr>
<tr>
<td>Samsung 840 Pro SSD</td>
<td>103/287</td>
<td>421/348</td>
</tr>
<tr>
<td>Seagate 600 SSD</td>
<td>84/252</td>
<td>424/374</td>
</tr>
<tr>
<td>Intel SSD 335 SSD</td>
<td>39/222</td>
<td>344/354</td>
</tr>
<tr>
<td>Seagate Savvio 15K.3 HDD</td>
<td>2/2</td>
<td>223/223</td>
</tr>
</table>

- Ser har at nå du har mye random skriving, så må man ha SSD
- Mekanikk er veldig tregt

```
Enkel test: 
T lester fra RAM (cached reads)
t fra disk (buffered disk reads)

$>sudo hdparm -Tt /dev/sda

Output:

 Timing cached reads:   26830 MB in  2.00 seconds = 13433.28 MB/sec
 Timing buffered disk reads: 1856 MB in  3.00 seconds = 618.22 MB/sec
```

- Så mye raskere er RAM enn SSD 
- ca 21 ganger raskere på min maskin


### IOPS (Input / Output Per Second)

- Hvor mange I/O operasjoner klarer man i sekundet
    - Rules of thumb: (for å se ca ha som er normalt)
    - RAM: ca 500K +
    - SSD: ca 10K +
    - HDD: ca 100 - 200 

- Ytelsestester: (IKKE PENSUM)
    - Sequential read - Enkleste
    - Random write - Vanskeligst
    - Random read / write (God test for benchmarks)
        - Er den vanligste opperasjonen til en disk
        - Skal man gjøre en skikkelig test mot disk, så tester man alle kombinasjoner

