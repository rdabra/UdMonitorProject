#include "ncurses_display.h"

#include <curses.h>

#include <algorithm>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "format.h"
#include "system.h"

using std::string;
using std::to_string;

// 50 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string NCursesDisplay::ProgressBar(double percent) {
  std::string result{"0%"};
  int size{50};
  double bars{percent * size};

  for (int i{0}; i < size; ++i) {
    result += i <= bars ? '|' : ' ';
  }

  string display{to_string(percent * 100).substr(0, 4)};
  if (percent < 0.1 || percent == 1.0)
    display = " " + to_string(percent * 100).substr(0, 3);
  return result + " " + display + "/100%";
}

void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
  int row{0};
  mvwprintw(window, ++row, 2, ("OS: " + system.OperatingSystem()).c_str());
  mvwprintw(window, ++row, 2, ("Kernel: " + system.Kernel()).c_str());

  std::vector<Processor> cpus{system.Cpus()};
  mvwprintw(window, ++row, 2, "CPU: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(cpus[0].utilization()).c_str());
  wattroff(window, COLOR_PAIR(1));

  /**
   * @brief  Displaying every cpu utilization until the text reaches maxximum
   * screen horizontal size
   */
  std::string line{"CPUn(%): "};
  for (unsigned i = 1; i < cpus.size(); i++) {
    std::string uz{to_string(cpus[i].utilization())};
    line +=
        to_string(i - 1) + "(" + uz.substr(0, uz.find_first_of(".") + 3) + ") ";
  }
  unsigned x_max{(unsigned)getmaxx(stdscr) - 5};
  if (line.size() > x_max) {
    line = line.substr(0, x_max - 4) + " ...";
  }
  mvwprintw(window, ++row, 2, line.c_str());

  mvwprintw(window, ++row, 2, "Memory: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "");
  wprintw(window, ProgressBar(system.MemoryUtilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2,
            ("Total Processes: " + to_string(system.TotalProcesses())).c_str());
  mvwprintw(
      window, ++row, 2,
      ("Running Processes: " + to_string(system.RunningProcesses())).c_str());
  mvwprintw(window, ++row, 2,
            ("Up Time: " + Format::ElapsedTime(system.UpTime())).c_str());
  wrefresh(window);
}

void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes,
                                      WINDOW* window, int n) {
  int row{0};
  int const pid_column{2};
  int const user_column{9};
  int const cpu_column{16};
  int const ram_column{26};
  int const time_column{35};
  int const command_column{46};
  wattron(window, COLOR_PAIR(2));
  mvwprintw(window, ++row, pid_column, "PID");
  mvwprintw(window, row, user_column, "USER");
  mvwprintw(window, row, cpu_column, "CPU[%%]");
  mvwprintw(window, row, ram_column, "RAM[MB]");
  mvwprintw(window, row, time_column, "TIME+");
  mvwprintw(window, row, command_column, "COMMAND");
  wattroff(window, COLOR_PAIR(2));

  /**
   * @brief Sorting processes in descending order of cpu utilization (a task of
   * the presentation layer, in my opinion)
   */
  auto descending_predicate{
      [](const Process& a, const Process& b) -> bool { return (a > b); }};
  std::sort(processes.begin(), processes.end(), descending_predicate);
  int n_processes{n > (int)processes.size() ? (int)processes.size() : n};

  for (int i = 0; i < n_processes; ++i) {
    mvwprintw(window, ++row, pid_column,
              (string(window->_maxx - 2, ' ').c_str()));
    mvwprintw(window, row, pid_column, to_string(processes[i].pid()).c_str());
    mvwprintw(window, row, user_column, processes[i].user().c_str());
    float cpu = processes[i].cpuUtilization();
    mvwprintw(window, row, cpu_column, to_string(cpu).substr(0, 4).c_str());
    std::string ram = processes[i].ram();
    mvwprintw(window, row, ram_column,
              ram.substr(0, ram.find_first_of(".") + 2).c_str());

    mvwprintw(window, row, time_column,
              Format::ElapsedTime(processes[i].elapsedTime()).c_str());
    mvwprintw(window, row, command_column,
              processes[i].command().substr(0, window->_maxx - 46).c_str());
  }
}

void NCursesDisplay::Display(System& system, int n) {
  initscr();      // start ncurses
  noecho();       // do not print input values
  cbreak();       // terminate ncurses on ctrl + c
  start_color();  // enable color

  int x_max{getmaxx(stdscr)};
  WINDOW* system_window = newwin(10, x_max - 1, 0, 0);
  WINDOW* process_window =
      newwin(3 + n, x_max - 1, system_window->_maxy + 1, 0);

  while (1) {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    box(system_window, 0, 0);
    box(process_window, 0, 0);
    DisplaySystem(system, system_window);
    DisplayProcesses(system.Processes(), process_window, n);
    wrefresh(system_window);
    wrefresh(process_window);
    refresh();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  endwin();
}
