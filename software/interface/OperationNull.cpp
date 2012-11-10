#include "Operation.h"

void OperationNull::work()
{
  // ncurses stuff
  initscr();
  clear();
  cbreak();
  printw("Type q to quit\n");
  refresh();
  while (get_next_char() != 'q') {}

  // close ncurses
  endwin();
}
