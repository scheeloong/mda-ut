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
  print("Hello World!");
  return 0;
}
