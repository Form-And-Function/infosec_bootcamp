#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>

#define MAX_PATH 1024

void print_process_info(const char *pid) {
    char path[MAX_PATH];
    char line[256];
    FILE *fp;
    struct passwd *pw;

    // Read process status
    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    fp = fopen(path, "r");
    if (fp == NULL) return;

    char name[256] = "";
    int uid = -1;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name: %255s", name);
        } else if (strncmp(line, "Uid:", 4) == 0) {
            sscanf(line, "Uid: %d", &uid);
            break;
        }
    }
    fclose(fp);

    // Read CPU and memory usage
    snprintf(path, sizeof(path), "/proc/%s/stat", pid);
    fp = fopen(path, "r");
    if (fp == NULL) return;

    long long unsigned int utime, stime, vsize;
    long long int rss;
    fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %llu %llu %*d %*d %*d %*d %*d %*d %*u %llu %lld", &utime, &stime, &vsize, &rss);
    fclose(fp);

    // Get username
    pw = getpwuid(uid);
    const char *username = pw ? pw->pw_name : "unknown";

    // Print process information
    printf("%-8s %-8s %-5s %6.2f %6.2f %s\n", 
           pid, 
           username, 
           name,
           (utime + stime) / sysconf(_SC_CLK_TCK) / 100.0, // CPU usage in seconds
           rss * sysconf(_SC_PAGESIZE) / 1024.0 / 1024.0,  // RSS in MB
           name);
}

int main() {
    DIR *dir;
    struct dirent *ent;

    printf("%-8s %-8s %-5s %6s %6s %s\n", "PID", "USER", "NAME", "CPU", "MEM", "COMMAND");

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_DIR) {
            char *endptr;
            long pid = strtol(ent->d_name, &endptr, 10);
            if (*endptr == '\0') {
                print_process_info(ent->d_name);
            }
        }
    }

    closedir(dir);
    return 0;
}