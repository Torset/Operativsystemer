# Computer architecture:

[Bilde som rep system](https://gitlab.com/erikhje/iikos/-/raw/master/img/hw-2-addrspace.png)

## CPU 

``` Henter / lagrer ting i minnet ```

### Registers 
<b>AX, BX, CX ...</b> (RAX = 64bit AX, EAX = 32 bit AX, AX = 16 bit)

<b>PC/IP</b> (Inneholder adressen inn i minnet, til neste instuksjon som skal utføres)

<b>IR</b> (Inneholder selve instruksjonen som skal utføres)

<b>SP, BP</b> (Stackpeker, Basepeker) 
SP adressen til toppen av stacken, base peker til bunnen

<b>PWS/FLAG</b> (Kontrollregister FLAG)

```Registre er den minste enhet i datamaskinenhvor man kan lagre data, her lagrer man typisk litt data fra 8 til 64 bit. Holder typisk et tall eller en adresse```

### Control Unit
    (CU)

### Arithmetic Logic Unit
    (ALU

### Memory Management Unit
    (MMU)
    CPU cache


## Memory
```Det som befinner seg i minnet: data og instruksjoner```
- Hele minnet er delt inn i linjer på 64 byte



## I/O (Input/Output)



## Instructions

A simple c program:
```
int main() {
    int a = 2;
    int b = 3;
    int a = a + b;
    return a;
}
```

Expressed somewhat like so:
- Store the value 2 somewhere (called a)
- Store the value 3 somewhere (called b)
- Add the value of b to a
- Return the value of a

Expressed in assembly:
- movl eax, 0x2
- movl ebx, 0x3
- addl eax, ebx
- ret

<b>instruction:</b> mov, add, ret
<b>sufix:</b> movl, l=32bit, b=8bit w=16bit, q=64bit
<b>oerand:</b> $, % ($0 = tallet 0, %=register Eks: %eax)
<b>paranteser:</b> (-4%ebp, %eax) = et sted i minnet. ## Load what is located in address(ebp -4) into eax

Eksempel:
```
movl    $0, -4(%rbp)        ## Legger tallet  0 i minnet
movl    -4(%rbp), %eax      ## Leser 0 fra minnet og legger det i AX registeret
```



### x86 arkitektur
- instructions (bestemt sett, ca 2000)
- registers (hvilke register finner)


### Regular instructions
- Moving data (mov)
- Math functions (add, sub)
- Function related (call, ret)
- Jumping (jmp, je, jne)
- Comparing (cmp)
- Stack (push, pop)


### CPU workflow

```
Instruction cycle AKA fetch, (decode,) execute cycle

Pseudocode:

while(not HALT) {   # while powered on
    IR=Program[PC]  # fetch instruction pointed to by PC to IR
    PC++            # increment PC
    execute(IR)     # execute instruction in IR
}

på 1ghz CPU kjøres denne løkken 1 millard ganger pr sekund

```

### CPU workflow with interrupts

```
while(not HALT) {   
    IR=mem[PC]          # IR = Instruction Register
    PC++                # PC =Program counter
    execute(IR)    
        if() {          # IRQ = Interrupt Request
            savePC();   
            load(IRQ);  # Hopper til interrupt-rutine
        }
}

```

###  Registers vs Physical memory (RAM)

NB! Register contetnts <b>belong to the current running program</b> and the OS

There can be several programs loaded into memory (Multiprogramming), men det programmet som kjører akkurat nå på CPU, eier registrene

Stopper du et program, så lagres registrene og registrene til det nye programmet lastes inn


### Et program i minnet 

## stack 
- int variabler. 
- er en datastruktur som du kan pushe ting oppå, og poppe de av.
- vokser nedover

## heap
## bss - uninitialized data
## data - initialized data
## text - selve instruksjonene




## Important concepts
- Clock speed/rate (1 gigahertz = 1 milliard ganger i sekundet)
- Pipeline, superskalar ( Det betyr: Laster en instruksjon -> Dekoder -> eksekverer)



### Hyperthreading

- Hver CPU-kjerne har dobbelt opp av registere
- Kan laste 2 programmer samtidig og skifte lynraskt mellom de to
- Kan bare regne på en om gangen (1 ALU)
- En CPU kjærne kan holde tilstanden til to programmer samtidig



### Access times
- CPU (0.5ns) Aksessere et register og utføre en 
- L1 cache (2ns) 
- L2 cache (10ns)
- L3 cache (20ns) 
- Minne-RAM (60ns) 
- SSD (100us)
- NIC (10ms)


### Cache

- cache er veldig effektivt på repetisjoner (løkker)
- En rask cache må være liten
- virker ikke første gang man aksesserer data, men ved gjenbruk
- Legger igjen en cacheline som som regel er 64 byte
    - en int er 4 byte


## Cache Locallity of reference
- Data gjenbrukes i tid og rom
```
Minnet er delt inn i linjer på 64 byte
Henter en linje til cache, som kalles en cacheline (typisk 64 byte)
Siden en INT er 4 byte, så kan man cache 16 INT
```
### Write policy

Eksempel: 
```
En minnepenn
Bruker man write-back, så kan det være at det som er skrevet ikke er lagret  på minnepenn enda, men ligger lagret i en cache/ramcache
Fjerner man minnepennen, så kan man risikere datatap
Derfor OS ber om "Safely remove hardware"
```

```
En minnepenn
Bruker man write-through
Så er det skrevet umiddelbart, men skrivingen går tregt
```

#### Write-through 
(Write to cache line AND immediately to memory)
- Gir dårlig ytelse
- Trygt og enkelt
- Slipper synkroniseringsproblemer


#### Write-back 
(Write to cache line and mark cache line as dirty)
- Endrede data som skal skrives ut kalles dirty
- Når OS ber om safely remove hardware, så sjekker det om det ligger dirty data, som må skrives før man fjerner minnepennen

```
write back er spessielt utfordrende når det er flere CPU-kjærner
Flere kjærner, mer synkronisering og ytelsen flater ut
Stopper vanligvis opp ved 32 / 64 kjerner på en server

```




### Spatial locality
- Når man aksesserer data, så skal man mest sansynlig aksessere data som ligger ved siden av etterpå.

### Temporal locality
- Samlokalisert i tid
- Løkker, instruksjoner som skal gjentas


``` 
I OS er det et poeng at når bi utfører en context switch (bytter fra å jobbe i et program til et annet), så er cachen full av data fra forrige program og det tar tid å bytte den ut. 

Derfor sier vi at en context switch er ganske kostbart


 ```






















