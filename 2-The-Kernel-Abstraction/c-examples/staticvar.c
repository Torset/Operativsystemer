  #include <stdio.h>     /* printf */
  #include <unistd.h>     /* sleep */


int staticVar = 0;

int main() {
    
    staticVar += 1;

    sleep(10);

    printf("Address: %p Value: %d\n", &staticVar, staticVar);
    //printf("Address: %x Value: %d\n", &staticVar, staticVar);
    // %x i eksempelet gir nummer %p gir virtual address

    return 0;

}


//compile: gcc staticvar.c -o staticvar
