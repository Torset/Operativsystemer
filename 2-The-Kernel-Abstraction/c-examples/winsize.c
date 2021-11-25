#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>



void func (int num)
{
        struct winsize w;
        ioctl(0, TIOCGWINSZ, &w);
        char buffer[256];

        snprintf(buffer, sizeof(buffer), "%d x %d\n", w.ws_col, w.ws_row);
        int len = strlen(buffer);

        //Clear
        printf("\033[2J");

        printf("\033[%d;%dH \033[1;31m", w.ws_row/2, (w.ws_col/2)-len/2);

        printf("%s", buffer);

        fflush(0);

}

int main (void)
{
        func("something");

        signal(SIGWINCH, func);

        while (1)
{
pause();

}
    return 0;
}
