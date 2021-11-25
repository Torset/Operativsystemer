# Computer architecture:

## CPU 

``` Henter / lagrer ting i minnet ```

### Registers 
<b>AX, BX, CX ...</b> (RAX = 64bit AX, EAX = 32 bit AX, AX = 16 bit)

<b>PC/IP</b> (Inneholder adressen inn i minnet, til neste instuksjon som skal utføres)

<b>IR</b> (Inneholder selve instruksjonen som skal utføres)

<b>SP, BP</b> (Stackpeker, Basepeker)

<b>PWS/FLAG</b> (Kontrollregister FLAG)

```Registre er den minste enhet i datamaskinenhvor man kan lagre data, her lagrer man typisk litt data fra 8 til 64 bit. Holder typisk et tall eller en adresse```

### Control Unit
    (CU)

### Arithmetic Logic Unit
    (ALU

### Memory Management Unit
    (MMU)
    CPU cache


## Memory
```Det som befinner seg i minnet: data og instruksjonen```


## I/O (Input/Output)



## Instructions

A simple c program:
```
int main() {
    int a = 2;
    int b = 3;
    int a = a + b;
    return a;
}
```

Expressed somewhat like so:
- Store the value 2 somewhere (called a)
- Store the value 3 somewhere (called b)
- Add the value of b to a
- Return the value of a

Expressed in assembly:
- movl eax, 0x2
- movl ebx, 0x3
- addl eax, ebx
- ret

### x86 arkitektur
- instructions (bestemt sett, ca 2000)
- registers (hvilke register finner)


### Regular instructions
- Moving data (mov)
- Math functions (add, sub)
- Function related (call, ret)
- Jumping (jmp, je, jne)
- Comparing (cmp)
- Stack (push, pop)


### CPU workflow

```
Instruction cycle AKA fetch, (decode,) execute cycle

Pseudocode:

while(not HALT) {   # while powered on
    IR=Program[PC]  # fetch instruction pointed to by PC to IR
    PC++            # increment PC
    execute(IR)     # execute instruction in IR
}

```

