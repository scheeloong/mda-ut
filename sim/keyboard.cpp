#include "keyboard.h"

void KeyboardInput::read_input(char key)
{
   switch (key)
   {
   case '0': // set speed from 0-9
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
      speed = (int)(key - '0');
      break;
   case '>':
      m.move(FORWARD, speed);
      break;
   case '<':
      m.move(REVERSE, speed);
      break;
   case '+':
      m.move(RISE, speed);
      break;
   case '-':
      m.move(SINK, speed);
      break;
   case '[':
      m.move(LEFT, speed);
      break;
   case ']':
      m.move(RIGHT, speed);
      break;
   case 'w': // forwards/back and turn is wasd
      m.translate(FORWARD);
      break;
   case 's':
      m.translate(REVERSE);
      break;
   case 'a':
      m.translate(LEFT);
      break;
   case 'd':
      m.translate(RIGHT);
      break;
   case '.':
      m.translate(STOP);
      break;
   default:
      break;
   }
}
