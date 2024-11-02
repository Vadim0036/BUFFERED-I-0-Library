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
            perror("Error occured reading file");
            return NULL;
        }
        printf("File has been opened successfully in READ MODE\n");
    }
    else if(flag == W)
    {
        if(access(path, F_OK) == 0)
        {
            /* when file already exists */
            fd = open(path, O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd == -1)
            {
                perror("Error opening file");
                return NULL;
            }
            printf("File has been opened successfully in WRITING MODE\n");
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
            printf("File has been created and opened successfully in WRITING MODE\n");
        }
    }
    else if(flag == A)
    {
        if(access(path, F_OK) == 0)
        {
            /* when file already exists */
            fd = open(path, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd == -1)
            {
                perror("Error opening file");
                return NULL;
            }
            printf("File has been opened successfully in APPEND MODE\n");  
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
            printf("File has been created and opened successfully in APPEND MODE\n");
        }
    }
    else 
    { 
        errno = EINVAL; // 22 
        perror("Error occured: ");
        errno = 0;
        return NULL;
    }
    /* Create file structure  */
    file = malloc(sizeof(My_File));
    if(!file)
    {
        close(fd);
        perror("Memory allocation failed");
        return NULL;
    }
    file->fd = fd; 
    file->flag = flag;
    file->offset = lseek(fd, 0, SEEK_CUR);
    if(file->offset == -1)
    {
        perror("Error seeking file");
        close(fd);
        free(file);
        return NULL;
    }
    init_buffer(file, BUFFER_SIZE);
    return file;
}

int close_file(My_File *f)
{
    int close_status; 
    if(f == NULL)
    {
        perror("File object is NULL");
        return -1;
    }
    flush_writing_buffer(f);
    close_status = close(f->fd);
    if(close_status == -1)
    {
        perror("Error closing file descriptor");
        free(f);
        return -1;
    }
    free(f);

    /* Free all buffers */
    return 0; 
}

static int init_buffer(My_File *file, size_t size)
{
    if(file->flag == R)
    {
        file->reading = malloc(size);   /* check malloc error later */
        load_reading_buffer(file);
        file->reading_buffer_offset = 0;
    }
    else if(file->flag == W)
    {
        file->writing = malloc(size); /* check malloc error later */
        file->writing_buffer_offset = 0;
    }
    else if(file->flag == A)
    {
        file->writing = malloc(size); /*  I will check this error later */
        file->reading = malloc(size);
        file->reading_buffer_offset = 0;
        file->writing_buffer_offset = 0;
        load_reading_buffer(file);
    }
    else
    {
        perror("Error occured while initializing buffer");
        return -1; 
    }
    return 0;
}

static int load_reading_buffer(My_File *file)
{
    int bytes = read(file->fd, file->reading, BUFFER_SIZE-1);

    if(bytes == -1)
    {
        perror("Error with reading file (lrb)");
        return -1; 
    }
    char c;

    file->reading[bytes] = '\0';
    return 0;
}

static int flush_writing_buffer(My_File *file)
{
    if(!file)
    {
        perror("File could not be opened.");
        return -1; 
    }
    int bytes = write(file->fd, file->writing, file->writing_buffer_offset);
    if(bytes == -1)
    {
        perror("Error flushing the writing buffer.");
        return -2;
    }
    file->writing_buffer_offset = 0;
    return 0;
}

/* Optimize this function later*/
int fgetst(My_File *file, char *dest, size_t length)
{
    if(!file)
    {
        perror("Error opening file\n");
        return -1;
    }
    if(file->flag == W)
    {
        perror("This file is opened in WRITE ONLY Mode\n");
        return -2;
    }
    unsigned int line_offset = 0; 
    char current_char = file->reading[file->reading_buffer_offset];
    if(current_char == '\0')
    {
        if(file->reading_buffer_offset >= BUFFER_SIZE-1)
        {
            memset(file->reading, 0, BUFFER_SIZE);
            load_reading_buffer(file);
            file->reading_buffer_offset = 0;
        }
        else { return 1; }
    }
    for(; file->reading_buffer_offset < BUFFER_SIZE; file->reading_buffer_offset++)
    {   
        if(file->reading_buffer_offset >= BUFFER_SIZE-1)
        {
            memset(file->reading, 0, BUFFER_SIZE);
            load_reading_buffer(file);
            file->reading_buffer_offset = -1;
            //printf("___%c____\n", file->reading[0]);
            continue;
        }
        if(line_offset >= length-1)
        {
            dest[line_offset] = '\0';
            return 0; 
        }  
        current_char = file->reading[file->reading_buffer_offset]; 
        //printf("Ascii: %d buffer_offset: %d line_offset: %d\n", current_char, file->reading_buffer_offset, line_offset);

        if(current_char == '\n')
        {
            dest[line_offset] = '\n';
            dest[line_offset+1] = '\0'; 
            file->reading_buffer_offset++;
            return 0;
        }
        if (current_char == '\0')
        {
            dest[line_offset] = '\0';
            return 0;
        }
        dest[line_offset] = current_char;
        line_offset++;
    }
    dest[line_offset] = '\0';
    return 0;
}


int fgetch(My_File *file)
{
    //printf("I am here!\n");
    if(!file)
    {
        perror("File couldn't be opened.");
        return EOF;
    }
    else if(file->flag == W)
    {
        perror("File is opened in WRITING ONLY Mode");
        return EOF;
    }
    if (file->reading_buffer_offset >= BUFFER_SIZE-1)
    {
        memset(file->reading, 0, BUFFER_SIZE); 
        load_reading_buffer(file);
        file->reading_buffer_offset = 0;
        if(file->reading[0] == '\0') { return EOF; }
    }
    int current_char; 
    current_char = file->reading[file->reading_buffer_offset]; 
    if(current_char == '\0'){ return EOF; }
    file->reading_buffer_offset++;
    return current_char;
}

int fputst(My_File *file, const char *data, size_t size)
{
    if(!file)
    {
        perror("The given file does not exist.");
        return -1;
    }
    if(file->flag == R)
    {
        perror("File is opened in READ ONLY mode.");
        return -2;
    }
    int total_chars_written = 0;
    int remaining_space = BUFFER_SIZE - file->writing_buffer_offset; 
    if(size <= remaining_space)
    {
       
        for(int i = 0; i < size; i++)
        {
            file->writing[file->writing_buffer_offset] = data[i];
            file->writing_buffer_offset++;
            total_chars_written++;
        }
    }
    else
    {
        if(size < BUFFER_SIZE)
        {
            printf("I am in size < buffer_size\n");
            flush_writing_buffer(file);
            memset(file->writing, 0, BUFFER_SIZE);
            for(int i = 0; i < size; i++)
            {
                file->writing[file->writing_buffer_offset] = data[i];
                file->writing_buffer_offset++;
                total_chars_written++;
            }
        }
        else
        {
            printf("I am in else; partially data write\n");
            // write data partially
            for(int i = 0; i < size; i++)
            {
                if(file->writing_buffer_offset >= BUFFER_SIZE)
                {
                    flush_writing_buffer(file);
                    memset(file->writing, 0, BUFFER_SIZE);
                }
                file->writing[file->writing_buffer_offset] = data[i];
                file->writing_buffer_offset++;
                total_chars_written++;
            }
        }
    }
    if(file->writing_buffer_offset >= BUFFER_SIZE)
    {
        flush_writing_buffer(file);
        memset(file->writing, 0, BUFFER_SIZE);
    }
    return total_chars_written;
}

int fputch(My_File *file, int c)
{
    if(!file)
    {
        perror("File could not be opened");
        return -1;
    }
    if(file->flag == R)
    {
        perror("File was opened in READ ONLY mode.");
        return -2;
    }
    int remaining_space = BUFFER_SIZE - file->writing_buffer_offset; 
    if(remaining_space <= 0)
    {
        printf("hi\n");
        flush_writing_buffer(file);
        memset(file->writing, 0, BUFFER_SIZE);
    }
    file->writing[file->writing_buffer_offset] = c;
    file->writing_buffer_offset++;
    return 0;
}