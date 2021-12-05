# Address spaces and paging

[Bilde som rep system](https://gitlab.com/erikhje/iikos/-/raw/master/img/hw-2-addrspace.png)

### To ting prosesser ikke får lov til å gjører i user mode
- I/O opperasjoner
    - tastatur
    - lese / skrive til disk
- Priviligerte instruksjoner 
    - Endre til kernelmode
    - Aksessere minne
    
##### lese / skrive til minne er ikke I/O!

### Multiprogramming
- Evnen til å laste flere program i minnet samtidig
- multi tasking = Evnen til å skifte mellom programmene i minnet og bruker time interrupt til å avbryte kjørende programmer.
- Det at du aktivt kan avbryte kjørende program basert på timer interrupt kalles pre-emptive
- Dagens OS er alle pre-emptive multitasking Operativsystemer

## Time sharing kalles også multitasking (utvidelse av multiprogramming)
- Prosessorens tid som er delt mellom brukere samtidig  kalles time-sharing
- Gjør at et OS kan ha flere brukere eks:
    - Flere kan SSH inn til samme datamaskin 


### The Address Space
- En abstraksjon av fysisk minne
- Kalles (process)address space
- Er hvordan et kjørende program ser på minnet i systemet
- program skal ikke behøve å bry seg om hvor det er plassert i minnet, skal forholde seg til at det har et minneområde som det ser, så kan operativsystemet palssere det hvor det vil i minnet.

##### Problem
- HOW TO VIRTUALIZE MEMORY <br/>
How can the OS build this abstraction of a private, potentially large address space for multiple running processes (all sharing memory) on
top of a single, physical memory?

##### Løsning
- Virtuelt minne

#### Mål for virtuellt minne:
- Være transparent for prosessen (skal ikke vite at minnet er virtuelt, prosessen utfører som om den har hele minnet for seg selv)
- Være effektivt, kan ikke ha masse tidkrevende behandling hver gang en prosess skal aksessere minnet
-V ære beskyttet slik at flere brukere jobber på samme maskin, skal ikke skrive til en annens minne, eller lese passordet til en annen som er lagret i minnet.

### Typer minne:

#### Stack minne 
- Noen ganger kallt automatic memory fordi allokering og deallokering er blir gjort av kompilatoren
- Allokering på stack er lett, Eksempel:
```
void func() {
        int x; // declares an integer on the stack
}
```
- kompilatoren allokerer minne på stacken når vi kaller func()
- Når vi returnerer fra func() deallokeres minnet
- Om du ønsker noe informasjon tilbake fra funksjonen, så bør den ikke bli værende på stack.
- Derfor trengs long lived memory, kallt heap

#### Heap minne
- Noen ganger kallt long lived memory 
- Kan også være at det ikke er plass på stacken
- Her blir allokering og deallokering gjort av programmereren
- Allokering på heap, Eksempel:
```
void func() {
  int *x = (int *) malloc(sizeof(int));
  ...
}
```
- (int *) forteller kompilatoren at den skal gjøre plass til en pointer som peker til en integer  (kalles casting og er ikke nødvendig)
- malloc(sizeof(int)) ber om plass til en integer på heapen



##### malloc()
- Om du ikke allokerer nok plass på heapen, så kan du få en buffer overflow
- malloc() gis en størrelse (i bytes) som allokeres på heap
    - Returnerer en pointer til frigjort minne
    - Eller NULL om den feiler
```
malloc(sizeof(int)) allokerer 4/8 bytes (størrelsen til en int ) på (32/64 bits maskiner)
malloc(10) allokerer 10 bytes
```

##### free()
- For å frigjøre minnet som ikke lenger er i bruk kan free() kalles
```
int *x = malloc(10 * sizeof(int));
  ...
  free(x);
```


## Plassering av variabler eksempel:

```
#include<stdio.h>
#include<stdlib.h>

int main(void) {
        int x,y;        
        int *h = malloc(sizeof(int));  
        printf("y&x is on stack at %p and %p while h is on heap at %p\n",&x,&y,h>
        return 0;
}

Output:
y&x is on stack at 0x7ffd298b4768 and 0x7ffd298b476c 
while h is on heap at 0x55eb184a82a0

```
- Her kan vi merke oss 2 ting:
1. x&y har nesten lik addresse (stack)
2. h har helt forskjellig (heap)


- Adressene er et hexadecimalt tall 
- 0x viser bare at det er hexadecimal
- 0x(0-f) = 2^4 = 16 = 4 bit = (0000 - 1111) 

### 64bit / 32bits operativsystem
- Sier hvor mange adresser man kan adressere i RAM
- Jeg har 64 bits operativsystem
    - Da skulle man trodd at adressene hadde en lengde på 16, siden 16*4 = 64
    - Men de har bare en lengde på 12 (12*4 = 48)
    - Selv om OS er 64 bit, så bruker man bare 48 på linux og windows
    - Fordi 48 but er mer enn nok, kan adressere 256 terrabyte RAM (mer enn vi noen gang trenger)
    - 32 bit kan adressere 4 gigabyte som ikke er nok.


## User space / kernel space i minnet
- vi ser at adressene fra stack som ligger nederst i adresseområdet starter med 7
- Vi vet at med hex kan vi adressere til 16
- Adressene bruker bare halvparten av adresseområdet fordi den andre halvparten av adressene tilhører kernel space

- Minneområde som en prosess ser er delt i 2
- Halvparten 0x0 - 0x7ffxxxxxx =  user space 
- Halvparten 0x7fffxxxxxx - 0xffffffff = kernel space

Kernel space mappet til operativsystemet sin del slik at mode-switch skal gå raskest mulig.


#### Eksempel på adresser:
```
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("location of code : %p\n", main);
    printf("location of heap : %p\n", malloc(100e6));
    int x = 3;
    printf("location of stack: %p\n", &x);
    return 0;
}

Output:
location of code : 0x558ef3e02189
location of heap : 0x7f600a635010
location of stack: 0x7fffa1c13944

Ser at main() ligger litt lenger ned i minnet

Output andre gang koden kjøres: 
location of code : 0x56110a97c189
location of heap : 0x7f93ef53a010
location of stack: 0x7fff95b1e244

Ser at adressene er litt endret PGA adress space layout randomisation som er en sikkerhetsmekanime


```

### Fysisk adresse
- Physical address = virtal address + base
#### Dynamic (Hardware based) Relocation
- Adress translation via Base & Bound
- 2 register i CPUen sitt Memory Management Unit (MMU)
    - baseregister: start (sjekker at jeg starter der jeg er tilordnet plass i minnet)
    - bound: slutt (sjekker at jeg ikke går utenfor tilordnet plass i minnet)



## NB! Vi addresserer alltid til en byte!
- Memory adresses are in BINARY <b>bytes</b>!
- adress 1234 is the 1234th byte, not the 1234th bit

```
A 32-bit integer is 4 bytes

for example 258 in binary is:

                    <---98 76543210

00000000 00000000 00000001 00000010 
                         ^       ^
                         |       |
                        2^8  +  2^1
                    =   256  +   2
                    =       258 
```



- en int er 4 byte stor
    - bruker da 4 adresser i minnet

### Example process with a 4KB adress space 
- Loaded into physical address 16KB (base)

 ### Virtual adress ---------------->   Physical address
        0 ---------------->         16KB   (base er her 16 KB)
        1KB -------------->         17KB
        3000 ------------->         19385b (over 19KB)
        4400 ------------->         Fault (out of bounds)

- Hva betyr det at dette skjer i hardware?
    - Du kan bygge ting i hardware som gjøre en oppgave uten software
    - Eks en datamaskin på lavt nivå som:
        - legger sammen primtall fra 1 - 1000 og si pip når ferdig
        - Kretser AND, OR, NAND, NOR, etc..

- Cluet er at å legge til Base-adressen til Virtial-adress for å få Physical-address, skjer i hardware (MMU i hver CPU)




### Segmentering (Brukes ikke)

- Betyr at en prosess har flere adresserom (som kan være forskjellig sted i minnet)

- Hvor mye plass skal man sette av på heapen?

- Deler opp adresserommet i 3
    1. For Coden
    1. For Heapen
    1. For Stacken
- Disse 3 kan være på forskjellig sted i minnet 

- Finnes ikke på 64 bit OS


### Memory Sharing
- Deler av minnet kan deles
- Eks GUI, hver enkelt program trenger ikke å laste inn knapper og vindusrammer i sitt eget adresseområde fordi det er delt.
- Skal man dele, så må noe være delt og noe må iikke være delt
- beskyttelsesbits: 
    - rwx (read, write, execute)
    - r-- (read only)



## Paging

#### Begrep: 

- Internal fragmentation (problem for variable sized units)
    - Bortkastet plass inni enheter(eks prosesser)
    - Er at en liten int kan ta opp en hel blokk i minnet
    - Løser problemet med Paging
- External fragmentation (problem for fixed size units)
    - Bortkastet plass mellom enheter(eks prosesser)
    - Er når du har ting lagret på en sånn måte at det slåser plass mellom prosesser i fysisk minne


### Free space management
- Hvordan holde orden på ledig plass
- Strategies for all storage (memory and disks)
- 2 alternativer
    1. Bitmap
    2. Free list


#### Free list:
- Blokkene er nummerert 1, 2, 3, 4, 5, 6, 7, 8 etc...
- Om blokk 6, 7 og 8 er ledig, så er free list en datastruktur som sier: 
    - Freelist: 6, 7, 8
    - evt fra 6 - 8
Bare en liste over hva som er ledig

Tar mye mer plass enn bitmap, en int tar 4 byte med plass, men muligens raskest å søke i

#### Bitmap:
- Blokkene er nummerert 1, 2, 3, 4, 5, 6, 7, 8 etc...
- Om blokkene er ledig eller ikke er representert med en bit
- 1 betyr ikke ledig
- 0 betyr ledig
- Om blokk 6, 7 og 8 er ledig, så er bitmap en datastruktur som sier: 
    - 1 2 3 5 6 7 8 etc...
    - 1 1 1 1 0 0 0 
- Om blokk 6, 7 og 8 er ledig, så er free list en datastruktur som sier: 

- Mest kompakt og tar minst plass, men må itterere gjennom hele(tregere å søke i)


Eks: B = Byte , b = bit

```
Har 2GB RAM, som er delt inn i 1KB blokker.
Hvor stor blir bitmapen ?

2GB / 1KB = 
2^31B / 2^10B =
2^21b =
2^18B =
256KB

```

# Paging, Det som vi faktisk gjør

- Tar RAMen vi har og deler den opp i faste størrelser som vi kaller for en page (vanlig 4KB)
    - Virtual memory = virtual page
    - Physical memory = Page frame
- Mekanismen kalles paging
- Det som Programet ser kalles page og er en fast størrelse
- En page plasseres i RAM og blir da kalt page-frame


### MMU 
- har TLB cache
- har PageTable-Register (Inneholder en adresse inn til fysisk minne hvor den aktuelle prosessens Page-Tabell ligger)

##### TLB cache
- Se Memory Management Notes.md

##### PageTable-Register
- heter CR3 på x86

#### Adress Translation med paging
- Oversetting er bare et tabelloppslag i PageTable-register av MMU
- Hver prosess har hver sin Page Tabell som beskriver hvordan alle adressene skal oversettes 

- <b>Merk!</b> Når CPU skal sende en adresse til minnet:
1. MMU slår opp i Page-Tabellen i RAM (adressen ligger i registeret (hentet fra RAM))
2. Oversetter den aktuelle adressen til en fysisk adresse
- Dette skaper illusjonen om at hver prosess har sitt eget adresserom

[AddressTranslation](https://gitlab.com/erikhje/iikos/-/raw/master/img/addrtrans.png)

### Hvordan MMU jobber når den oversetter
- Oversettingsmekanismen

16 bit Virtual Address (1110 1001 0001 1110)
(64KB Virtual Address Space)

Tar Så mange bit som trengs for å representere page størrelsen som offset
Om page size er 4KB, trengs 12 bits (2^12 = 4068b = 4KB)

- Adresse: 1110 1001 0001 1110
    - Første 4 er Virtual page number (VPN = 1110)
    - Siste 12 er Offset (1001 0001 1110) 
    - Offset forblir det samme i den fysiske adressen

MMU:
1. Ser i Page-Table register og finner adressen inn i fysisk minne hvor page table ligger.
2. Leter opp VirtualPageNnumber i PageTable(som tilhører prosessen)
3. 1110 = 14 ser på nr 14 i page table og finner Page-Frame nummer
4. I dette eksempelet er page frame nummer = 101
5. Setter page frame nummer først = 101
6. Legger på offset som er 1001 0001 1110
7. Den fysiske adressen blir da 101 1001 0001 1110


#### Illusjonen: 
- Hver prosess tror den har alt minnet
- Page tabell mapper Virtual Address -> Physical Address
Page Tabell har i tillegg til Page Frame Number  en presentbit
- Presentbit sier om adressen er mappet til en Physicall *
Address 1 er mappet, 0 er ikke mappet
- Dirtybit indikerer at pagen er skrevet til, og endringen ikke er lagret på disk
- R/W bit = aksesskontroll
- Page tabell ligger i RAM og må lastet inn i PageTable-register for hver prosess på CPU

 






























    
