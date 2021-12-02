# Condition variables and Semaphores


### Liten intro
- Vi har sett på låser for å begrense tilgang til delte ressurser til en tråd. 
- Dessverre er ikke låser det eneste som trengs for å bygge flertråd program (raralelle program)
- Spessielt er det mange ganger at en tråd ønsker å sjekke om en kondisjon er sann før den fortsetter eks:
    - En parent thread ønsker å sjekke om child er ferdig før den fortsetter (ofte klallt join())
- Hvordan skal denne ventingen bli implementert?

Summary: 
- Lås = begrense adgang
- ConditionVariables = rekkefølgen


#### Problemet Parent waiting for child:

Eks:

```
void *child(void *arg) {
    printf("child\n");
    // XXX how to indicate we are done ?
    return NULL;
}

int main (int argc,m char *argv[]) {
    printf("parent: begin\n");
    pthread_t c;
    pthread_create(&c, NULL, child, NULL); // create child
    // XXX how to wait for child?
    printf("parent: end\n");
    return 0;
}
```

#### Spin based approach
- Fungerer, men er veldig lite effektivt, siden den sløser CPU cycler med å spinne når den venter.

```
volatile int done = 0;
//global, men fungerer siden kun en tråd inkrementerer.

void *child(void *arg) {
    printf("child\n");
    done = 1;                //Endret
    return NULL;
}

int main (int argc,m char *argv[]) {
    printf("parent: begin\n");
    pthread_t c;
    pthread_create(&c, NULL, child, NULL); // create child
    while (done == 0)
        ; //spin (busy waiting)
    printf("parent: end\n");
    return 0;
}
```

while(done == 1); er en spinlock

Optimalt ønsker vi at parent skal sove/være blokkert fram til den fåt et signal om å starte igjen. Dette gjør vi med condition variables.

## Condition variables
- For å vente på at en kondisjon skal bli sann, så kan en tråd bruke en condition variable.
- En Condition variable er en "kø" som tråder setter seg selv i når kondisjon ikke er som ønsket.
- når denne kondisjonen endres, så kan tråder i den "køen" vekkes, og på den måten la dem fortsette (gjøres med et signal)

- For å deklarere en slik variabel kan man skrive:
```pthread_cond_t c;``` 
    - som deklarerer "c" som condition variable
- En condition variable har to opperasjoner:
    1. wait() - kalles når tråden ønsker å sette seg selv i sleep
    2. signal() - kalles når en trad er ferdig og vil signalisere dette til ventende tråder.

#### Eksempel med mutex og ConditionVariable

- Best practice er å alltid holde en lås når du skal sende et signal
```
int done  = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c  = PTHREAD_COND_INITIALIZER;

void thr_exit() {
    Pthread_mutex_lock(&m);
    done = 1;
    Pthread_cond_signal(&c); //signal når ferdig
    Pthread_mutex_unlock(&m);
}

void *child(void *arg) {
    printf("child\n");
    thr_exit();
    return NULL;
}

void thr_join() {
    Pthread_mutex_lock(&m);
    while (done == 0)
    Pthread_cond_wait(&c, &m); //setter seg i kø"c" venter på signal og frigir låsen 
    Pthread_mutex_unlock(&m); 
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    pthread_t p;
    Pthread_create(&p, NULL, child, NULL);
    thr_join();
    printf("parent: end\n");

return 0; 
}
``` 

##### Producer og Consumer:

```
$>ls -l | wc -l
```
- Producer prosess: ls -l
- Consumer prosess: wc-l

- Producer produserer noe data 
- Consumer tar dataen å gjør noe med den

Noen produserer noe, og legger det i en kø, så tar consumer å tar det ut

### Producer consumer problem:
- Kallt Bounded Buffer
- Helt vanlig problem
1. Bare en tråd kan aksessere bufferen av gangen
2. En producer kan ikke legge noe i en buffer som er full
3. En Consumer kan ikke fjerne noe fra en tom buffer

Interessant fordi:
- Vi må styre tilgangen 
- På et tidspunkt må en producer blokkere når buffer er full
- På et tidspunkt må en consumer blokkere når buffer er tom
- Vi må synkronisere rekkefølgen


##### Eksempel - enkelt
- Her legger vi noe i buffer "put" og tar noe ut "get"
```
int buffer;
int count = 0; //initally empty

void put(int value) {
    assert(count == 0);
    count = 1;
    buffer = value;
}

int get(){
    assert(count == 1);
    count = 0;
    return buffer;
}
```
- Her har vi to tråder, 1 producer og 1 consumer
- producer skal hele tiden forsøke å leggee noe i buffer (put)
- consumer skal hele tiden forsøke å hente noe ut av buffer (get)
```
void *producer(void *arg){
    int i;
    int loops = (int) arg;
for (i = 0; i < loops; i++) {
        put(i);
    }
}

void *consumer(void *arg) {
    while (1) {
        int tmp = get();
        printf("%d\n", tmp);
    }
}
```

#### Dårlig løsning: Single CV and IF
- Fungerer ikke med flere consumers
- bruker condition variable
- bruker mutex lås
- count == 1 (buffer full)

```
int loops; // must initialize somewhere...
cond_t  cond;
mutex_t mutex;

void *producer(void *arg) {
int i; 
for (i = 0; i < loops; i++) {
    Pthread_mutex_lock(&mutex);                 //p1
    if (count == 1)                             //p2  
        Pthread_cond_wait(&cond, &mutex);       //p3
    put(i);                                     //p4
    Pthread_cond_signal(&cond);                 //p5
    Pthread_mutex_unlock(&mutex);               //p6
    }
}

void *consumer(void *arg) {
int i;
for (i = 0; i < loops; i++) {
    Pthread_mutex_lock(&mutex);                 //c1
    if (count == 0)                             //c2
        Pthread_cond_wait(&cond, &mutex);       //c3
    int tmp = get();                            //c4
    Pthread_cond_signal(&cond);                 //c5
    Pthread_mutex_unlock(&mutex);               //c6
    printf("%d\n", tmp);                        

    }
}

```
- Flere consumers gjør at denne blir feil pga if condition
    - sjekker bare en gang 
- Denne løsningen kal fikses med en spinlock
    - while (count == 0)


#### Dårlig løsning: Single CV and while
- problem: flere tråder og bare 1 CV
- CV kan ende opp med å vekke en annen tråd, isteden for producer

```
int loops; // must initialize somewhere...
cond_t  cond;
mutex_t mutex;

void *producer(void *arg) {
int i; 
for (i = 0; i < loops; i++) {
    Pthread_mutex_lock(&mutex);                 //p1
    while (count == 1)                             //p2  
        Pthread_cond_wait(&cond, &mutex);       //p3
    put(i);                                     //p4
    Pthread_cond_signal(&cond);                 //p5
    Pthread_mutex_unlock(&mutex);               //p6
    }
}

void *consumer(void *arg) {
int i;
for (i = 0; i < loops; i++) {
    Pthread_mutex_lock(&mutex);                 //c1
    while (count == 0)                             //c2
        Pthread_cond_wait(&cond, &mutex);       //c3
    int tmp = get();                            //c4
    Pthread_cond_signal(&cond);                 //c5
    Pthread_mutex_unlock(&mutex);               //c6
    printf("%d\n", tmp);                        

    }
}

```



#### Riktig løsning!!: Multiple CVs and while
- Egen CV for producer
- Egen CV for consumer
- Producer venter på empty   //Vet da at noe er tatt ut av consumer
- Producer signaliserer fill //når den har lagt noe i buffer
- Consumer venter på fill //Vet da at det er noe i buffer
- Consumer signaliserer //når den har tatt noe ut av buffer

- Har nå en fungerende versjon


```
int loops; // must initialize somewhere...
cond_t  empty, fill; // 2 stk CV
mutex_t mutex;

void *producer(void *arg) {
int i; 
for (i = 0; i < loops; i++) {
    Pthread_mutex_lock(&mutex);                 //p1
    while (count == 1)                          //p2  
        Pthread_cond_wait(&empty, &mutex);      //p3
    put(i);                                     //p4
    Pthread_cond_signal(&fill);                 //p5
    Pthread_mutex_unlock(&mutex);               //p6
    }
}

void *consumer(void *arg) {
int i;
for (i = 0; i < loops; i++) {
    Pthread_mutex_lock(&mutex);                 //c1
    while (count == 0)                          //c2
        Pthread_cond_wait(&fill, &mutex);       //c3
    int tmp = get();                            //c4
    Pthread_cond_signal(&empty);                //c5
    Pthread_mutex_unlock(&mutex);               //c6
    printf("%d\n", tmp);                        

    }
}
```

##### NB! Broadcast
- Det fins et signal for å vekke alle
- pthread_cond_broadcast()
- Vekker alle ventende tråder


```
Test denne mer (om tid)
$> ~/Operativsystemer/Gjøvik/iikos-files/08-semaph/3-en-producer-consumer-mutex-og-condvar
```



## Semaphore (kombinasjon av lås og CV)

- Alternativet til å bruke låser og CV er Semaphorer
- En Semaphor er en konstruksjon som kombinerer låser og CV
- Semaforer blokkerer bare når verdien er 0!

Semaphor:
er en datatype med 2 operasjoner
1. sem_wait("down()")   - Kalles down
2. sem_post("up()")     - Kalles up

```
#include <semaphore.h>
sem_t s;                    // Deklarerer "s" til å være semafor
sem_init(&s, 0, 1);         // Initialiserer lik 
```
sem_init(&s, 0, 1)
&s = navnet
0  = delt mellom tråder(skal alltid være 0)
1  = startverdien (dette er det interessante)

- Startverdien sier hvor mange tråder som kan ha tilgang samtidig
    - Inkrementert av sem_post()
    - Dekrementert av sem_wait()

##### up / down 
- 0 / negative:
    - Om en prosess / tråd gjør en down(sem_wait()) på en semafor som er 0 eller negativ, så blir den blokkert (satt i waiting køen)
        - Blokkerer tråden

    - Om en prosess / tråd gjør en up(sem_post()) på en semafor som er 0 eller negativ, så blir en av prosessene fjernet fra waiting køen (becomes unblocked)
        - Vekker en av trådene som venter

- Et semafor som er initsialisert til 1 (binærsemafo) Kan brukes som en lås
- Vitenker oss at:
    - Den negative verdien representerer  antall prosesser / tråder som venter på det semaforet
    - Vi kan ikke hente ut den negative verdien og bruke den pga at det ikke vises på Linux
    
#### BinarySemaphore example 

- X = 1 når semaforen skal brukes som en lås

```
sem_t m;
sem_init(&m, 0, X); //init to X what should X be?

sem_wait(&m);
//critical section here
sem_post(&m)
```


#### Semaphore parent waiting for child example

- X = 0 her, da den skal fungere som en lås
1. Parent lager child og venter på s
2. Child gjør seg ferdig og inkrementerer sem til 1
3. Derav signaliserer child at den er ferdig og parent vekkes

```
sem_t s;

void *child(void *arg) {
    printf("child\n");
    sem_post(&s); // signal here: child is done
    return NULL;
}

int main(int argc, char *argv[]){
    sem_init(&s, 0, X); // what should X be?
    printf("parent: begin\n");
    pthread_t c;
    Pthread_create(&c, NULL, child, NULL);
    sem_wait(&s); // wait here for child
    printf("parent: end\n");
    return 0;
}
```



###  Lets solve the  Producer Consumer problem with semaphores
- Adding full and empty conditions
- Virker ikke når buffer er større enn 1



```
sem_t empty;
sem_t full;

void *producer(void *arg) {
int i; 
for (i = 0; i < loops; i++) {
        sem_wait(&empty);      
        put(i);    
        sem_post(&full)                               
               
    }
}

void *consumer(void *arg) {
int tmp = 0
while (tmp != -1) {
    sem_wait (&full);
    tmp = get();
    sem_post(&empty);
    printf("%d\n", tmp);
    }

}
int main(int argc, char *argv[]){
// ...
sem_init(&empty, 0, MAX);   // MAX are empty
sem_init(&full, 0, 0);      // 0 are full
// ...
}

```

- Her har vi bare synkronisering for fullt buffer / tom buffer
    - Fortsatt er det flere som kan legge inn ting samtidig i buffer
    - Vi må legge inn beskyttelse
    - Binærsemafor:

#### Adding mutual exclusion incorrectly:

- Telle ned på annet semafor mens man holder låsen kan føre til deadlock

```
// Dette eksempelet gir en deadlock:

void *producer(void *arg) {
int i; 
for (i = 0; i < loops; i++) {
    sem_wait(&mutex);              // Låser kritisk sektor
        sem_wait(&empty);          // Teller ned på et annet semafor
        put(i);                    
        sem_post(&full)             
    sem_post(&mutex);              // New line -                 
               
    }
}

void *consumer(void *arg) {
int tmp = 0
for (i = 0; i < loops; i++) {
    sem_wait(&mutex);               // New line
        sem_wait (&full);
        tmp = get();
        sem_post(&empty);
    sem_post(&mutex);               // New line
    printf("%d\n", tmp);
    }

}

```


#### Adding mutual exclusion Correctly:

- Bruker binærsemafor kun rundt opperasjonen, ikke rundt en annen semafor
- Binærsemafor virker på akkurat samme måte som en lock (mutex)

```


void *producer(void *arg) {
int i; 
for (i = 0; i < loops; i++) {       
        sem_wait(&empty);   
    sem_wait(&mutex);         // Binærsem kun rundt opperasjonen
        put(i);   
    sem_post(&mutex);         // Binærsem kun rundt opperasjonen     
        sem_post(&full)             
            
               
    }
}

void *consumer(void *arg) {
int tmp = 0
for (i = 0; i < loops; i++) {      
        sem_wait (&full);
    sem_wait(&mutex);           // Binærsem kun rundt opperasjonen
        tmp = get();
    sem_post(&mutex);  
        sem_post(&empty);       // Binærsem kun rundt opperasjonen
    printf("%d\n", tmp);
    }

}

```


### Barrier (Synkroniseringsmulighet)
- Kan være nyttig for å vente på et set med tåder
- pthread_barrier_wait()
- barrier_example.c:

    - Her må alle tråder vente til alle trådene har kommet til barrier

```
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define THREAD_COUNT 4

pthread_barrier_t mybarrier;

void* threadFn(void *id_ptr) {
  int thread_id = *(int*)id_ptr;
  int wait_sec = 1 + random() % 5;
  printf("thread %d: Wait for %d seconds.\n", thread_id, wait_sec);
  sleep(wait_sec);
  printf("thread %d: I'm ready...\n", thread_id);

  pthread_barrier_wait(&mybarrier);

  printf("thread %d: going!\n", thread_id);
  return NULL;
}


int main() {
  int i;
  pthread_t ids[THREAD_COUNT];
  int short_ids[THREAD_COUNT];

  srand(time(NULL));
  pthread_barrier_init(&mybarrier, NULL, THREAD_COUNT + 1);

  for (i=0; i < THREAD_COUNT; i++) {
    short_ids[i] = i;
    pthread_create(&ids[i], NULL, threadFn, &short_ids[i]);
  }

  printf("main() is ready.\n");

  pthread_barrier_wait(&mybarrier);

  printf("main() is going!\n");

  for (i=0; i < THREAD_COUNT; i++) {
    pthread_join(ids[i], NULL);
  }

  pthread_barrier_destroy(&mybarrier);

  return 0;
}

Output:
thread 1: Wait for 2 seconds.
thread 0: Wait for 2 seconds.
thread 2: Wait for 4 seconds.
main() is ready.
thread 3: Wait for 1 seconds.
thread 3: I'm ready...
thread 0: I'm ready...
thread 1: I'm ready...
thread 2: I'm ready...
thread 2: going!
thread 0: going!
thread 3: going!
thread 1: going!
main() is going!
```

- Her lages 4 tråder som alle (på et tidspunkt i koden) må vente til alle trådene er der.
- Så kan alle gå videre samtidig


#### Monitor
- Siden synkronisering er vanskelig, så har noen språk en monitor som gjør det lett for oss
- Java har dette nøkkelordet som heter synchronized
- Et java objekt som har syncronized nøkkelordet kalles en monitor
- Kort forklart, kompilatorengjør synkroniseringen for oss.

- I nyere C (ikke POSIX, men moderne C tråder)
    - Vi bruker ikke det
    - Det finnes atomic_int
        - Datatype som teller opp / ned atomisk
        - Kan telles opp og ned atomisk

### Deadlock (Vranglås)
- Når et set med tråder / prosesser alle venter på noe som bare en av dem kan trigge
    - Skjer bare når en tråd/prosess holder en ressurs (lock/sem) og prøver å skaffe en annen ressurs
- Alle tråder venter på hverandre = deadlock

- Kan unngås med 2 enkle regler VIKTIG!
1. Nummerer ressursene
2. Aksesser de i samme rekkefølge

    1. amutex, bmutex
    2. amutex må aksesseres før bmutex
    - Vises på de to eksemplene under

##### Eksempel: Som ikke skaper deadlock:

```

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define NITER 1000

struct ab {
        int a;
        int b;
};

struct ab A={0,0};
pthread_mutex_t amutex,bmutex;

void *IncThread(void *a)
{
    int i;
    for (i = 0; i < NITER; i++) {
       pthread_mutex_lock(&amutex);
       pthread_mutex_lock(&bmutex);
       A.a++;
       A.b++;
       pthread_mutex_unlock(&amutex);
       pthread_mutex_unlock(&bmutex);
    }
    return 0;
}

void *DecThread(void *a)
{
    int i;
    for (i = 0; i < NITER; i++) {
       pthread_mutex_lock(&amutex);
       pthread_mutex_lock(&bmutex);
       A.a--;
       A.b--;
       pthread_mutex_unlock(&bmutex);
       pthread_mutex_unlock(&amutex);
    }
    return 0;
}

int main(int argc, char * argv[])
{
    pthread_t tid1, tid2;
    pthread_mutex_init(&amutex,0);
    pthread_mutex_init(&bmutex,0);
    pthread_create(&tid1, NULL, IncThread, NULL); /* start tråder */
    pthread_create(&tid2, NULL, DecThread, NULL);
    pthread_join(tid1, NULL);                     /* vent på tråder */
    pthread_join(tid2, NULL); 
    printf("A.a is %d, A.b is %d\n", A.a, A.b);
    return 0;
}
```


##### Eksempel: Som skaper deadlock:

```

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define NITER 1000

struct ab {
        int a;
        int b;
};

struct ab A={0,0};
pthread_mutex_t amutex,bmutex;

void *IncThread(void *a)
{
    int i;
    for (i = 0; i < NITER; i++) {
       pthread_mutex_lock(&bmutex);  //årsaken, må aksessere a først
       pthread_mutex_lock(&amutex);
       A.a++;
       A.b++;
       pthread_mutex_unlock(&amutex);
       pthread_mutex_unlock(&bmutex);
    }
    return 0;
}

void *DecThread(void *a)
{
    int i;
    for (i = 0; i < NITER; i++) {
       pthread_mutex_lock(&amutex);     
       pthread_mutex_lock(&bmutex);
       A.a--;
       A.b--;
       pthread_mutex_unlock(&bmutex);
       pthread_mutex_unlock(&amutex);
    }
    return 0;
}

int main(int argc, char * argv[])
{
    pthread_t tid1, tid2;
    pthread_mutex_init(&amutex,0);
    pthread_mutex_init(&bmutex,0);
    pthread_create(&tid1, NULL, IncThread, NULL); /* start tråder */
    pthread_create(&tid2, NULL, DecThread, NULL);
    pthread_join(tid1, NULL);                     /* vent på tråder */
    pthread_join(tid2, NULL); 
    printf("A.a is %d, A.b is %d\n", A.a, A.b);
    return 0;
}
```
### Forklaring Deadlock (eksemplene)
- Det siste eksempelet låser vi i forskjellig rekkefølge
    - DVS at Tråd1 kan holde lås bmutex og vente på amutex
    - samtidig som Tråd2 holder amutex mens den venter på bmutex



