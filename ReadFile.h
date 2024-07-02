#ifndef READ_FILE_H
#define READ_FILE_H

int read_file(const char *filename, char **content);
int file_exists(const char *filename);
int delete_file(const char *filename);

#endif
