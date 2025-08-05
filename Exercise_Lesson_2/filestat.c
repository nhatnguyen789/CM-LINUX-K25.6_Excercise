#include<stdio.h>
#include<stdlib.h>  // need for exit() function
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<time.h> // need for ctime() function
#include<string.h> //need for strcspn() function to handle time series

int main(int argc, char *argv[])
{
    //1. Check number of parameters
    // The requirement is "exactly one parameter", meaning argc should be 2
    // - argv[0]: ./filestat
    // - argv[1]: <file_path>
    if(argc != 2)
    {
        // 2. If error, print error message and exit
        // Printing error messages to "Standard Error" is a good practice
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        //  Exit the program with an error code (other than 0) to signal failure
        exit(1);
    }
    // 3. If true, get the path and use
    // if the program run to here, it mean argc == 2 
    // Get the file path from the second parameter
    char *file_path = argv[1];
    
    //4. Declare struct variable to store information about the file 
    struct stat file_info;

    // 5. call lstat() and check for errors
    // Use if to check the return value of lstat 
    // if it is -1, it means an error occurred 
    if(lstat(file_path, &file_info) == -1)
    {
        // perror() prints the string passed to it followed by 
        // the specific error message from the system
        perror("lstat");
        exit(1);
    }

    // --------EXTRACT AND PRINT FILE INFORMATION--------
    //1. ---Print file path---
    printf("File path: %s\n", file_path);

    // 2. ---Identify and print file type---
    printf("File type:");
    if(S_ISREG(file_info.st_mode))
    {
        printf("Regular File\n");
    }
    else if(S_ISDIR(file_info.st_mode))
    {
        printf("Directory\n");
    }
    else if(S_ISLNK(file_info.st_mode))
    {
        printf("Symbolic Link\n");
    }
    else
    {
        printf("Other\n");
    }

    // 3. ---Print file size in bytes---
    printf("File size: %ld bytes\n", file_info.st_size);

    // --- 4. Print last modification time ---
    // Get the original time series 
    char *original_time_str = ctime(&file_info.st_mtime);

    // Create a buffer to hold the formatted time string 
    char time_buffer[30];
    strncpy(time_buffer, original_time_str, sizeof(time_buffer));

    // Ensure the time string is null-terminated
    time_buffer[sizeof(time_buffer) - 1] = '\0';

    // Let's remove the newline characrter from the end of the time string
    time_buffer[strcspn(time_buffer, "\n")] = '\0';
    
    printf("Last modification time: %s\n", time_buffer);
    //------------------------------------------
    return 0;
}