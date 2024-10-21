#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "bufferIO.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>


My_File *open_file(const char *path, int flag)
{
    int fd; 
    My_File *file = malloc(sizeof(My_File));
    if(flag == W)
    {
        // Check if file exists and WRITE MODE
        if(access(path, F_OK) == 0) 
        {
            fd = open(path, O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == -1;
            if(fd == -1)
            {
                perror("Error opening file!");
                free(file);
                return NULL;
            }
            else if( fd != -1) 
            { 
                file->fd = fd;
                file->offset = lseek(fd, 0, SEEK_CUR); 
                file->flag = flag; 
                printf("File has been opened successfully in WRITE MODE\n");   
                return file;
            }
        }
        else
        {
            fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == -1;
            if(fd == -1)
            {
                perror("Error creating file!");
                free(file);
                return NULL;
            }
            else 
            { 
                file->fd = fd;
                file->offset = lseek(fd, 0, SEEK_CUR);
                file->flag = flag; 
                printf("File has been created successfully in WRITE_MODE\n"); 
                return file; 
            }
        }
    }
    else if(flag == R)
    {
        fd = open(path, O_RDONLY);
        if(fd == -1)
        {
            perror("File does not exist");
            free(file);
            return NULL;
        } 
        else
        { 
            file->fd = fd; 
            file->offset = lseek(fd, 0, SEEK_CUR);
            file->flag = flag; 
            printf("File has been opened successfully\n");
            return file; 
        }
    }
    else if(flag == A)
    {
        if(access(path, F_OK) == 0) 
        {
            fd = open(path, O_APPEND | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd == -1)
            {
                perror("Error opening file file!");
                free(file);
                return NULL;
            }
            else
            {    
                file->fd = fd;
                file->flag = flag; 
                file->offset = lseek(fd, 0, SEEK_END);
                printf("File has been opened successfully in APPEND MODE\n");
                return file;  
            }
        }
        else
        {
            fd = open(path, O_APPEND | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd == -1)
            {
                perror("Error creating file!");
                free(file);
                return NULL;
            }
            else 
            { 
                file->fd = fd;
                file->offset = lseek(fd, 0, SEEK_END);
                file->flag = flag;
                printf("File has been created successfully in APPEND MODE\n"); 
                return file; 
            }
        }
    }
    else 
    { 
        errno = EINVAL; // 22 
        perror("Error occured: ");
        errno = 0;
    }
}


