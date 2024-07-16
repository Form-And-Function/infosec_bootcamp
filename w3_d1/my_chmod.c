#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

mode_t parse_mode(const char *mode_str) {
    mode_t mode = 0;
    int i;
    
    if (mode_str[0] >= '0' && mode_str[0] <= '7') {
        // Numeric mode
        return (mode_t)strtol(mode_str, NULL, 8);
    }
    
    // Symbolic mode
    for (i = 0; mode_str[i]; i++) {
        switch(mode_str[i]) {
            case 'r': mode |= S_IRUSR | S_IRGRP | S_IROTH; break;
            case 'w': mode |= S_IWUSR | S_IWGRP | S_IWOTH; break;
            case 'x': mode |= S_IXUSR | S_IXGRP | S_IXOTH; break;
        }
    }
    
    return mode;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <mode> <file>\n", argv[0]);
        return 1;
    }

    const char *mode_str = argv[1];
    const char *file = argv[2];
    mode_t mode = parse_mode(mode_str);

    if (chmod(file, mode) == -1) {
        perror("chmod");
        return 1;
    }

    printf("Changed mode of %s to %o\n", file, mode);
    return 0;
}