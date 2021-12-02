# System calls

### fork()
- Brukt for å lage en ny prosess
- resultatet er ikke deterministisk (må synkromisere for å påse hvilken prosess som kjører først) 
- Skaper en eksakt kopi av den kjørende prosessen (dupliserer seg selv) (etter fork())
- int rc = fork() // Gjør at rc = returverdien til prosessen
- Parent returnerer PID til child
- Child returnerer 0

#### med dette kan vi si at denne koden kal utføres i child / parent eks:

```
if rc < 0 Fork feilet

if rc != 0 do something in parent

if rc = 0 do something in child
```


```
Prosess pid = 9277 Parent (rc = 79399)
int rc = fork();
Prosess pid = 79399 Child (rc = 0)

```

### getpid()
- printer ut pid til prosessen


### Copy On Write (COW)
- Legg inn denne ... !!!!!!
!!!!!!!!!!!!!!!!!!!!!!


### Parralellisering Wait()
- må passe på synkronisering
- kan gjøres med wait() systemkall

```
>         int wc = wait(NULL); // wait(NULL) = Vent på alle child prosesser som er skapt av denne
>         printf("hello, I am parent of %d (wc:%d((pid:%d\n",rc, wc, (int) getpid());
```


### exec()
- Vanlig med exec etter fork()
- exec fyller den nye prosessen med innhold

```
/// HER FÅR MAN CHILD TIL Å GJØRE NOE, men ikke paren

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
        char *myargs[3];
        myargs[0] = strdup("wc");   // program: "wc" (word count)
        myargs[1] = strdup("p3.c"); // argument: file to count
        myargs[2] = NULL;           // marks end of array
        execvp(myargs[0], myargs);  // runs word count
        printf("this shouldn't print out"); //exec() erstatter resten av prosessen i minnet, så den stopper der og denne vil ikke kjøre
    } else {
        // parent goes down this path (original process)
        int wc = wait(NULL);
        printf("hello, I am parent of %d (wc:%d) (pid:%d)\n",
            rc, wc, (int) getpid());
    }
    return 0;
}
```


### Windows har CreateProcess som er fork() og exec() i samme systemkall


## Kernel!
- Nøkkelfunksjonaliteten i operativsystemet
- 


```
// Dette er kernel: 
$>ls -lh /boot/vmlinuz-5.13.0-22-generic 
-rw------- 1 root root 9.8M Nov  5 05:51 /boot/vmlinuz-5.13.0-22-generic
$> 
```

### Hva gjør kernel for å starte en prosess:

#### OS                                     Program
1.  Lager en entry for prosess listen
2.  allokerer minne for programmet
3.  Laster programmet inn i minnet(fra disk)
4.  Settert opp stack med argc/argv
5.  Tømmer alle registre på CPU
6.  Execute call main()
7.                                          Kjører main()
8.                                          Execute return fra main
9.  Frigjør minnet som prosessen hadde
10. Fjerner prosessen frra prosess listen

- Hva om det er noe feil?
- Vi må ha en form for kontroll

## Dual mode operation
- mode bestemmes av en bit i FLAGS registeret
- bit 12-13 bestemmer om vi er i user eller kernel
    - 00 = user mode
    - 11 = kernel mode
    - 4 tilstander, men viser bare 2 her, de to andre (01 /10 kommer vi tilbake til i virtualisering)

### Kernel mode
- OS kjører i kernel mode
- Bare kernel får lov til å gjøre alt
- Bare kernel får gjøre I/O

### User mode
- Program kjører i user mode
- Program får lese og skrive til minnet (sitt eget adresseområde)
- Program får bruke CPU
- program må be kernel om å gjøre priviligerte opperasjoner




## OS @ boot                        Hardware
### (Kernel mode)
1. Initialize interrupt vector table
2.                                  Remember address of-
                                    syscall handler


## OS @ run                         Hardware                    Program
### (Kernel mode)                                               (User mode)
1. Create entry for process list
2. Allocate memory for program
3. Load program into memory
4. Setup user stack with argv
5. Fill kernel stack with reg/PC 
(Egen kernel stack for hver prosess, brukes til å lagre tilstanden når man bytter ut et program)
6. return from trap
7.                                  restore regs
8.                                  (from kernel stack)
9.                                  move to user mode
10.                                 jump to main
11.                                                             Run main()
12.                                                             ...
13.                                                             Call system call
14.                                                             trap to kernel
15.                                 save registers
16.                                 (to kernel stack)
17.                                 move to kernel mode
18.                                 jump to trap handler
19. Handle trap
20. Do work with syscall
21. Return from trap
22.                                 restore registers(from kernel stack)
23.                                 move to user mode
24.                                 jump to PC after trap
25.                                                             ... return from main()
26.                                                             trap (via exit())
27. Free memory of process
28. Remove from process list



### Terminologi
- User mode (application)
- Kernel mode (OS)
- Mode switch (between user / kernel mode)
- Context switch (between processes)


### Når kjører kernel? Tre typer interrupts:
1. (Trap) Software interrupt/SystemCall (Synkront)
2. (Trap) Exeption (Eks: deler på 0 / aksesserer ulovlig minne) (synkront)
3. Hardware interrupt (Eks: timer interrupt) (asynkront) 

- (synkront = skjer som konsekvens av en instruksjon)
- (asynkront = OS kan ikke forutse, overaskende for OS)



##### Laboppgave 

```
//Forksync

#include <stdio.h>     /* printf */
  #include <stdlib.h>    /* exit */
  #include <unistd.h>    /* fork */
  #include <sys/wait.h>  /* waitpid */
  #include <sys/types.h> /* pid_t */
  /* Note: pid_t is probably just an int, but it might be different
     kind of ints on different platforms, so using pid_t instead of
     int helps makes the code more platform-independent 
  */

  void process(int number, int time) {
    printf("Prosess %d kjører\n", number);
    sleep(time);
    printf("Prosess %d kjørte i %d sekunder\n", number, time);
  }

int main(void) {

//fork() parent returnerer pid, child returnerer 0
int rc = fork();

//Derfor blir rc == 0 på child
while (rc == 0){
        //Child (new process)
int rc1 = fork();
        while (rc1 == 0){
        process(0,1);
        exit(0);
}
int rc3 = fork();
        while (rc3 == 0){
        process(2,3);
        exit(0);
}
        waitpid(rc1, NULL, 0);
int rc4 = fork();
        while (rc4 == 0){
        process(1,2);
        exit(0);
}
int rc2 = fork();
        while(rc2 == 0){
        process(4,3);
        exit(0);
}
        waitpid(rc4, NULL, 0);
int rc5 = fork();
        while (rc5 == 0){
        process(3,2);
        exit(0);
}
        waitpid(rc2, NULL, 0);
        process(5,3);
        exit(0);
}
waitpid(rc, NULL, 0);

printf("Finishing parent process PID: %d\n", getpid());


return 0;
}


```




