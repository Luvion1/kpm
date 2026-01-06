#ifndef VISUAL_UTILS_H
#define VISUAL_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Fungsi-fungsi untuk efek visual dan animasi CLI
void print_logo();
void show_loading_animation(const char* message, int duration);
void show_progress_bar(int progress, int total, const char* message);
void show_installing_animation(const char* package_name);
void show_status_message(const char* message, const char* status);

// Warna untuk output
#define COL_RESET           "\033[0m"
#define COL_BLACK           "\033[0;30m"
#define COL_RED             "\033[0;31m"
#define COL_GREEN           "\033[0;32m"
#define COL_YELLOW          "\033[0;33m"
#define COL_BLUE            "\033[0;34m"
#define COL_MAGENTA         "\033[0;35m"
#define COL_CYAN            "\033[0;36m"
#define COL_WHITE           "\033[0;37m"
#define COL_BRIGHT_BLACK    "\033[0;90m"
#define COL_BRIGHT_RED      "\033[0;91m"
#define COL_BRIGHT_GREEN    "\033[0;92m"
#define COL_BRIGHT_YELLOW   "\033[0;93m"
#define COL_BRIGHT_BLUE     "\033[0;94m"
#define COL_BRIGHT_MAGENTA  "\033[0;95m"
#define COL_BRIGHT_CYAN     "\033[0;96m"
#define COL_BRIGHT_WHITE    "\033[0;97m"
#define COL_BOLD            "\033[1m"
#define COL_DIM             "\033[2m"
#define COL_ITALIC          "\033[3m"
#define COL_UNDERLINE       "\033[4m"
#define COL_BLINK           "\033[5m"
#define COL_REVERSE         "\033[7m"
#define COL_BOLD_BLACK      "\033[1;30m"
#define COL_BOLD_RED        "\033[1;31m"
#define COL_BOLD_GREEN      "\033[1;32m"
#define COL_BOLD_YELLOW     "\033[1;33m"
#define COL_BOLD_BLUE       "\033[1;34m"
#define COL_BOLD_MAGENTA    "\033[1;35m"
#define COL_BOLD_CYAN       "\033[1;36m"
#define COL_BOLD_WHITE      "\033[1;37m"

#endif
