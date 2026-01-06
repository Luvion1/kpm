#include "visual_utils.h"
#include <sys/ioctl.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

// Fungsi untuk mencetak logo K# Package Manager
void print_logo() {
    printf(COL_CYAN COL_BOLD);
    printf("  _  __      " COL_MAGENTA "# #" COL_CYAN "  \n");
    printf(" | |/ /     " COL_MAGENTA "#####" COL_CYAN "  \n");
    printf(" | ' /       " COL_MAGENTA "# #" COL_CYAN "  \n");
    printf(" |  <       " COL_MAGENTA "#####" COL_CYAN "  \n");
    // Menggunakan \x5c untuk backslash agar aman dari urutan escape yang salah
    printf(" |_|\\\\_\\\\      " COL_MAGENTA "# #" COL_CYAN "  \n");
    printf(COL_RESET);
}

// Fungsi untuk menampilkan animasi loading sederhana
void show_loading_animation(const char* message, int duration) {
    if (!message) return; 
    
    const char* frames[] = {"|", "/", "-", "\\"};
    int frame_count = 4;
    
    printf(" %s ", message);
    
    for (int i = 0; i < (duration * 4); i++) {
        printf("\b%s", frames[i % frame_count]);
        fflush(stdout);
        usleep(250000); // 0.25 detik
    }
    
    printf("\b ");
}

// Fungsi untuk menampilkan progress bar
void show_progress_bar(int progress, int total, const char* message) {
    if (!message) return; 
    
    int t = (total <= 0) ? 100 : total;
    float percentage = (float)progress / (float)t * 100.0f;
    int bar_width = 40;
    int filled = (int)((float)bar_width * (float)progress / (float)t);
    
    printf("\r %s [", message);
    
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) {
            printf(COL_GREEN "█" COL_RESET);
        } else {
            printf(COL_CYAN "░" COL_RESET);
        }
    }
    
    printf("] %.1f%% (%d/%d)", percentage, progress, t);
    fflush(stdout);
}

// Fungsi untuk menampilkan animasi instalasi
void show_installing_animation(const char* package_name) {
    if (!package_name) return; 
    
    printf(COL_CYAN "\n  Installing package: " COL_BOLD "%s" COL_RESET "\n", package_name);
    
    // Simulasikan proses instalasi dengan progress bar
    for (int i = 0; i <= 100; i += 10) {
        show_progress_bar(i, 100, "Progress");
        usleep(100000); // 0.1 detik
    }
    printf("\n");
}

// Fungsi untuk menampilkan pesan status
void show_status_message(const char* message, const char* status) {
    if (!message || !status) return; 

    if (strcmp(status, "success") == 0) {
        printf(COL_BRIGHT_GREEN "  ✅ %s" COL_RESET "\n", message);
    } else if (strcmp(status, "error") == 0) {
        printf(COL_BRIGHT_RED "  ❌ %s" COL_RESET "\n", message);
    } else if (strcmp(status, "warning") == 0) {
        printf(COL_BRIGHT_YELLOW "  ⚠️  %s" COL_RESET "\n", message);
    } else if (strcmp(status, "info") == 0) {
        printf(COL_BRIGHT_CYAN "  ℹ️  %s" COL_RESET "\n", message);
    } else {
        printf("  %s\n", message);
    }
}
