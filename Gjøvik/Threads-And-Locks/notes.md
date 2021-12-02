# Threads and locks

- Concurrency = Samtidighet = inter process kommunikasjone

### Thread
- Er en lettvekts prosess
- Lever innenfor en prosess
- Laget for å utnytte paralellisering innenfor en prosess
- Lever innenfor samme adresserom
- prosesser konkurrerer, tråder sammarbeider.

#### Hver tråd har:
- Sin egen stack
    - Betyr at de lokale variablene som tråden lager er det bare den tråden som kan se
    - Om tråden lager en variabel på heapen, så kan alle trådene se den
- Sin egen PC (Program Counter)
    - Naturlig nok, for de er som en mini prosess
- Sin egen tilstand
    - Kan være ready / running / bliocked
- Sine egne registre
    - Unikt for en tråd



#### Ellers deles alt annet innenfor prosessen
- En prosess med 2 tråder har 3 stacker i sitt adresserom
    - main + thread1 + thread2

#### Vi bruker tråder for:
1. Å paralellisere - Få brukt alle CPU cores
2. Å overlappe - I/O

### PCB 
- Prosess tilstand 
- All informasjon om en prosess
- ##### Hvor ligger den? ????

### TCB
- All informasjon om en tråd
- Inneholder:
    - a

#### Entråd prosess
- Prosess uten tråder


### pthread
- pthread_create() lager tråd
-pthread_exi() avslutter tråden
- pthread_join() (venter på at tråden er ferdig og avslutter tråd) (kan tenke på det som at tråden joiner hvovedtråden (prosessen))
- Kompileres med: -pthread (flag)
```$>gcc test.c -o test -pthread ```


```
/// Eksempel som lager 10 tråder

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define NUMBER_OF_THREADS 10

void *print_hello_world(void *tid) {
 printf("Hello World. Greetings from thread %ld\n", (intptr_t) tid);
 pthread_exit(NULL);
}

int main(void) {
 pthread_t threads[NUMBER_OF_THREADS];
 int status,j;
 intptr_t i;

 // Lager 10 tråder hvor hver tråd kjører funksjonen

 for (i=0; i < NUMBER_OF_THREADS; i++) {
  printf("Main here. Creating thread %ld\n", i);
  status = pthread_create(&threads[i], NULL, print_hello_world, (void *)i);

  if (status != 0) {
   printf("Oops. pthread create returned error code %d\n", status);
   exit(EXIT_FAILURE);
  }
 }
 for(j=0;j<NUMBER_OF_THREADS;j++) {
        pthread_join(threads[j], NULL);
 }
 return 0;
}
```

##### Output:
```
Main here. Creating thread 0
Main here. Creating thread 1
Main here. Creating thread 2
Hello World. Greetings from thread 0
Hello World. Greetings from thread 1
Main here. Creating thread 3
Hello World. Greetings from thread 2
Main here. Creating thread 4
Main here. Creating thread 5
Main here. Creating thread 6
Hello World. Greetings from thread 4
Main here. Creating thread 7
Main here. Creating thread 8
Hello World. Greetings from thread 5
Hello World. Greetings from thread 7
Hello World. Greetings from thread 8
Main here. Creating thread 9
Hello World. Greetings from thread 6
Hello World. Greetings from thread 9
Hello World. Greetings from thread 3
```

- Her ser vi at det ikke er noen kontroll over hvilken tråd som kjører når....
- OS slipper til tråder på CPU, ikke prosesser

### Shared data
- Som regel så skal disse trådene jobbe mot noen delte data
    - Eks array, filer, etc..
    - Lager vi globale variabler (på heap, utenfor main), så har alle tråder tilgang til den.

#### Når vi telle opp en variabel på heapen
counter++
counter + 1
- Dette er ikke en instruksjon, men det er 3
```
maskinkode for å inkrementere counter:

100 mov     0x8049alc, %eax
105 add     $0x1, %eax
108 mov     %eax, 0x8049alc

1. Hent fra addressn i minnet inn i et register
2. Legg til 1 i registeret
3. Skriv registeret tilbake til minnet

```
- Behandling av felles variabler (der ting kan gå galt i koden)
Kalles kritisk sektor

### Critical section
- Der ting kan gå galt i koden (Behandling av felles variabler)
- Aksessering og endring av felles ressurs

#### Problem: Race condition
- Utfallet avgjøres av hvilken tråd som får kjøre når
- Interrupt midt i en criticat section kan forstyrre og gi feil data
    - Eks:
    - T1 leser minnet = 50
        - T2 interrupt
        - T2 leser minnet = 50
        - T2 inkrementerer = 51
        - T2 Skriver til minnet = 51
    - T1 inkrementerer = 51
    - T1 Skriver til minnet = 51
    - Skulle vært 52

#### Løsning: Mutual exclusion
- Bare en tråd kan telle om gangen
- Instruksjonene må utføres atomisk
    - Atomisk: utføres som en udelelig enhet
    - Hvordan utføre instruksjoner atomisk?
        - Innføre locks

### Locks
- Problemet er interrupts, men kan ikke slå av interrupts i user mode
- Mål med locks:
    - Mutual exclusion
    - Fairness
    - Performance

``` 
1. lock_t mutex //en global allokert lock, som jeg kaller "mutex"
2. ...
3. lock(&mutex) // låser
4. balance = balance + 1 //Critical section
5. unlock(&mutex) //Gir opp låsen
```
- En lås er bare en variabel, så du må deklarere en låsevariabel

#### Pthread locks (POSIX biblioteket)
- C-koden vi må kunne om låsing
- Standard måte å initialisere en lås i C
    - PTHREAD_MUTEX_INITIALIZER
```
1. pthread_mutex_t lås = PTHREAD_MUTEX_INITIALIZER; // Init globalt
2. 
3. pthread_mutex_lock(&lås); 
4. balance = balance + 1;                   //Critical section
5. pthread_mutex_unlock(&lås);

```

#### Controlling Interrupts  (Ikke bruk)
- Kunne ha vært brukt istedenfor locks, men ikke heldig
    - User mode prosesser kan ikke få denne muligheten
    - Disableinterrupts fungere som regel bare på 1 CPU
```
void lock() {
    DisableInterrupts();
}
void unlock() {
    EnableInterrupts();
}
```

### Spin wait (SpinLock without test and set) Ikke trygt
Eksempel: Spinlock
- spin-wait står å spinner (mens den tester at flag = 1)
```
void lås(lock_t *mutex) {
while(TRUE)
; // Do nothing
mutex->flag = 1; // Set it
}
```
- trenger hardware støtte for å lese + skrive atomisk
    - Løses med test and set

### Test and set
- Kalles atomisk exchange
- Er funksjonalitet i hardware, men viser hva den gjør i følgende C kode:

```
int TestAndSet(int *old_ptr, int new){
    int old = *old_ptr;     //hent gammel verdi fra old_ptr
    *old_ptr = new;         //lagre "new" i old_ptr
    return old;             //return the old value
}

Er egentlig: les minne, skriv verdi i en atomisk operasjon
```

### Compare and swap
- Omtrent det samme som TestAndSwap, bare at du legger på en test 
inne i koden
- Brukes helt likt som test and set
```
int CompareAndSwap (int *ptr, int expected, int new) {
    int original = *ptr;
    if(original == expected)
    *ptr = new;
    return original;
}
```


### Spin wait (SpinLock with test and set) trygt
Eksempel: Spinlock
- spin-wait står å spinner (mens den tester at flag = 1)
- siden testAndSet er atomisk, så kan ikke interruptes mellom lesing og skriving
```
void lås(lock_t *mutex) {
while(TestAndSet(&lock_t *lock)) 
; // Do nothing
mutex->flag = 1; // Set it
}
```

### (Spin)Lock with testAndSet and Yield (Fungerer bra på 1 CPU)
- Fungerer ikke på multiprocessor
```
void init() {
    flag = 0;
}

void lock() {
    while (TestAndSet(&flag, 1) == 1)
    yield(); //give up the CPU
}

void unlock() {
    flag = 0;
}
```


### LOCK
- I hardware (assembly) så finnes en instruksjon som heter LOCK som kan brukes for at den følkgende instruksjonen skal være atomisk.
- Problem løses fordi va har hardware støtte
- LOCK gjør at vi kan bygge en lås som vi bruker i C-koden vår
```
 leaq   lock(%rip), %rax
        movq        %rax, %rdi
        call        pthread_mutex_lock@PLT
        movl        g_ant(%rip), %eax
        addl        $1, %eax
        movl        %eax, g_ant(%rip)
        leaq        lock(%rip), %rax
        movq        %rax, %rdi
        call        pthread_mutex_unlock@PLT
```


### Spin or switch?

- Skal jeg stå å spinne og teste, eller skal jeg tvinge frem en  switch til en annen tråd, eller evt en full context switch?

- Spin locks can be bad for performance (think uniprocessor,
round-robin, and 100 threads)
- Kanskje yielde CPU og legge seg tilbake i køen igjen ?
- Spin locks can be ok on multiprocessor if spinning time (waiting
time) is short
- Can be combined into a two-phase lock: spin a litte first, then
switch

#### Demo
```
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define NITER 1000000

int g_ant=0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *IncThread() {
    int i;
    for (i = 0; i < NITER; i++) {
    pthread_mutex_lock(&lock);
        g_ant++;
    pthread_mutex_unlock(&lock);
    }
    return 0;
}

void *DecThread() {
    int i;
    for (i = 0; i < NITER; i++) {
    pthread_mutex_lock(&lock);
        g_ant--;
    pthread_mutex_unlock(&lock);
    }
    return 0;
}

int main(void) {
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, IncThread, NULL); /* start tråder */
    pthread_create(&tid2, NULL, DecThread, NULL);
    pthread_join(tid1, NULL);                     /* vent på tråder */
    pthread_join(tid2, NULL); 

    if (g_ant != 0) {
        printf(" HUFF! g_ant er %d, skulle vært 0\n", g_ant);
    } else {
        printf(" OK! g_ant er %d\n", g_ant);
    }
    return 0;
}
```

































    







