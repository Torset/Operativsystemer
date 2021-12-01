# Scheduling



## Scheduler definition
```
Special system software which handle process scheduling
in various ways. 
```
<b>Main task:</b> Select the jobs to be submitted into the system and to decide which process to run.



### Three types of scheduler:
<b>Long term scheduler:</b> 

- Also called job scheduler
- Determines which programs are admitted to the system for processing
- Select processes from a pool, and loads them into memory
- Provide a balanced mix of I/O bound and CPU-bound jobs for the CPU to process

<b>Short term scheduler:</b>

- Also called the CPU scheduler
- main task is to increase system performance in accordance with a set of criteria
- Selects a process among those in the ready que and allocates a CPU to them
- Faster than long ter schedulers

<b>Medium term scheduler:</b>

- IKKE I PENSUM!
- In charge of handling the swapped out processes
- Removes processes from the memory


### Process characteristics

#### Begreper

- <b>CPU-bound</b> 
Prosesser som ønsker å bruke CPU hele tiden, trnger å regne mye eks:
    - Scientific computing
    - Machine learning
    - Video rendering

- <b>I/O-bound</b>
Om en prosess ikke er CPU-bound så er den I/O-bound.
Betyr at prosessen venter på I/O, for det meste har den ikke noe å gjøre etc:
    - Nettleser
    - Tekstbehandler

- <b>Memory-bound</b>
Ikke så mye brukt, laster du mye data i minne, så kan minne bli flaskehalsen.

- <b>Real-time</b>
Real time-Prosesser som har tidsfrister
    - Soft real time (multimedia) (Har deadlines, men ikke noe krise om du går glipp av de, kan føre til feks, hakkete film)
    - hard real time (robotics) Krise om du misser tidsfrister, feks selvkjørende bil


- <b>Batch vs Interactive</b>
Batch har ikke I/O
Batch er en bunke med jobber som skal regnes gjennom

- <b>Service</b>
Service prosess er alt det som kjører før vi logger inn på maskinen. 
Er kanskje en server uten innlogging, webserver?




### Turnaround time

Turnaround = (Time of)Completion - (Time of)Arrival



<b>Turnaround time</b> The time from a process enters the system until it leave, e.g

```$>time uuigdgen```

Remember the process states (ready, running, blocked)


## SCHEDULER METHODS:
 |
 v


### FIFO (First In First Out)
- Første prosess inn får kjøre først

```Eksempel: 

3 prosesser ankommer samtidig og bruker 10 sekunder på å bli ferdig. Hva blir gjennomsnittlig tunaround tid?

P1---|P2---|P3---|

0----10----20----30----40-----> Tid

Alle ankommer på 0

p1 har turnaround på 10 - 0 = 10
p2 har turnaround på 20 - 0 = 20
p3 har turnaround på 30 - 0 = 30

10 + 20 + 30 = 60 / 3(prosesser) = 20

Gjennomsnittlig Turnaround for alle 3 prosessene er 20


```

### SJF (Shortest job first)
- Bad for response time
- Kan bli problematisk ved mange korte jobber, noen jobber kan bli helt utestengt
- Om den korteste jobben i køen tar 100 sekunder, og mens den kjører kommer det inn korte jobber på 5 sekunder, så må de vente til den lange jobben er ferdig.
    - Løsning på dette kan være STCF (shortesty time to completion first - Kan være vanskelig å finne ut av)







## A new metric: Response time

- Tidspunktet første gang prosessen kjører minus annkomst

´´´
(Time of)Response = (Time of)First run - (Time of)Arrival
```


```Eksempel: 

3 prosesser ankommer samtidig og bruker 10 sekunder på å bli ferdig. Hva blir gjennomsnittlig responstid?

P1---|P2---|P3---|

0----10----20----30----40-----> Tid

Alle ankommer på 0

p1 har turnaround på 0 - 0 = 0
p2 har turnaround på 10 - 0 = 10
p3 har turnaround på 20 - 0 = 20

0 + 10 + 20 = 30 / 3(prosesser) = 10

Gjennomsnittlig Responstid for alle 3 prosessene er 10
```

### RR (Round Robin)
- Good for response time
- Deler opp tiden i timeslice (tidsluke)
- Prosessene ligger i kø og kjører etter hverandre, men de får bare (eks: time quantum = 2) 2 sekunder hver.
- En slags rettferdighet
- Det vanskelige å avgjøre er hvor stor tidsluken er 
(Ønsker vi så god som mulig responstid, så må tidsluken være så liten som mulig, men da vil overheaden bli stor pga mange context switch)
- context switch tar tid pga at vi må flushe cache og fylle den igjen

```
>$ cat /proc/sys/kernel/sched_rr_timeslice_ms 
100

Mitt OS bruker 100 millisekunder som timeslice
```


#### NB! En server trenger ikke god responstid, men en klientmaskin ønsker det
- Server ønsker god Throughput
- Klient ønsker god Responstid







### MLFQ (Multi Level Feedback que Que)
- Kompromiss, prøver å ta det beste fra alle
- Både MAC OS og Windows bruker MLFQ (Liinux bruker CFQ)
- Completely Fair Scheduler (IKKE PENSUM)
- Isteden for en kø, så brukes flere køer med ulik prioritet
    - Regler:
    1. If Priority(A) > Priority(B) => A runs, B doesnt
    2. If Priority(A) = Proiority(B) => A & B run in RR (RR praktiseres altså på hver kø)
    3. When a job enters the system, it gets highest priority
    4. a) If a job uses up an entire time slice, it moves down one que (gets 1 lower priority)
    4. b)If a job gives up the CPU before the time slice is up, it is at the same priority level (En job må gi opp CPU om den skal gjøre feks I/O)
     - Problem: prioritet kan skape starvation (løses med 5)
    5. After some time period S, move all the jobs in the system to the topmost que (top priority, kalles priority boost)
    - <b>Problem:</b> Om timeslice er statisk for alle system, kan det utnyttes av prosesser(program kan programmeres slik at de gir opp CPQ rett før timeslice er over, og forbli på høyeste prioritet alltid) 
    - <b>Løsning:</b> Bytte ut regel 4 a og b med: Once a job uses up its time allotment at a given level (regardless of how many times it has given up the CPU), its priority level is reduced (Eks: Du får maks 24 sekunder på høyeste prioritet)

    - Sammendrag
    1. Priority
    2. RR
    3. Processes start with top priority
    4. a) processes moves down prio when timeslice is spent
    4. b) prosesses that gives up CPU remain @ same prio
    5. Priority boost
    - alternativ til 4 a og b. Implementere time alotment 


    ```
    Eksempel med 8 prioritetsnivåer:
    [high priority] Q8 --> A --> B
                    Q7
                    Q6
                    Q5
                    Q4 --> C
                    Q3
                    Q2
    [Low priority]  Q1 --> D

    Forsøker å kjøre de høyest prioriterte prosessene først
    ```

```

```
 
    