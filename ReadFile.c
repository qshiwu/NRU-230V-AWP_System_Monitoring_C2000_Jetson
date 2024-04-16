#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int read_file(const char *filename, char **content) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Open Fail %s。\n", filename);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    *content = (char *)malloc(size + 1);
    if (*content == NULL) {
        printf("malloc Fail\n");
        fclose(file);
        return 1;
    }

    size_t result = fread(*content, 1, size, file);
    if (result != size) {
        printf("Read Fail\n");
        fclose(file);
        free(*content);
        return 1;
    }
    (*content)[size] = '\0';

    fclose(file);
    return 0;
}

int file_exists(const char *filename) {
    return access(filename, F_OK) != -1;
}

int delete_file(const char *filename) {
    return remove(filename);
}
