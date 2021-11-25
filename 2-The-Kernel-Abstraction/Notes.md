# The kernel abstraction

### Remember
1. Trap = synchronous transfer from a less privileged => more privileged level
2. Interrupt = Asynchronous signal to CPU that some external event has occured that may require its attention
3. System call, when a user process needs the kernel to do some privileged work.

## Exercises page 90.

1. When a user process is interrupted or causes a processor exeption the x86 hardware switches the stack pointer to a kernel stack, before saving the current process state. Explain why.

```The pointer is set to the kernel stack, more spesifically the base of the kernels interrupt stack. The process state must be saved in a kernel level interrupt stack for reliability (if the process has a bug, the kernel handler must continue to run properly) and security (on multicore threads in the same process can modify user memory, if local variables are stored on the user-level stack, the program might be able to modify the kernels return address, potentially causing the kernel to jump to malicious code). ```

2. For the "Hello world" program, we mentioned that the kernel must copy the string from the user program to screen memory. Why must the screen`s buffer memory be protected? Explain what might happen if a malicious application could alter any pixel on the screen, not just those within its own window.

```The screen`s buffer memory must be protected to prevent user processes to read sensitive user data.```

```If a malicious application could alter any pixel on the screen, it could run malicios code that uses alot of CPU power, while altering the task-manager blaming other programs for the high CPU usage.```

3. For each of the three mechanisms that supports dual-mode operation: <b>privileged instructions</b>, <b>memory protection</b> , and <b>timer interrupts</b>, explain what might go wrong without that mechanism, assuming the system still had the other two. 

```Without privileged instructions the other two mechanisms would be obsolete, if pr program could run all instructions, it could disable the memory protection and timer interrupts```

```Without memory protection programs could read and write to the the kernel and gain control over the system, it could also read asensitive user data and modify login program to give a potential attacker full administrator rights```

```Without timer interrupts a user program could enter an infinite loop, consuming all resources of the computer, and the kernel would not have a way of regaining control of the processor```

4. Suppose you are tasked with designing the security system for a new web browser that supports rendering  web pages with embedded web page scripts. What checks would you  need to implement to ensure that executing buggy or malicious scripts could not corrupt or crash the browser?

``` 1. Are the instructions executed allowed for the pages privilege level?  ```

``` 2. Is the memory that the page is trying to access allowed according to its privilege level?  ```

``` 3. Are the reasources consumed by the page ok, according to its priority?  ```


5. Define three types of user-mode to kernel-mode transfers.

``` 1. Interrupts: (Asynchronous signal to CPU that some external event has occured that may require its attention) As the processor executes it periodically checks if an interrupt has occured, if it has, it completes or stalls execution and saves the state, and starts executing the designated interrupt handler in the kernel  ```

``` 2. System calls (user program want to execute privileged instructions) it is a service provided by the kernel that can be called from user-level ```

``` 3. Exeptions: User program divides by 0, tries to access memory outside its memory region, tries to execute privileged instructions) A hardware event caused by a user level program`s behaviour that causes a transfer of control to a kernel handler ```

6. Define four types of kernel mode to user mode transfers.

```1. New process: To start a new process, the kernel copies the program into memory, sets program counter(PC) to the first instruction of the process, sets the stack pointer to the base of the user stack and switches to user-mode```

```2. Resume after interrupt, exeption or system call: When finished it resumes execution of the process by restoring its PC, restores registers and switches to user-mode```

```3. Switch to a different process: Kernel saves current process state, registers PC, etc. in the Process Control Block(PCB). I then loads the PCB from the new process into the CPU and switches to user-mode```

```4. User level upcall: User programs can recieve asynchronous notification of events. Messages from the kernel. Kernel sends a signal to a user process, and switches to user-mode```

7. Most hardware architectures provide an instruction to return from an interrupt, such as iret. This instruction switches the mode of operation from kernel to user-mode

    a) Explain where in the operating system this instruction would be used.

    ```Iret can be used in all four types of kernel to user-mode transfers described in question 6```

    b) Explain what happens if an application program executes this instruction.

    ```The iret instruction will use the cs:eip, eflags and ss:esp from kernel's interrupt stack to restore the code segment, program counter, execution flags, stack segment, and stack pointer, thus switching the mode of operation from kernel to user-mode```

8. A hardware designer argues that there is now enough on-chip transistors to provide 1024 integer registers and 512 floating point registers. As a result, the compiler should almost never need to store anything on the stack. As an operating system guru, give your opinion of this design.

    a)  What is the effect on the operating system of having a large number of registers?

    ```The operating system will be able to execute mode switches very efficiently since it will not need to save the current process state on the stack, when there are som many availible registers```

    b) What hardware features would you recommend adding to the design?

    ``` a```

    c) What happens if the hardware designer also wants to add a 16-stage pipeline into the CPU, with precise exceptions. How would that affect the user-kernel switching overhead?

    ``` a```