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