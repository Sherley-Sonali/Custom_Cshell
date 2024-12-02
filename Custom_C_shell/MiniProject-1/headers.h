#ifndef HEADERS_H
#define HEADERS_H

#include <pwd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/wait.h>
#include <ctype.h>
#include <grp.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <termios.h>
#include <arpa/inet.h>
#include <netdb.h>

#define COLOR_BOLD    "\033[1m"
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"  // Green for executables
#define COLOR_WHITE   "\033[37m"  // White for regular files
#define COLOR_BLUE    "\033[34m"  // Blue for directories
#define COLOR_RED   "\033[31m"  // Red
#define COLOR_ORANGE   "\033[33m"  // Orange

#endif