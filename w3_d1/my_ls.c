#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

void print_permissions(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    struct passwd *pw;
    struct group *gr;
    char datestring[256];

    const char *path = (argc > 1) ? argv[1] : ".";

    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (lstat(fullpath, &file_stat) < 0) {
            perror("Error getting file stats");
            continue;
        }

        print_permissions(file_stat.st_mode);

        printf(" %2lu", file_stat.st_nlink);

        pw = getpwuid(file_stat.st_uid);
        printf(" %s", (pw) ? pw->pw_name : "unknown");

        gr = getgrgid(file_stat.st_gid);
        printf(" %s", (gr) ? gr->gr_name : "unknown");

        printf(" %8lld", (long long)file_stat.st_size);

        strftime(datestring, sizeof(datestring), "%b %d %H:%M", localtime(&file_stat.st_mtime));
        printf(" %s", datestring);

        printf(" %s\n", entry->d_name);
    }

    closedir(dir);
    return 0;
}