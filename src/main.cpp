#include <iostream>

#include "ncurses_display.h"
#include "system.h"
#include "format.h"

int main() {
  System system;
  NCursesDisplay::Display(system, 20);
}