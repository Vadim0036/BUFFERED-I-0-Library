#ifndef BUFFER_IO_H
#define BUFFER_IO_H

#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096    /* defines buffer size */ 

#define R O_RDONLY
#define W O_WRONLY
#define A O_APPEND

/*  defines File structure */
typedef struct
{

    char *writing[BUFFER_SIZE];  /* writing buffer */
    char *reading[BUFFER_SIZE];  /* reading buffer */
    int fd;                      /* file's descriptor */
    int offset;                  /* file's offset */
    int flag;                    /* file mode */
    size_t size;                 /* file size*/

} My_File;


/*
        My_File *open_file(const char *path, int flags);

        *path - is the path or name of the file 
        flag  - one of three file modes that are described below. 

Opens file with one of the following flags: 
    R - Read Mode

        if file does not exist, function returns NULL pointer
        and outputs error message
        if file exists, function opens the file in reading mode 
        and returns pointer to dynamically allocated file
    
    W - Write Mode

        if file does not exist, function will create the file
        and open it in the writing mode. 
        if file exists, function will open it in writing 
        mode 

    A - Append Mode 
        if file does not exist, function will create the file
        and open it in the writing and reading mode. 
        if file exists, function will open it in writing and reading
        mode 
*/
My_File *open_file(const char *path, int flags);

int close_file(My_File *f);


static char *init_read_buffer(My_File *file, size_t size);
static char *init_write_buffer(My_File *file, size_t size);

static int destruct_buffers();

static int loadbuffer(My_File *file, int buffer_type);

int read_file();
int write_file();

int fgetch();
int fgetst(My_File *file, char *dest, size_t size);

int fputca();
int fputst();


#endif