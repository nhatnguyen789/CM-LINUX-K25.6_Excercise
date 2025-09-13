#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>      /* For O_* constants */
#include<sys/stat.h>   /* For mode constants */
#include<sys/mman.h>  /* For mmap, munmap */

// --- Define constants and data structures ---
// Name of shared memory object. Must be identical to producer.
#define SHM_NAME "/my_shared_mem"

// The data structure must be identical to the producer to read correctly.
typedef struct
{
    int id;
    char name[20];
    double price;
}Product;

int main(int argc, char *argv[])
{
    int shm_fd; // File descriptor for shared memory object
    Product *shm_ptr; // Pointer to mapped memory 

    // --- Step 1: Open an existing shared memory object ---
    // shm_open is called with the O_RDONLY (read-only) flag.
    // Consumer does not use the O_CREAT flag because it assumes the producer has already created the object.
    shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if(shm_fd == -1)
    {
        perror("shm_open");
        fprintf(stderr, "Error: Make sure producer is running first.\n");
        exit(EXIT_FAILURE);
    }

    // --- Step 2: Map the memory object into the process's address space ---
    // Consumer does not need to call ftruncate() because the size has been set by the producer.
    // mmap() is called with PROT_READ (read-only) permission.
    shm_ptr = mmap(NULL, sizeof(Product), PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // --- Step 3: Read data from shared memory ---
    // Read data from pointer as a normal struct and print to screen.
    printf("Consumer: Read data from shared memory:\n");
    printf("Product ID: %d\n", shm_ptr->id);
    printf("Product Name: %s\n", shm_ptr->name);
    printf("Product Price: %.2f\n", shm_ptr->price);

    // --- Step 4: Cleanup ---
    // Unmap the memory area from the process
    if(munmap(shm_ptr, sizeof(Product)) == -1)
    {
        perror("munmap");
    }

    // Close file descriptor
    if(close(shm_fd) == -1)
    {
        perror("close");
    }

    // --- Step 5: Remove shared memory object from system ---
    // Since consumer is the end user, it is responsible for cleaning up the object.
    // Without this step, the object will remain forever in /dev/shm until reboot.
    if(shm_unlink(SHM_NAME) == -1)
    {
        perror("shm_unlink");
    }

    printf("Consumer: Cleanup completed. Exiting.\n");

    return 0;
}