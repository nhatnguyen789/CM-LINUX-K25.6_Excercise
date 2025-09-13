#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>       /* For O_* constants */
#include<sys/stat.h>    /* For mode constants */
#include<sys/mman.h>    /* For mmap, munmap */

// --- Define constants and data structures ---
// Name of the shared memory object. It must start with a '/'
#define SHM_NAME "/my_shared_mem"

// Data structure to share. 
// Both producer and consumer must use this definition.
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

    // --- Step 1: Create or open a shared memory object ---
    // shm_open works similarly to the open() function for files.
    // O_CREAT: Creates an object if it does not exist.
    // O_RDWR: Opens for reading and writing.
    // 0666: Access permissions (similar to file permissions).
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if(shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // --- Step 2: Set the size of the shared memory area ---
    // A newly created object has a size of 0.
    // ftruncate() is used to resize it.
    // We need it to be large enough to hold a Product struct.
    if(ftruncate(shm_fd, sizeof(Product)) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // --- Step 3: Map the memory object into the process's address space ---
    // mmap() will "hang" the shared memory area into the memory of this process.
    // NULL: Let the operating system choose the starting address.
    // sizeof(Product): Size of the memory area to be mapped.
    // PROT_WRITE: Request write permission on this memory area.
    // MAP_SHARED: Changes will be shared with other processes.
    // shm_fd: File descriptor of the object to be mapped.
    // 0: Starting offset (from the beginning of the object).
    shm_ptr = mmap(NULL, sizeof(Product), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // --- Step 4: Write data to shared memory ---
    // Now, shm_ptr acts exactly like a pointer to a normal struct.
    printf("Enter product ID: ");
    scanf("%d", &shm_ptr->id);

    printf("Enter product name: ");
    // Read product name (handle remaining newline characters from scanf)
    fgetc(stdin);
    fgets(shm_ptr->name, sizeof(shm_ptr->name), stdin);
    shm_ptr->name[strcspn(shm_ptr->name, "\n")] = 0; // Remove newline character

    printf("Enter product price: ");
    scanf("%lf", &shm_ptr->price);

    printf("Producer: Data written to shared memory: ID = %d, Name = %s, Price = %.2f\n", shm_ptr->id, shm_ptr->name, shm_ptr->price);
    
    // --- Step 5: Cleanup ---
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

    return 0;
}