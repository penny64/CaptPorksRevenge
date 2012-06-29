#include <conio.h>


int main(void)
{

 int number = 888500;

 int num1 = number >> 16;
 int num2 = (number >> 8) & 0xff;

 int num3 = number & 0xff;

 int out_number = 0;

 out_number += num1 << 16;
 out_number += num2 << 8;
 out_number += num3;

 cprintf("\n\rResult: number %i, num1 %i, num2 %i, num3 %i, out_number %i", number, num1, num2, num3, out_number);

 if (getch() == 0) getch();

 return 1;

}
