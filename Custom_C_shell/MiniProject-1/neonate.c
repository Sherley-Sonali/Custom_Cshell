#include "neonate.h"

pid_t get_most_recent_pid() {
    DIR *dir;
    struct dirent *entry;
    pid_t latest_pid = 0;

    dir = opendir("/proc");
    if (dir == NULL) {
        printf("Error opening /proc directory\n");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Check if entry name is a number (PID)
            if (isdigit(entry->d_name[0])) {
                pid_t pid = atoi(entry->d_name);
                if (pid > latest_pid) {
                    latest_pid = pid;
                }
            }
        }
    }

    closedir(dir);
    return latest_pid;
}

// Function to check for a key press
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
