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
    close_status = close(f->fd);
    if(close_status == -1)
    {
        perror("Error closing file descriptor");
        free(f);
        return -1;
    }
    free(f);
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
    printf("Buffer Init Successful\n");
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

    file->reading[bytes] = '\0';
    
    printf("loaded reading buffer\n");
    return 0;
}

/*
int fgetst(My_File *file, char *dest, size_t size)
{

    /* Handle issue when user tries to read string in write only mode 

    if(!file)
    {
        perror("File does not exist");
        return -1;
    }

    unsigned int current_line_char_number = 0;

    unsigned int spot = file->reading_buffer_offset;

    char current_char = ' ';  

    while(current_char != '\0' && current_char != '\n' && current_line_char_number < size-1)
    {
        current_char = file->reading[spot];
        //printf("I am here\n");
        dest[current_line_char_number] = current_char; 
        spot++;
        current_line_char_number++;
        //printf("%c\n", current_char);
        if(spot >= BUFFER_SIZE-1)
        {
            reset_buffer(file);
            load_reading_buffer(file);  /* Error handling later 
            spot = 0;
            file->reading_buffer_offset = spot;
        }
        current_char = file->reading[spot];
    }    
    file->reading_buffer_offset = current_line_char_number+1; 
    dest[current_line_char_number] = '\0';
    return 0;
}

*/

int fgetst(My_File *file, char *dest, size_t size)
{
    if(!file)
    {
        perror("Error opening file: file does not exist");
        return -1;
    }

    if(file->flag == W)
    {
        perror("You dont have permission to read 'write mode' file");
        return -2; 
    }
    
    unsigned int spot = file->reading_buffer_offset; 
    unsigned int current_line_char_number = 0; 

    char current_char = file->reading[spot]; 

    dest[0] = '\0';

    while(current_char != '\0' && current_char != '\n' && current_line_char_number < size-1)
    {
        dest[current_line_char_number] = current_char;
        spot++;
        current_line_char_number++;

        if(spot >= BUFFER_SIZE-1)
        {
            reset_buffer(file);
            load_reading_buffer(file);  /* Error handling malloc later */
            spot = 0;
        }
        current_char = file->reading[spot];
    }
    file->reading_buffer_offset = spot+1; 
    dest[current_line_char_number] = '\0';
    return 0;
}







static int reset_buffer(My_File *file)
{
    memset(file->reading, 0, BUFFER_SIZE);
}

/*


    

*/