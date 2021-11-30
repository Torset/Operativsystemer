#include <stdio.h>

int main(void) {

    int i = 0;
    int j = 0;
    int *pek;
    

    for (; i<3; i++){
        printf("i er %d og adr er %p\n", i, &i);
    }

    pek = &i; // adressen til i

    for (; j<3; j++){
        printf("i er %d via pek er i %d, og pek er %p\n", i, *pek, pek );
        *pek+=1;
        // evt: (*pek)++;
    }
    
    return(0);
}