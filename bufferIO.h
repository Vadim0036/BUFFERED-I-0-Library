#ifndef BUFFER_IO_H
#define BUFFER_IO_H

#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 16

#define R O_RDONLY
#define W O_WRONLY
#define B O_RDWR
#define A O_APPEND

typedef struct
{
    char writing_buffer[BUFFER_SIZE];
    char reading_buffer[BUFFER_SIZE];
    int fd; 
    int offset; 
    int flag; 
    size_t size; 
} My_File;

My_File *open_file(const char *path, int flags);

int close_file(My_File *f);


int read_file();
int write_file();



#endif