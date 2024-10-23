#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "bufferIO.h"


My_File *open_file(const char *path, int flag)
{
    int fd;
    My_File *file; 
    if(flag == R)
    {
        fd = open(path, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(fd == -1)
        {
            perror("Error occured reading file: ");
            return NULL;
        }
        file = malloc(sizeof(My_File));   /*add malloc check*/
        file->fd = fd; 
        file->flag = flag;
        file->offset = lseek(fd, 0, SEEK_CUR);
        printf("File has been opened successfully in READ MODE\n");
        return file;
    }
    else if(flag == W)
    {
        if(access(path, F_OK) == 0)
        {
            fd = open(path, O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd == -1)
            {
                perror("Error opening file");
                return NULL;
            }
            file = malloc(sizeof(My_File));   /*add malloc check*/
            file->fd = fd; 
            file->flag = flag;
            file->offset = lseek(fd, 0, SEEK_CUR);
            printf("File has been opened successfully in WRITING MODE\n");
            return file; 
        }
        else
        {
            /* when file does not exist */
            fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd == -1)
            {
                perror("Error opening file");
                return NULL;
            }
            file = malloc(sizeof(My_File));   /*add malloc check*/
            file->fd = fd; 
            file->flag = flag;
            file->offset = lseek(fd, 0, SEEK_CUR);
            printf("File has been created and opened successfully in WRITING MODE\n");
            return file;
        }
    }
    else if(flag == A)
    {
        if(access(path, F_OK) == 0)
        {
            fd = open(path, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd == -1)
            {
                perror("Error opening file");
                return NULL;
            }
            file = malloc(sizeof(My_File));   /*add malloc check*/
            file->fd = fd; 
            file->flag = flag;
            file->offset = lseek(fd, 0, SEEK_CUR);
            printf("File has been opened successfully in APPEND MODE\n");
            return file; 
        }
        else
        {
            /* when file does not exist */
            fd = open(path, O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd == -1)
            {
                perror("Error opening file");
                return NULL;
            }
            file = malloc(sizeof(My_File));   /*add malloc check*/
            file->fd = fd; 
            file->flag = flag;
            file->offset = lseek(fd, 0, SEEK_CUR);
            printf("File has been created and opened successfully in APPEND MODE\n");
            return file;
        }
    }
    else 
    { 
        errno = EINVAL; // 22 
        perror("Error occured: ");
        errno = 0;
        return NULL;
    }
}






















int close_file(My_File *f)
{
    if(f == NULL)
    {
        write(STDERR_FILENO, "File object is NULL\n", strlen("File object is NULL\n"));
        return -1;
    }
    if(close(f->fd) == -1)
    {
        perror("Error closing file");
        free(f);
        return -1;
    }
    free(f);
    return 0; 
}

static char *init_read_buffer(My_File *file, size_t size)
{
    char *buffer = malloc(size);
    return buffer;
}

static char *init_write_buffer(My_File *file, size_t size)
{
    char *buffer = malloc(size);
    return buffer; 
}

static int loadbuffer(My_File *file, int buffer_type)
{
    size_t bytes_read; 

    if(buffer_type == R)
    {
        bytes_read = read(file->fd, file->reading, BUFFER_SIZE);
        if(bytes_read == -1)
        {
            perror(" 2.) Error occurred with loading buffer\n");
            return -1; 
        }
        printf("loadedBufferReading\n");
    }
    else if(buffer_type == W)
    {
        bytes_read = read(file->fd, file->writing, BUFFER_SIZE);
        if(bytes_read == -1)
        {
            perror("1.) Error occurrd with loading buffer\n");
            return -1;
        }
        printf("loadedBufferWriting\n");
    }
    else if(buffer_type == A)
    {
        if((bytes_read = read(file->fd, file->reading, BUFFER_SIZE)) == -1)
        {
            perror("Error has occurred with loading buffer\n");
            return -1;
        }
        if((bytes_read = read(file->fd, file->writing, BUFFER_SIZE)) == -1)
        {
            perror("Error has occurred with loading buffer\n");
            return -1;
        }
        printf("bufferedWritingAndReadingLoaded\n");
    }
    else
    {
        errno = EINVAL;  /* 22 */
        perror("Error has occurred in buffer loader");
        errno = 0;
        return -1;
    }
    return 0; 
}

int fgetst(My_File *file, char *dest, size_t size)
{
    if(!file)
    {
        return -1;
    }
    loadbuffer(file, file->flag);
}



// currently working on buffer loading, specifically, bytes read variable to correctly check errors, then i will 
// need to countinue my fgets function