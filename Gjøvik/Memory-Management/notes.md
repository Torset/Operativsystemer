# Memory management


[Bilde som rep system](https://gitlab.com/erikhje/iikos/-/raw/master/img/hw-2-addrspace.png)

### Faster translations

- Paging is wonderful, but have 2 problems
1. Slow
2. 


## TLB-cache
- Ligger i MMU
- Translation lookaside buffer
- Når MMU skal hente en page, så gjør den en liten "lookaside" for å se om den ligger der, slik at den slipper å hente fra RAM, som tar mye lengre tid
- Cache kalles også buffer (fra gammelt av)

```
$>cpuid -1 | less
/tlb
Output:

cache and TLB information (2):
      0x63: data TLB: 2M/4M pages, 4-way, 32 entries
            data TLB: 1G pages, 4-way, 4 entries
      0x03: data TLB: 4K pages, 4-way, 64 entries
      0x76: instruction TLB: 2M/4M pages, fully, 8 entries
      0xff: cache data is in CPUID leaf 4
      0xb5: instruction TLB: 4K, 8-way, 64 entries
      0xf0: 64 byte prefetching
      0xc3: L2 TLB: 4K/2M pages, 6-way, 1536 entries

```
- Ser at det er en data, instruksjon og cache TLB,
- TLB er i L1, og L2



### Hvordan TLB fungerer
1. Vi vet at en Virtuell adresse deles inn i pagenr o offset
2. Offset blir med til fysisk minne
3. pagenr bruker vi til å slå opp i page tabellen
4. Med TLB så gjør vi en ting før vi slår opp
5. Vi sjekker TLB om pagenr ligger der
- Hit
7. Gjør den det, har vi en TLB hit og pagenr mapper til framenr
8. framenr + offset = fysisk adresse, å vi kan hente rett fra RAM
- Miss
7. Om pagenr ikke ligger i TLB, er det en TLB miss
8. Da hentes framenr fra page table i RAM 
9. framenr legges i cachen
10. CPU prøver på nytt (henter framenr fra cache) og får en TLB hit

- TLB miss koster mye, fordi man må traversere Page table



#### Caching virker pga gjenbruk


- Spatial locallity
    - Samlokalisert i rom
    

- Temporal locallity
    - Samlokalisert i tid

Caher er raske, men må derfor være små

- On x86, Hardware handles TLB
    - Skjer av seg selv i hardware


### Hva er en TLB entry?
- En kopi ab Page Table Entry
- Address Space Identifier (on modern architectures to avoid TLB flush on every context switch)

- Når vi skifter mellom kjørende prosesser, så må cachen tømmes
- Så må den fylles igjen (skjult kostnad ved context switch)
- Om hver entry i TLB er tagget med hvilken prosess den tilhører, så må vi ikke fushe cache.
- Her kommer Address Space Identifier(ASID) inn
- TLB har et egett felt med ASID
    - Disse entryene tilhører denne prosessen
    - Gjør det mulig for en prosess å finne igjen sine entries i TLB etter en context switch. Og gjør at cache ikke trenger å flushes.



### Page table entries, FORSKJELL PÅ 16 og 32 bits addresser

- 16 bits adresser har 2^4 antall page table entries når page size er 4KB

som vi husker: 1111 | 0100 1010 0011
              pagenr|offset

- 32 bits addresser har 2^20 antall page table entries når page size er 4KB (offset er det samme)

0010 1010 1010 1010 1111 | 0100 1010 0011
                   pagenr|offset

DVS at vi kan mappe 2^20 = 1 048 576 pagenr til framenr i page tabellen

Dette tar veldig mye plass i RAM


### Page table tar veldig mye plass

- Større pager = færre linjer i page tabellen (4kb / 2MB / 1GG)
    - Større pager gir mindre page tabeller
    - men gjør at intern fragmentering blir et problem
    (mage pages vil ha lite data i seg)

- Løsning: 
#### Multi level page tables


- Siden page tabeller har veldig mye ledig plass, mappinger som er tomme

- Page table register (CR3) peker her på et page directory, som sier hvor page table er

- Er en slags index

#### Eksempel
[AddressTranslation](https://gitlab.com/erikhje/iikos/-/raw/master/img/addrtrans.png)
- Siden 4 bit gir mulighet til (16 - 12(offset)) = 4b = 2^4 = 16 tall, så er det bare mulighet for 16 VPN (Virtual Page Number) i PT til en 16 bits addresse

- Om du har 48 bits adresser og 4KB page-size, gir det mulighet for 48 - 12(offset) = 36b = 2^36 VPN 
    - Dette gjør at PT adressere 2^36  adresser
    - Om hver entry i PageTable er 64 bit = 8B = 2^3B
        - Så får vi en PageTable som er 2^36 * 2^3B = 512 GB
        - Dette går ikke, så bi må ha multi level PageTable

- Si at vi har en 32 bits adresse (isteden for 16 som på bildet over)
- Siden page størrelsen fortsatt er 4KB, så endres ikke lengden på offset
    - Men du får flere bits forran som gir mulighet til flere PT-entries
    - Og PD (Page Directory (Multi level page tables)) 

1111 1111 11|00 0000 0000 | 0000 0000 0000
(PD bits)       (PT bits)       (offset)

1. Tar de første 10 bit (1111 1111 11) og slår opp i en tabell (Page directory). Fra denne får vi page-table-adressen vi skal bruke
2. Bruker page-table-adressen til å finne hvilken page table vi skal bruke.
3. Bruker de neste 10 bits (00 0000 0000) til å slå opp i Page-Table for å finne Page-frame nummeret som peker ut en 4KB page frame

- Denne prosessen tar opp ca 4 KB isteden for 4 MB
- TLB cacher kun Page Table mappingen


### 64 bit er samme konsept (48 bit)
- Bare at PD mapper til et ett eller flere underliggende PD som mapper til PT
- 64 bit deler man i 4, 3 PD og en PT
- PD -> PD -> PD -> PT

```
EKS 64bit bruker 48 bit

VirtuellAdresseBits = bit48-39 bit38-29 bit28-19 bit18-9 bit8-1

bit48-39 - slår opp i PD1 som gir adressen til PD2

bit38-29 - slår opp i PD2 som gir adressen til PD3

bit28-19 - slår opp i PD3 gir adressen til PT

bit18-9 - slår opp i PT som gir PageFrameNR

PageFrameNR + bit8-1 = Physical address


Hver PD har 2^9 adresser

```

- Når vi deler opp slik, så behøver vi bare å lage de PageTablene som vi trenger (Det vi faktisk bruker)

## OM TID SE FERDIG MEMORY MANAGEMENT DEL 2

## Sjekk opp dirty COW





010 0000 0000 1100

2^13 + 2^2 + 2^3 = 8204


    


    