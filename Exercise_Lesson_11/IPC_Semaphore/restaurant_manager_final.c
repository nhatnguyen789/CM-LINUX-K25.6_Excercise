/*---STAGE FINAL: COMPLETE PROGRAM---
- The "Manager" (parent process) will hire workers (forks), 
then step back into the supervisor role.
- The "Workers" (child processes) will automatically work, 
interacting with the "food tray" (shared memory) safely.

Steps: 
1. Fork() Loop - "Hiring Employees"
2. Producer and Consumer Logic
3. Parent Process's Supervisory Role
4. Safe Cleaning with Signals
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/mman.h>
#include<semaphore.h>
#include<fcntl.h>
#include<time.h>
#include<signal.h>
#include<sys/wait.h>

// --- Define constants and data structures ---
#define TRAY_SIZE 10
#define NUM_CHEFS 2
#define NUM_CUSTOMERS 3

// "All-in-One" data structure that holds everything that needs to be shared
typedef struct {
    int non_vegan_tray[TRAY_SIZE];
    int vegan_tray[TRAY_SIZE];
    int non_vegan_in, non_vegan_out;
    int vegan_in, vegan_out;
    int non_vegan_count;
    int vegan_count;
    sem_t nv_mutex, nv_empty, nv_full;
    sem_t v_mutex, v_empty, v_full;
} RestaurantSharedSpace;

// Global array to store PIDs of child processes, so that the parent can manage them
pid_t child_pids[NUM_CHEFS + NUM_CUSTOMERS];

// --- The "Employees" Working Logic ---

// Chef Donatello (non-vegetarian)
void chef_donatello_work(RestaurantSharedSpace *shared_space)
{
    const char* dishes[] = {"Fettuccine Chicken Alfredo", "Garlic Sirloin Steak"};
    srand(getpid()); // Initialize random seed for this process
    while(1)
    {
        // Waiting for an empty seat on the non-vegetarian tray
        sem_wait(&shared_space->nv_empty);
        // Lock the tray to order
        sem_wait(&shared_space->nv_mutex);

        // --- Critical Section: Ordering ---
        shared_space->non_vegan_tray[shared_space->non_vegan_in] = 2; //2 = non-vegetarian
        const char* dish_name = dishes[rand() %2];
        printf("[Chef Donatello] Dish '%s' is done cooking. Place on a non-vegetarian plate.\n", dish_name);
        shared_space->non_vegan_in = (shared_space->non_vegan_in + 1) % TRAY_SIZE;
        shared_space->non_vegan_count++;
        // ---------------------------------

        // Unlock the wheel
        sem_post(&shared_space->nv_mutex);
        // Signals that there is an additional dish
        sem_post(&shared_space->nv_full);

        sleep(1 + rand() % 5);  // Cook next dish after 1-5 seconds

    }
}

// Chef Portecelli (vegetarian)
void chef_portecelli_work(RestaurantSharedSpace *shared_space)
{
    const char* dishes[] = {"Pistachio Pesto Pasta", "Avocado Fruit salad"};
    srand(getpid());
    while(1)
    {
        sem_wait(&shared_space->v_empty);
        sem_wait(&shared_space->v_mutex);
        shared_space->vegan_tray[shared_space->vegan_in] = 1; // 1 = run
        const char* dish_name = dishes[rand() % 2];
        printf("[Chef Portecelli] Dish '%s' is done cooking. Place on vegetarian tray.\n", dish_name);
        shared_space->vegan_in = (shared_space->vegan_in + 1) % TRAY_SIZE;
        shared_space->vegan_count++;
        sem_post(&shared_space->v_mutex);
        sem_post(&shared_space->v_full);
        sleep(1 + rand() % 5);
    }
}

// Customer 1 (only eats non-vegetarian food)
void customer_1_worker(RestaurantSharedSpace *shared_space)
{
    srand(getpid());
    while(1)
    {
        printf("[Customer 1] Waiting for NON-VEGETARIAN dish...\n");
        // Waiting for non-vegetarian dishes
        sem_wait(&shared_space->nv_full);
        // Lock the tray to get the dish
        sem_wait(&shared_space->nv_mutex);

        // --- Critical Section: Get Item ---
        shared_space->non_vegan_tray[shared_space->non_vegan_out] = 0; // 0 = empty
        printf("[Customer 1] Ordered a NON-VEGGIE item. Start eating.\n");
        shared_space->non_vegan_out = (shared_space->non_vegan_out + 1) & TRAY_SIZE;
        shared_space->non_vegan_count--;
        // ---------------------------------

        // Unlock the wheel
        sem_post(&shared_space->nv_mutex);
        // Signals that there is an additional space
        sem_post(&shared_space->nv_empty);

        sleep(10 + rand() % 6); // Eat in 10-15 seconds
    }
}

// Customer 2 (vegetarian only)
void customer_2_worker(RestaurantSharedSpace *shared_space)
{
    srand(getpid());
    while(1)
    {
        printf("[Customer 2] waiting for VEGETARIAN dish...\n");
        sem_wait(&shared_space->v_full);
        sem_wait(&shared_space->v_mutex);
        shared_space->vegan_tray[shared_space->vegan_out] = 0;
        printf("[Customer 2] Took a VEGAN dish. Started eating.\n");
        shared_space->vegan_out = (shared_space->vegan_out + 1) % TRAY_SIZE;
        shared_space->vegan_count--;
        sem_post(&shared_space->v_mutex);
        sem_post(&shared_space->v_empty);
        sleep(10 + rand() % 6);
    }
}

// Customer 3 (eats both)
void customer_3_worker(RestaurantSharedSpace *shared_space)
{
    srand(getpid());
    while(1)
    {
        // Get vegetarian food
        printf("[Customer 3] Waiting for VEGETTARIAN dish...\n");
        sem_wait(&shared_space->v_full);
        sem_wait(&shared_space->v_mutex);
        shared_space->vegan_tray[shared_space->vegan_out] = 0;
        printf("[Customer 3] Ordered a VEGETARIAN dish. Started eating.\n");
        shared_space->vegan_out = (shared_space->vegan_out + 1) % TRAY_SIZE;
        shared_space->vegan_count--;
        sem_post(&shared_space->v_mutex);
        sem_post(&shared_space->v_empty);

        // Get non-vegetarian dishes
        printf("[Customer 3] Waiting for NON-VEGETARIAN dishes...\n");
        sem_wait(&shared_space->nv_full);
        sem_wait(&shared_space->nv_mutex);
        shared_space->non_vegan_tray[shared_space->non_vegan_out] = 0;
        printf("[Customer 3] Ordered a NON-VEGGIE item. Start eating.\n");
        shared_space->non_vegan_out = (shared_space->non_vegan_out  + 1) % TRAY_SIZE;
        shared_space->non_vegan_count--;
        sem_post(&shared_space->nv_mutex);
        sem_post(&shared_space->nv_empty);

        sleep(10 + rand() % 6);
    }
}

// Main cleanup function for Manager
void cleanup(RestaurantSharedSpace *shared_space)
{
    printf("\nManagement ordered the closure...\n");

    // Send stop signal to all child processes
    for(int i = 0; i < NUM_CHEFS + NUM_CUSTOMERS; i++)
    {
        if(child_pids[i] > 0)
        {
            kill(child_pids[i], SIGKILL);
        }
    }

    // Wait for all the children to actually finish
    for(int i = 0; i < NUM_CHEFS + NUM_CUSTOMERS; i++)
    {
        wait(NULL);
    }

    // Cancel semaphores
    sem_destroy(&shared_space->nv_mutex);
    sem_destroy(&shared_space->nv_empty);
    sem_destroy(&shared_space->nv_full);
    sem_destroy(&shared_space->v_mutex);
    sem_destroy(&shared_space->v_empty);
    sem_destroy(&shared_space->v_full);

    // Destroy shared memory
    munmap(shared_space, sizeof(RestaurantSharedSpace));
    printf("The restaurant has been cleaned.\n");
}

int main()
{
    // Create shared memory
    RestaurantSharedSpace *shared_space = mmap(NULL, sizeof(RestaurantSharedSpace),
                                               PROT_READ | PROT_WRITE,
                                               MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_space == MAP_FAILED) {
        perror("mmap"); exit(1);
    }
    
    // Initialize the initial state
    memset(shared_space, 0, sizeof(RestaurantSharedSpace));
    
    // Initialize semaphores
    sem_init(&shared_space->nv_mutex, 1, 1);
    sem_init(&shared_space->nv_empty, 1, TRAY_SIZE);
    sem_init(&shared_space->nv_full, 1, 0);
    sem_init(&shared_space->v_mutex, 1, 1);
    sem_init(&shared_space->v_empty, 1, TRAY_SIZE);
    sem_init(&shared_space->v_full, 1, 0);

    // --- Hire workers (fork child processes) ---
    for(int i = 0; i < NUM_CHEFS + NUM_CUSTOMERS; i++)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            perror("fork");
            exit(1);
        }
        else if(pid == 0)
        {
            // This is the code of CHILD PROCESS
            // Based on 'i' to decide the role
            if(i == 0) chef_donatello_work(shared_space);
            else if(i == 1) chef_portecelli_work(shared_space);
            else if(i == 2) customer_1_worker(shared_space);
            else if(i == 3) customer_2_worker(shared_space);
            else if(i == 4) customer_3_worker(shared_space);
            exit(0);
        }
        else
        {
            // This is the code of the PARENT PROCESS
            child_pids[i] = pid; // Save the child's PID
        }
    }

    // --- Supervisory role of Manager ---
    printf("The restaurant is open! (Press Ctrl+C to close)\n");
    signal(SIGINT, (void(*) (int)) cleanup); // Register a cleanup function when pressing Ctrl+C

    while(1)
    {
        sleep(10);
        // Lock mutex to safely read counter variable
        // (Even though it's read only, chefs/customers may be writing in the middle)
        sem_wait(&shared_space->nv_mutex);
        sem_wait(&shared_space->v_mutex);

        printf("\n--- REPORT FROM MANAGER ---\n");
        printf("Non-vegan dishes on the tray: %d/%d\n", shared_space->non_vegan_count, TRAY_SIZE);
        printf("Vegetarian dishes on the tray: %d/%d\n", shared_space->vegan_count, TRAY_SIZE);
        printf("---------------------------\n\n");

        sem_post(&shared_space->v_mutex);
        sem_post(&shared_space->nv_mutex);
    }
    return 0;
}