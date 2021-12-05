# Operating System Security

### Intro
- Selve Operativsystemet må være sikkert
    - Er det ikke det, så er alt som kjører på det, heler ikke sikkert
- OS benytter sikkerhets policies (Aksess controll)
- OS bør forhindre/minimere skade fra software som er sårbar

#### CVE database
- Består av sårbarheter med CVE nummer
- Slik at alle som jobber med sårbarheten vet ad du jobber med den samme
- Mange sårbarheter i et OS, men vanskelig å utnytte
    - Heftig kompetanse skal være på plass
    - Det meste av sårbarheter er vanskelig å utnytte
- Det finnes sårbarheter overal
    - En ting er i selve OS
    - Kan også være i GRUB (Boot loader) Grand Unified Boot-Loader
        - Programvaren som starter operativsystemet
    - Også på hardware nivå (Meltdown / Spectre)

### Sikkerhet (Container vs VM)
- En Virtuell maskin har mye sterkere sikkerhet enn en container
    - En Docker conteiner hviler på systemkallgrensesnittet og på OS, så så fort du har en kernel mode sårbarhet i OS, så kan du kanskje bruke den til å bryte deg ut av en container.
    - Men å bryte seg ut av en VM, da må du finne en sårbarhet knyttet til hypervisor-grensesnittet og det er mye vanskeligere (men mulig)

#### Blue Pill angrepet (Software) - Digresjon
- Når du kjører Windows på laptopen, hvordan vet du at du kjører på laptopen, eller om du er i en virtuell maskin ?

- Om Blue Pill - programvaren ble kjørt, så ble Windowsen plassert i en VM uten at brukeren visste om det.


### Mål for sikkerhet: (CIA)
- Confidentiality   (Hemmelighold)
- Integrity         (Korrekt)
- Availibility      (Tilgjengelig når man trenger den)

- Security Policies are rules that state what is or is not permitted
    - Hva som er lov og ikke lov
    - OS skal passe på at sikkerhetspolicyene overholdes


### Design principles: 
1. Economy of mechanism 
    - Hold systemet så lite som mulig. Enkle system har mindre bugs, og det er lettere å forstå oppførselen
2. Fail-safe defaults
    - Betyr at default er sikkert. Eks alle porter er bblokkert som default, og må åpnes om de skal brukes. (Default deny)
3. Complete mediation
    - Gjør alltid sikkerhetssjekker, aldri anta noe
4. Open design
    - I et system så skal du anta at en angriper kjenner alt om systemet ditt. (Betyr ikke at du skal dele alt om systemet ditt)
    - Sikkerheten skal ikke være avhengig av hemmelighold
5. Separation of privilege
    - Krev separate credentials for å utføre kritiske handlinger
    - Eks: 2FA (passord + sikkerhetsnøkkel) er tryggere enn bare en av delene
6. Least privilege
    - Begrens alt så mye som mulig (Need to know)
    - Gi en bruker bare de rettighetene han trenger for å utføre det vi vil tillate
7. Least common mechanism
    - Hold alt så adskilt som mulig
    - Eks prosesser, brukere etc..
8. Acceptability (Viktigst)
    - Et system virker bare hvis brukerene godtar det
    - Hjelper ikke å lage et 100% sikkert system om brukerene ikke godtar det
    - Må balansere sikkerhet og brukervenlighet.

## Access Control
- Hver gang jeg skal åpne en fil, skrive en fil, lese en fil, starte et program, må OS bestemme om jeg har lov


### Security Reference Monitor
- Er den delen av OS som skal si ja eller nei
    - Er denne aksessen tillatt? ja/nei

#### Capabilities / ACL
- To måter Security Reference Monitor avgjør aksess:
    1. Gi nøkkel til subjektet Capabilities(1)
    2. Aksesskontroll liste med hva man har lov til å gjøre med objektet ACL(1)
        - Spørsmålet: Capabilities(1) eller ACL(2)?
        - Svar: Litt av begge
            - Linux: UID/GID = Capabilities(1)
            - Linux: rwxrwx-- = ACL(2)
            - Windows: Access token = Capabilities(1)
            - Windows: Security descriptor = ACL(2)
```
$>id
uid=1000(jorgen) gid=1000(jorgen) groups=1000(jorgen),4(adm),24(cdrom),27(sudo),30(dip),46(plugdev),122(lpadmin),133(lxd),134(sambashare)
```
- Brukerid
- Gruppeid
- Hvilke grupper jeg tilhører
- Grupper har forskjellige rettigheter

```
$>getfacl I
# file: I
# owner: jorgen
# group: jorgen
user::rwx
group::rwx
other::r-x
```



- Eks:
- Identification (Jeg er jørgentn bruker)
- Authentication (Jeg må bekrefte det med passord abc123)
- Authorization  (Hver gang jeg skal gjøre noe, så må jeg autoriseres om  jeg har lov til å gjøre det)


```
                    access?
[Subjekt jørgentn]-------------->[Objekt]
(process)              ^         (fil, dir)
                       |
    Avgjøres av OS sin Security Reference Monitor
```
- Subjekt representert av et shell (prosess)

### MAC / DAC


 #### DAC = Discretionary Access Control
- Brukere bestemmer Aksess Coontroll

```
$>chmod 777 fil.tt
```



#### MAC = Mandatory Access Control
- Systemet bestemmer Acsess Controll
    - Man ønsker ofte å ha en Aksess Controll som overstyrer brukerene
    - PGA at det var så mange sidekanal-angrep, så stoppet windows å bruke det, men kom tilbake i Vista som:
##### Mandatory Integrity Control:
- Får ikke skrive til høyere integritetsnivå

- Eks: Setter brukeren som en prosess på medium nivå
- Kan starte eks Powershell på lavt integritets nivå
    - Får da ikke lov til å skrive til egen hjemmekatalog, fordi den katalogen er på medium integritets nivå.
- Laget for a Internet Explorer skal starte med lavt integritets nivå, for å beskytte brukeren fra malware fra nettleseren.





##### Sidekanal angrep

- Fange opp informasjon ved en sidekanal

- Eks1: 
    - Flere brukere på en maskin med 8 CPU kjerner
    - Kan sende meldinger til andre brukere med CPU-pinning
    - Mye trafikk = 1, Lite trafikk = 0
    - Har da en 8 bits måte å kommunisere på



### Windows opperation: 

#### Login (windows)
1. CTRL-ALT-DEL (Secure Attention Sequence) initiate winlogin
(CTRL-ALT-DEL - genererer interrupt, OS gjenkjenner kombinasjon og starter det ekte innloggingsprogrammet)
2. Winlogon uses Isass to authenticate (LocalSecurityAuthoritySubsystem sjekker hash av passord mot hash i registry)
3. User ends up with the GUI shell explorer process with an access token

#### UAC (windows)
- User account Control = UAC
- Windows < NT lansert med at alle har admin hele tiden
    - NT fikset dette, men all software antok fortsaatt admin.

- Problemet: Software-utviklere antar at deres applikasjon vil kjøre som administrator på windows

    - UAC implementert for å rette dette Vved at:
        - Alle admin-brukere blir startet med standard-bruker privilegier
            - Tilhørighet i admin gruppen markert DENY
            - Privilegie-set redusert til user-set

    - Som admin kan du skrive over hele filsystemet
        - UAC retter dette med filsystem og registry virtualisering

### Linux operation
1. Login checks username/password and groups
    - /etc/passwd, /etc/shadow (bruker/passord)
    - /etc/groups
    - Fordi mange prosesser trenger å mappe mellom UID og GID
        - passwd og groups lesbare, men ikke passordet(shadow)
2. Starts shell with users UID, GID (shell = GUI / terminal)
3. sudo similar to UAC

- Har ikke CTRL - ALT - DELETE (restarter)
- Alle brukere skal få lov til å kjøre passwd programet som root, men ingen får skrive til shadowfilen (setUID bit = s)

```
$>passwd
Changing password for jorgen.
Current password: 
New password: 
```



## Memory protection:

### Hva vil det si å utnytte en sårbarhet
- Ta over programflyten til et program
- Hvordan kan et program i minnet miste kontrollen ?



#### Buffer overflow (gets() - brukt i gamledager)
```
1 void A( ) {
2   char B[128];                  /* space for 128 B on stack */ 
3   printf ("Type log message:"); 
4   gets (B);                     /* read into buffer */ 
5   writeLog (B);                 /* output to the log file */
6 }
```
- gets() sjekker ikke hvor mye vi leser inn
    - gets her leser gjerne inn mer enn 128 byte
    - og da fortsetter vi å overskrive stacken og vi kan skrive inn mye mer i minnet, enn det vi har lov til
    - Nøkkelen: Klarer å skrive over en del av minnet, som du egentlig ikke skal ha lov til å skrive til
        - Løsning fgets()
            - Da kan du legge inn hvor mye du har lov til å skrive inn
            - eks: maks 128 bytes

- Punchline! De fleste sårbarheter oppstår pga at det finnes mange måter å få overskrevet noe i minnet på

#### Heap Spraying:
- En angriper trenger ikke å vite eksakt hvor han skal returnere om et angrep er basert på et "nop sled" (NoOp Slede)

```
Si at dette er minnet og den angriper har skrevet inn et virus
1 [instruksjon]
2 [instruksjon]
3 [instruksjon] 
4 [instruksjon]
5 [instruksjon]
6 [instruksjon]
7 [instruksjon]
8 [instruksjon]
9 [instruksjon]
10[instruksjon]
11[instruksjon]
12[ <malware> ]
13[ <malware> ]
14[ <malware> ]
15[ <malware> ]
16[ <malware> ]
17[instruksjon]

Hvordan skal angriper si at koden starter på 12? vanskelig

om du får til en buffer overflow kan angriper benytte seg ar "noop sled" DVS: skrive over annen kode med "nop" slik:


1 [    nop    ]
2 [    nop    ]
3 [    nop    ] 
4 [    nop    ]
5 [    nop    ]
6 [    nop    ]
7 [    nop    ]
8 [    nop    ]
9 [    nop    ]
10[    nop    ]
11[    nop    ]
12[ <malware> ]
13[ <malware> ]
14[ <malware> ]
15[ <malware> ]
16[ <malware> ]
17[    nop    ]

nop = no operation og PC vil hoppe over alle linjer 1 - 11 og starte på 12
Hopper på noop sleden og treffe malware.

```

#### Hva gjør OS for å beskytte mot buffer overflow

##### DEP (Data Execution Prevention)
- Bør det være mulig å skrive kode til minnet som også skal kunne utføres?
    - Nei, minnet bør være (W XOR X) skriv ELLER execute. Aldri begge!

- Eksempel
```
$>cat /proc/$(pgrep -n bash)/maps

557278908000-557278937000 r--p 00000000 08:03 789463                     /usr/bin/bash
557278937000-557278a16000 r-xp 0002f000 08:03 789463                     /usr/bin/bash
557278a16000-557278a50000 r--p 0010e000 08:03 789463                     /usr/bin/bash
557278a50000-557278a54000 r--p 00147000 08:03 789463                     /usr/bin/bash
557278a54000-557278a5d000 rw-p 0014b000 08:03 789463                     /usr/bin/bash
557278a5d000-557278a68000 rw-p 00000000 00:00 0 
55727a02b000-55727a1d3000 rw-p 00000000 00:00 0                          [heap]
7f7a198a3000-7f7a1a685000 r--p 00000000 08:03 793500                     /usr/lib/locale/locale-archive
7f7a1a685000-7f7a1a688000 rw-p 00000000 00:00 0 
7f7a1a688000-7f7a1a6b4000 r--p 00000000 08:03 796136                     /usr/lib/x86_64-linux-gnu/libc.so.6
7f7a1a6b4000-7f7a1a848000 r-xp 0002c000 08:03 796136                     /usr/lib/x86_64-linux-gnu/libc.so.6
7f7a1a848000-7f7a1a89c000 r--p 001c0000 08:03 796136                     /usr/lib/x86_64-linux-gnu/libc.so.6
7f7a1a89c000-7f7a1a89d000 ---p 00214000 08:03 796136                     /usr/lib/x86_64-linux-gnu/libc.so.6
7f7a1a89d000-7f7a1a8a0000 r--p 00214000 08:03 796136                     /usr/lib/x86_64-linux-gnu/libc.so.6
7f7a1a8a0000-7f7a1a8a3000 rw-p 00217000 08:03 796136                     /usr/lib/x86_64-linux-gnu/libc.so.6
7f7a1a8a3000-7f7a1a8b0000 rw-p 00000000 00:00 0 
7f7a1a8b0000-7f7a1a8be000 r--p 00000000 08:03 797255                     /usr/lib/x86_64-linux-gnu/libtinfo.so.6.2
7f7a1a8be000-7f7a1a8cc000 r-xp 0000e000 08:03 797255                     /usr/lib/x86_64-linux-gnu/libtinfo.so.6.2
7f7a1a8cc000-7f7a1a8da000 r--p 0001c000 08:03 797255                     /usr/lib/x86_64-linux-gnu/libtinfo.so.6.2
7f7a1a8da000-7f7a1a8de000 r--p 00029000 08:03 797255                     /usr/lib/x86_64-linux-gnu/libtinfo.so.6.2
7f7a1a8de000-7f7a1a8df000 rw-p 0002d000 08:03 797255                     /usr/lib/x86_64-linux-gnu/libtinfo.so.6.2
7f7a1a8df000-7f7a1a8e1000 rw-p 00000000 00:00 0 
7f7a1a8e8000-7f7a1a8ef000 r--s 00000000 08:03 131990                     /usr/lib/x86_64-linux-gnu/gconv/gconv-modules.cache
7f7a1a8ef000-7f7a1a8f0000 r--p 00000000 08:03 795922                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f7a1a8f0000-7f7a1a918000 r-xp 00001000 08:03 795922                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f7a1a918000-7f7a1a922000 r--p 00029000 08:03 795922                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f7a1a922000-7f7a1a924000 r--p 00032000 08:03 795922                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7f7a1a924000-7f7a1a926000 rw-p 00034000 08:03 795922                     /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffedaac4000-7ffedaae5000 rw-p 00000000 00:00 0                          [stack]
7ffedabe1000-7ffedabe5000 r--p 00000000 00:00 0                          [vvar]
7ffedabe5000-7ffedabe7000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
```

- Ser at noen deler er x og noen er w, men ingen er både w og x
- Stack er bare write, ikke mulig å skrive kode til stack som er eksekverbar
- Betyr at du ikke kan skrive inn kode på stack / heap og deretter få kjørt den

### Neste generasjons angrep: return to libc
- Siden DEP stopper eksekverbar kode i minner, måtte de finne en ny måte
- Hvorfor skrive inn kode i minnet når det ligger så mye kode der fra før?
- Hva har alle linux prosesser i minnet?
    - De har mappet inn libc-biblioteket
    - Der finnes mye interessant kode
    - blandt annet system()
    - får man kjørt system("/bin/bash/") så har jeg full tilgang til makinen som root (rootshell)
    - Dette angrepet kalles "return to libc"
