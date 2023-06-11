/**

 * @author Roberto Dias Algarte (robertodias70@outlook.com)
 * @brief SIMPLE MONITOR BASED ON HTOP
 * @version 0.1
 * @date 2023-06-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "ncurses_display.h"
#include "system.h"


int main() {
  System system;
  NCursesDisplay::Display(system);
}