#include "Operation.h"

void OperationNull::work()
{
  // ncurses stuff
  initscr();
  clear();
  cbreak();
  printw("Type q to quit\n");
  refresh();
  while (CharacterStreamSingleton::get_instance().get_next_char() != 'q') {}

  // close ncurses
  endwin();
}
