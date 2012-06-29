#include <bios.h>
#include <conio.h>














int main(void)
{
// COM1 is serial port.

int i;
int buf [10];
     bioscom(0, 0xe3, 0); /* 9600 baud, no parity, one stop, 8 bits */
     
     for (i=0; i < 5; i++)
     {
       buf [i] = bioscom(2, 0, 0);
       if (buf [i] == 0) i--;
     }

       cprintf("Buffer: %i %i %i %i %i", buf [0], buf [1], buf [2], buf [3], buf [4]);


return 1;

}
