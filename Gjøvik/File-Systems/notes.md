# File Systems

#### Fil
1. Hva betyr det at en fil har to deler?

2. Hva er en directory / katalog / mappe / folder?

3. Hva betyr det at en directory bare er en fil ?

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


#### Slette fil eksempel:
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

### Fortsett her, forelesning krasjet på 39 min CA!

### Access control

- Permissions bits

- rwxrwxrwx user, group, others

- change with chmod(), chown()

- SetUID, SetGID, Sticky bit

```
Demo:
    ls -l $(which passwd)
    ls -ld /tmp
```




