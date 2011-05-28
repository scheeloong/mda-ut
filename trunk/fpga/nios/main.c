#include <stdio.h>

void print(char *str)
{
  while(*str != '\0') {
    putchar(*str++);
  }
  putchar('\n');
}

int main()
{
  while (1) {
    print("Hello World!");
    while(getchar() != '\n') ;
  }
  return 0;
}
