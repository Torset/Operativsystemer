# File Systems

#### Fil
1. Hva betyr det at en fil har to deler?
    - En fil er en inode og filinholdet

2. Hva er en directory / katalog / mappe / folder?
    - En spesiel fil med en tabell som ampper filnavn til inode

3. Hva betyr det at en directory bare er en fil ?
    - Er bare en fil med eget inodenr

- Operativsystemet sin oppgave er å ta vare på innholdet i fila, og de tilhørende metadataene. Dette må man organisere et filsystem for

En fil er organisert i en katalog-struktur

```
Lage en fil
$>touch mysil

Vise fil + inode nummer
$>ls -li mysil
655996 -rw-rw-r-- 1 jorgen jorgen 0 Dec  3 04:35 mysil
```
#### Inode nummer (Lavnivå navnet til fila)
- Det egentlige navnet til fila
- Unikt nummer

### Standard filsystem for Linux: EXT
- Vi skal holde oss til EXT
- NTFS FAT (andre filsystemer, ikke pensum)


## EXT:
- Toppen (starten) av filsystemet er rota "/"

### API
- Systemkall som OS tilbyr mot FS;
    - open(),openat(),creat() Disse returnerer en<b>fildeskriptor</b>
    - read() - for å lese en fil
    - write() - skrive til en fil
    - close() - lukke en fil


#### Fildeskriptor (FD)
- Er et tall som er privat for hver prosess
- I UNIX er det brukt for å aksessere filer, 
    - DVS: Når en fil er åpnet, så kan du bruke FD til å skrive eller lese filen

- STDIN FD = 0
- STDOUT FD = 1
- STDERR FD = 2

```
Eksempel på hvordan openat() returnerer en FD:

$>myfile=$(mktemp /tmp/XXXXXXXXXXXXXXXXXX) || exit 1
$>echo mysil > $myfile
$>strace cat $myfile |& less

Sample fra strace:

openat(AT_FDCWD. "/tmp/9fo4mNmkPFeos1S8GD", O_RDONLY) = 3

3 er her fildeskriptoren
```
- DVS, i denne prosessen så er 3 et unikt tall som identifiserer denne filen som åpen
- Her 3 fordi 0, 1 og 2 er reservert til STDIN, STDOUT og STDERR


#### strace
- System Call Trace
```
Oppsummering av systemkall som er brukt og antall ganger

$>strace -c cat $myfile mysil

% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ----------------
  0.00    0.000000           0         4           read
  0.00    0.000000           0         1           write
  0.00    0.000000           0         7           close
  0.00    0.000000           0         3           fstat
  0.00    0.000000           0        10           mmap
  0.00    0.000000           0         4           mprotect
  0.00    0.000000           0         2           munmap
  0.00    0.000000           0         4           brk
  0.00    0.000000           0         4           pread64
  0.00    0.000000           0         1         1 access
  0.00    0.000000           0         1           execve
  0.00    0.000000           0         2         1 arch_prctl
  0.00    0.000000           0         1           set_tid_address
  0.00    0.000000           0         2           fadvise64
  0.00    0.000000           0         5           openat
  0.00    0.000000           0         3           newfstatat
  0.00    0.000000           0         1           set_robust_list
  0.00    0.000000           0         1           prlimit64
  0.00    0.000000           0         1           getrandom
------ ----------- ----------- --------- --------- ----------------
100.00    0.000000           0        57         2 total
```

#### Pipe
- Tar output før pipe og gir det som input etter pipe
- eks 1:
```
$>cat | wc
Hei på
deg

(ctrl + d)

//Output:
2   3   13

2 linjer, 3 ord, 13 tegn
```
- eks 2:
- $(pgrep -n cat) (spør etter PID til cat og setter inn den)
- /fd vil se det som er i fil deskriptor katalogen til denne prosessen

```
$>cat | wc &
$> ls -l /proc/$(pgrep -n cat)/fd

//Output:
lrwx------ 1 jorgen jorgen 64 Dec  3 05:07 0 -> /dev/pts/2
l-wx------ 1 jorgen jorgen 64 Dec  3 05:07 1 -> 'pipe:[316480]'
lrwx------ 1 jorgen jorgen 64 Dec  3 05:07 2 -> dev/pts/2

ser at cat sin STDIN (FD 1) er omdirigert til en pipe
```
- Så når vi piper og skriver data til fil etc, så endrer vi bare FD


### Sync
- Husk at RAM blir brukt som cache mot underliggende lagringsenheter (blokk enheter)
    - For å flushe en fil fra RAM ut til disk : fsync
    - For å flushe hele filsystemet fra RAM til disk: sync

- Vanligvis brukes "write-back caching"
    - Lagrer filen i RAM
    - Etterhvert så blir den skrevet til disk

- Om vi lagrer en sikkerhetskritisk applikasjon (Eks database) som skal gi brukeren beskjed om at filen er skrevet til disk, når den faktisk er skrevet, så kan vi bruke fsync()
1. Skriver man med en write-forespørsel
2. Så gjør man en fsync() for å sikre at filen faktisk er skrevet
3. Så gir man beskjed om at filen er skrevet til disk
- Gir ikke aksept for at brukeren skal tro at data er skrevet til disk, når den faktisk bare ligger i RAM

### stat (viser metadata)
- Viser:
- Filnavn
- Størrelsen
- Antall blokker den bruker (Der innholdet i fila ligger)
- IO Block = Størrelsen på blokkene (på disk)
- linker
- Inodenummer
- Rettigheter
- Når den sist ble: Aksessert, Modifisert og Endret
    - Aksessert
    - Modifisert 
    - Endret 
    - (Modify og change er ikke det samme)
        - Oppdateres innoldet i fila så oppdateres modify og change
        - Oppdateres metadata til fila, så endres bare change


```
$>stat mysil
//output:
  File: mysil
  Size: 0               Blocks: 0          IO Block: 4096   regular empty file
Device: 803h/2051d      Inode: 655996      Links: 1
Access: (0664/-rw-rw-r--)  Uid: ( 1000/  jorgen)   Gid: ( 1000/  jorgen)
Access: 2021-12-03 05:01:45.098160543 -0800
Modify: 2021-12-03 04:35:08.095020744 -0800
Change: 2021-12-03 04:35:08.095020744 -0800
 Birth: 2021-12-03 04:35:08.095020744 -0800
```


## Metadata!
- Alt som tilhører filen, som ikke er innholdet i filen


### Directories
- Er bare en fil, men innholdet i fila kan vi tenke på som en stor tabell med 2 rader.
<table>
<th>Filnavn</th><th>Inodenummer</th>
<tr>
<td>mysil</td>
<td>12365242</td>
</tr>
<tr>
<td>myfile</td>
<td>522242</td>
</tr>
</table>

- Her mappes filnavn -> unikt inodenummer
    - filnavn trenger ikke være unikt, men filer i samme katalog må også ha unikt filnavn.

- Har i tillegg: (Ikke pensum)
    - Offset til neste entry
    - Lengde 
    - Type fil
```
$>stat /tmp
//output:
   File: /tmp
  Size: 4096            Blocks: 8          IO Block: 4096   directory
Device: 803h/2051d      Inode: 655363      Links: 21
Access: (1777/drwxrwxrwt)  Uid: (    0/    root)   Gid: (    0/    root)
Access: 2021-12-03 05:21:24.683413595 -0800
Modify: 2021-12-03 05:01:03.882066987 -0800
Change: 2021-12-03 05:01:03.882066987 -0800
 Birth: 2021-11-22 05:24:51.812191709 -0800
```

- Er kontrollert av OS for integritetsårsaker
- mkdir()
- opendir()
- readdir()
- closedir()
- rmdir()



### Links

#### Slette en fil (unlinkat())
- Hvorfor unlink?
- Fordi det finnes to typer linker:

##### Hard-links (directory entry)
- En hard link er et entry i directory tabellen

##### Soft-links (shortcut)
- En soft link er en fil (med egen inode nummer) hvor innholdet er stien  (eks: /tmp/mysil) til en annen fil


### Eksempel på linker:
```
$>cd /tmp
$> echo mysil > a
$>cat a
mysil
$>ls -l a
-rw-rw-r-- 1 jorgen jorgen 6 Dec  3 05:47 a

Tallet 1 betyr at filen har 1 hard-link (katalogoppføring)
Betyr at filen finnes bare ett sted, og det er i denne katalogen
```

- Lage symbolsk-link

```
$>cd /tmp
$> echo mysil > a
$>cat a
mysil
$>ls -l a
-rw-rw-r-- 1 jorgen jorgen 6 Dec  3 05:47 a
$>ln -s a sym

$>ls -l
-rw-rw-r-- 1 jorgen jorgen 6 Dec  3 05:47 a
lrwxrwxrwx 1 jorgen jorgen    1 Dec  3 05:50 sym -> a

Ser her at det ikke er noe endring i filen, men vi har fått en ny entry: (sym link (shortcut))
```

- Lage hard-link

```
$>cd /tmp
$>cd echo mysil > a
$>cat a
mysil
$>ls -l a
-rw-rw-r-- 1 jorgen jorgen 6 Dec  3 05:47 a
$>ln a hard

$>ls -l a
-rw-rw-r-- 2 jorgen jorgen 6 Dec  3 05:47 a

Ser her at a har fått 2 linker
```


- Katalogen ser slik ut:

```
$>ls -li

656074 -rw-rw-r-- 2 jorgen jorgen 6 Dec  3 05:57 a
656074 -rw-rw-r-- 2 jorgen jorgen 6 Dec  3 05:57 hard
656075 lrwxrwxrwx 1 jorgen jorgen 1 Dec  3 05:57 sym -> a

// Ser ut som 3 filer, men er bare 2

a og hard har samme inodenummer = samme fil
sym har eget inodenr og er en shortcut til innholdet i a
```

#### Slette fil eksempel:
```
$>cat a
mysil
$>cat sym
mysil
$>rm a
$>ls -li
total 4
656074 -rw-rw-r-- 1 jorgen jorgen 6 Dec  3 05:57 hard
656075 lrwxrwxrwx 1 jorgen jorgen 1 Dec  3 05:57 sym -> a


// Ser her at sym -> a (er i rød farge)
// Siden den ikke fungerer

$>cat sym
cat: sym: No such file or directory

// filen finnes fortsatt(inoden), men sym linker til filnavnet a, som ikke eksisterer
```

- 3 ting som skjer når man sletter en fil
    1. Fjerner katalog-entry
    2. Inoden markeres som ledig for gjenbruk
    3. Datablokkene markeres som ledig til gjenbruk
- 3 skrive opperasjoner når man sletter en fil



### Rettigheter (rwxrwxrwx user, group, others)
- Permissions bits
    - 9 bits (3 for hver, eier, gruppe og andre)
        - 111111111 = rwxrwxrwx
        - 111000000 = rwx------
        - 111110100 = rwxrw-r--

- change with chmod(), chown()
    - r = 4
    - w = 2
    - x = 1
```
$>chmod 111 hard
$>ls -l
---x--x--x 1 jorgen jorgen 6 Dec  3 05:57 hard

$>chmod 764 hard
$>ls -l
-rwxrw-r-- 1 jorgen jorgen 6 Dec  3 05:57 hard


$>chmod 700 hard
$>ls -l
-rwx------ 1 jorgen jorgen 6 Dec  3 05:57 hard

```

### SetUID, SetGID, Sticky bit

#### SetUID = s
- Settes sammen med (oppå) x til eier
- Gjør at alle andre har lov til å kjøre programmet som eier av filen  (i dette tilfellet som root)
    - passwd må ha skrivetilgang til passorddatabasen
    - Jeg som bruker kan ikke få tilgang til passorddatabasen
    - Men passwd må ha det, derfor må jeg ha lov til å kjøre    passwd rom root for å endre passordet mitt

```
    $>ls -l $(which passwd)
    -rwsr-xr-x 1 root root 59976 Jun 17 12:35 /usr/bin/passwd
    // s = setUid bit
    
 
```
#### SetGid
- kjøre som gruppen til root, men aldri i bruk

#### Stickybit = t
- På tmp katalogen er stickybit satt
    - Fra gammelt, bit som ber OS om å forsøke å holde denne prosessen i minnet (Ikke swap ut på disk)
    - I dag betyr det at alle får lov til å skrive til tmp katalogen, men ingen har lov til å slette hverandres filer.
        - Selv om alle har skriverettigheter til katalogen

```
     $>ls -ld /tmp
    drwxrwxrwt 23 root root 4096 Dec  3 06:29 /tmp
    
```

### Creating and mounting

```
Partisjonering:
$>cfdisk /dev/sdb5
```

```
Opprette Filsystem
$>mkfs /dev/sda1
```

- Fil-systemet tar plass på disken (kanskje 50gb på en 1TB disk)
    - Må reservere plass til inoder
    - Sektorer blir så gruppert i blokker

```
Eks:
FS = Fil-system (Består av inoder (en pr fil) 256 bytes)
D = Sektor / Page(SSD)

1 TB Disk:       |-- Blokk 1 --||-- Blokk 2 --|...etc...   
[[FS][FS][FS][FS][D][D][D][D][D][D][D][D][D][D][D]]
```

### FS oversikt
1. Data-bitmap
2. Inode-bitmap

- 1 og 2 = Oversikt over hvilke data/inoder som er i bruk, og hvilke som er ledige 

```
Eks: 
I = inoder
i = inode-bitmap
D = data
d = data-bitmap
S = Superblokk (Informasjon om systemet)


1 TB Disk:               |-- Blokk 1 --||-- Blokk 2 --|...etc...   
[[S][i][d][I][I][I][I][I][D][D][D][D][D][D][D][D][D][D][D]]
```
Ser her at ganske mange MegaByte forsvinner til "Metadata"



### Hvordan kommer vi oss fra metadata til datablokkene
- Datablokken er der selve innholdet er

```
$>cat hard
mysil

// Hvordan kom mysil på skjermen?

$>ls -li
656074 -rwxrw-r-- 1 jorgen jorgen 6 Dec  3 05:57 hard
656075 lrwxrwxrwx 1 jorgen jorgen 1 Dec  3 05:57 sym -> a

// Ser at hard er i denne katalogen og har 656074 som inodenummer.
// Slår opp i denne (656074) inoden og på ett eller annet magisk vis
// kommer jeg meg til innholdet i fila og får skrevet ut mysil.

// Hvordan kommer jeg meg fra inodetallet til innholdet i fila ?

// Løsning:
// I Inoden så er det satt av 60byte til disk pointers(15 stk totalt)
```



- Når filen er liten, så er det enkelt:
    - Inoden inneholder adressene til blokka

- Når filen er stor
    - Bruker indirekte adressering


### Indirekte Adressering

- Liten fil:
- Inode med informasjon og opptil 12 blokk adresser
- Om filen ikke er større enn 12 blokker, så får alt plass i inoden
- Standard blokkstørrelse = 4KB
- 12 * 4KB = 48KB, så så lenge filen ikke er større en 48KB, så går dette fint

```
Eks:

[Inode]
[Informasjon]
[Informasjon]
[Informasjon]
[Informasjon]
[ Addresse 1]
[ Addresse 2]
[ Addresse 3]
[...........]
[Addresse 12] 
[Addresse 13]reservert indirekte
[Addresse 14]reservert dobbel
[Addresse 15]reservert trippel
```


- Stor fil;
- Om filen er større enn 12 blokker, så blir Addresse 13 til en peker, som peker til en blokk på disk, som kan fylles med adresser

```
Eks:

[Inode]
[Informasjon]
[Informasjon]
[Informasjon]
[Informasjon]
[ Addresse 1]
[ Addresse 2]
[ Addresse 3]
[...........]
[Addresse 12]
[Addresse 13] ---> Peker til en blokk med adresser
[Addresse 14] reservert dobbel
[Addresse 15] reservert trippel
```

## Trolig spm på eksamen:
Med Blokkstørrelse på 4KB og Addresse-størrelse på 32bit, hvor stor fil kan jeg ha med singel-indirekte adressering?

### Utregning Singel-Indirekte
- Blokkstørrelse = 4KB
- Addresse-størrelse = 32bit

```
Først finn ut hvor mange adresser som går i en blokk:
4KB / 32b
32b = 4B
4KB/4B = 1K
1K = 2^10 (= 1024) Antall adresser

Her peker hver addresse til en blokk som er 4KB

4KB = 2^2 * 2^10 = 2^12

1K * 4KB = 2^10 * 2^12 = 2^22B

= 4MB (+ 48KB fra inoden)

Svar: Med indirekte adressering kan filen være 4 MB stor.
```


### Kan ha Dobbel-indirekte adressering:
- Her peker adresse 14 på en blokk som inneholder pekere til blokker med adresser
DVS: 
- adressene 1 - 12 i inoden er full (48KB)
- adressene i dobbel indirekte er full (4MB)
- Snakker nå om adresse 14 i inoden


### Utregning Dobbel-Indirekte
- Blokkstørrelse = 4KB
- Addresse-størrelse = 32bit

```
Først finn ut hvor mange adresser som går i en blokk:
4KB / 32b
32b = 4B
4KB/4B = 1K
1K = 2^10 (= 1024) Antall adresser

Her peker hver addresse til en blokk som hver kan ha 1024 adresser
antall adresser blir da 1024^2
evt 2^10^2 = 2^20 = 1M

4KB = 2^2 * 2^10 = 2^12

1M * 4KB = 2^20 * 2^12 = 2^32B

= 4GB (+ 48KB fra inoden)

Svar: Med dobbel-indirekte adressering kan filen være 4 GB stor.
```

### Kan ha trippel indirekte også.


#### EXT4
- Bruker extents
- På samme måte som NTFS


### Lese en fil fra disk
- Tar 10ms å lese fra disk (Veldig tregt)
- 10 000 000 ganger raskere å aksessere register enn å aksessere disk
- Ledig RAM brukes derfor som cache mot disk

- Skal lese file "foo" som ligger i katalogen "bar"
    - Inode nr 2 er alltid rota / toppen av filsystemet på EXT
    - vat da at path er "/foo/bar"



## Crash management
- Hva hvis stømmen går?
    - Jeg har en inode som er i bruk, men som ikke er registrert i en katalog
    - Dette er det FSCK gjør
        - Går gjennom: For hver eneste inode i inode-bitmapen:
        - Hvis den er i bruk, skal jeg finne den i en katalog
        - Hvis ikke, så er det en feil i filsysteme som jeg må håndtere


### FSCK

- File System Checking
- Are the inodes that are marked as used present in directories?
- Are the data blocks that are marked as used present in inodes?
- A bunch of small checks: e.g. are all values sensible (within
range)?

Takes "forever" on a large HDD

#### Idempotence

- Er noe galt, og jeg gjør en funksjon på det, så blir det riktig
- Er noe riktig, og jeg gjør en funksjon på det, så forblirblir det riktig
- altså:
    - f(galt) = riktig
    - f(riktig) = riktig

### Journalling

- Journalling File Systems

- What is the difference between:

- Add newly freed blocks from i-node K to the end of the free list
    - (Ikke idempotent fordi om den gjøres flere ganger, så legges     blokkadressene til flere ganger på lista)

- and

- Search the list of free blocks and add newly freed blocks from
i-node K to it if they are not already present
    - (Idempotent  fordi hvist jeg søker gjennom listen å legger de til kun om de ikke finnes der fra før)
- ?

#### Journaling File Systems

- To make journalling work, the logged operations must be idempotent
("convergent").

- A Journalling file system keep a log of the operations it is going
to do, so they can be redone in case of a system crash (see first
two illustrations of chp 42.3)






















