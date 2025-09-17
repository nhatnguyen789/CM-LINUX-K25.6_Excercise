/*---STAGE 1: DESIGNING THE "KITCHEN"---
- Source code for the setup phase:
    - Create shared memory
    - Initialize the "food trays" and managed variables
    - And then clean up
=> At this stage, we have not created a child process yet.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/mman.h>    // Main library for mmap (shared memory)  
#include<semaphore.h>   // Library for semaphores (will be used in later stages)
#include<fcntl.h>   /* For O_* constants */

// --- Define constants and data structures ---
#define TRAY_SIZE 10    // Size of each food tray

// "All-in-One" data structure that holds everything that needs to be shared. 
// Putting everything into one struct makes creating and sharing memory extremely simple.
typedef struct 
{
    // 1. Two food trays (limited buffer) [cite: 272, 273]  
    // Each cell is an int: 0=empty, 1=vegetarian, 2=non-vegetarian [cite: 274, 275, 276, 277, 278]
    int non_vegan_tray[TRAY_SIZE];
    int vegan_tray[TRAY_SIZE];

    // 2. Indexes for managing the bounded buffer 
    int non_vegan_in;
    int non_vegan_out;
    int vegan_int;
    int vegan_out;

    // 3. Counter variables for parent process to monitor
    int non_vegan_count;
    int vegan_count;

    // 4. All 6 semaphores are also placed here [cite: 281]
    // They will be initialized and used in the next stage.
    sem_t nv_mutex, nv_empty, nv_full; // For non-vegetarian tray
    sem_t v_mutex, v_empty, v_full;    // For vegetarian tray
}RestaurantSharedSpace;

int main(int argc, char* argv[])
{
    printf("Restaurant manager starts work...\n");

    // --- Step 1: Create shared memory ---
    // Use mmap to ask the kernel to allocate a memory area.
    // NULL: Let the kernel choose the address.
    // sizeof(RestaurantSharedSpace): Size of the memory area to be allocated.
    // PROT_READ | PROT_WRITE: This memory area can be read and written.
    // MAP_SHARED | MAP_ANONYMOUS:
    // - MAP_SHARED: Changes to this memory area will be shared with child processes.
    // - MAP_ANONYMOUS: The memory area is not associated with any file, it only exists in RAM.
    RestaurantSharedSpace *shared_space = mmap(NULL, sizeof(RestaurantSharedSpace), 
                                            PROT_READ | PROT_WRITE,
                                            MAP_SHARED | MAP_ANONYMOUS,
                                            -1, 0);

    if(shared_space == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("Successfully created 'kitchen' (Shared Memory)...\n");

    // --- Step 2: Initialize the "kitchen" state ---
    // memset all memory to 0.
    // This will automatically set the tray cells to 0 (empty), the indices and counters to 0.
    memset(shared_space, 0, sizeof(RestaurantSharedSpace));

    printf("Clean and Initialized food trays (Currently empty)...\n");
    printf("Non-Vegen tray status: %d/%d\n", shared_space->non_vegan_count, TRAY_SIZE);
    printf("Vegan tray status: %d/%d\n", shared_space->vegan_count, TRAY_SIZE);


    // NOTE: In the later stages, this is where we initialize the 6 semaphores
    // using the sem_init() function
    printf("Next step: Initialize 'Kitchen rules' (Semaphores)\n");

    // NOTE: In the later stages, this is also where we will create 5 child processes
    // using loops and the fork() function
    printf("Next stage: 'Hire' employees (Create child process).\n");

    // --- Step 3: Clean up shared memory ---
    // In the complete program, this step will be performed when the program ends (e.g. when Ctrl+C is pressed).
    // Here, we do it right away for illustration.

    if(munmap(shared_space, sizeof(RestaurantSharedSpace)) == -1)
    {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    printf("Administrator closed, 'kitchen' has been closed.\n");

    return 0;
}