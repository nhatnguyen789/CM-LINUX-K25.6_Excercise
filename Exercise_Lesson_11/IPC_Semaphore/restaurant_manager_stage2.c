/*---STAGE 2: SET UP A "KITCHEN RULE" (SEMAPHORE)---
- Phase 2 focuses on setting up the working "rules" by initializing Semaphores.
- The "Manager" (parent process) will set these rules before hiring any staff.
- These rules will ensure that future chefs and customers work in an orderly manner and do not cause chaos.
*/

// restaurant_manager_p2.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>   // Main library for mmap (Shared Memory)
#include <semaphore.h>  // Main Library for Semaphores
#include <fcntl.h>      /* For O_* constants */

// --- Define constants and data structures ---

#define TRAY_SIZE 10 //Size of each food tray

// "All-in-One" data structure that holds everything that needs to be shared
typedef struct {
    // 1. Two food trays
    int non_vegan_tray[TRAY_SIZE];
    int vegan_tray[TRAY_SIZE];

    // 2. Management indicators
    int non_vegan_in, non_vegan_out;
    int vegan_in, vegan_out;
    
    // 3. Counter variables
    int non_vegan_count;
    int vegan_count;

    // 4. All 6 semaphores
    sem_t nv_mutex, nv_empty, nv_full; // For non-vegetarian tray
    sem_t v_mutex, v_empty, v_full;   // For vegetarian tray
} RestaurantSharedSpace;

int main() {
    printf("Restaurant manager starts work...\n");

    // --- Step 1: Create shared memory (like Phase 1) ---
    RestaurantSharedSpace *shared_space = mmap(NULL, sizeof(RestaurantSharedSpace),
                                               PROT_READ | PROT_WRITE,
                                               MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_space == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    printf("Successfully created 'kitchen' (shared memory area).\n");

    // --- Step 2: Initialize the initial state for the "kitchen" (like Phase 1) ---
    memset(shared_space, 0, sizeof(RestaurantSharedSpace));
    printf("Initialized food trays to empty state.\n");

    // --- Step 3: Initialize the "kitchen rules" (Semaphores) ---
    printf("Setting up rules (Semaphore)...\n");

    // Initialize semaphores for the NON-RUNNING tray
    // sem_init(pointer_to_semaphore, pshared, initial_value)
    // pshared = 1: semaphore is shared between PROCESSES
    // pshared = 0: semaphore is shared between THREADS (same process)
    sem_init(&shared_space->nv_mutex, 1, 1);    //Mutex: Initialized to 1 (open)
    sem_init(&shared_space->nv_empty, 1, TRAY_SIZE); //Empty: Initialized with 10 (10 blank spaces)
    sem_init(&shared_space->nv_full, 1, 0); //Full: Initialize to 0 (no items yet)

    // Initialize semaphores for the CHAY tray
    sem_init(&shared_space->v_mutex, 1, 1);
    sem_init(&shared_space->v_empty, 1, TRAY_SIZE);
    sem_init(&shared_space->v_full, 1, 0);

    printf("Rules set up done.\n");

    // NOTE: In the later stages, this is where we will create 5 child processes
    printf("Next stage: 'Hire' workers (Create child processes).\n");

    // Simulate restaurant operating hours
    printf("Restaurant is open (press Enter to close)...\n");
    getchar(); //Wait for the user to press Enter

    // --- Step 4: Clean up Semaphores and Shared Memory ---
    printf("Management ordered the closure...\n");

    // Destroy semaphores before destroying the memory containing them
    sem_destroy(&shared_space->nv_mutex);
    sem_destroy(&shared_space->nv_empty);
    sem_destroy(&shared_space->nv_full);
    sem_destroy(&shared_space->v_mutex);
    sem_destroy(&shared_space->v_empty);
    sem_destroy(&shared_space->v_full);
    printf("Semaphore cancalled.\n");

    // Destroy shared memory
    if(munmap(shared_space, sizeof(RestaurantSharedSpace)) == -1)
    {
        perror("munmap");
    }
    printf("'Kitchen' has been cleanned.\n");
    return 0;
}