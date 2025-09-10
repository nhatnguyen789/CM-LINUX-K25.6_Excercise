#!/bin/bash

# Send all tasks to the background (&) so they are executed almost at the same time
./task_dispatcher 10 "Update weekly report" &
./task_dispatcher 30 "CRITICAL: Restart web server" &
./task_dispatcher 20 "Backup user database" &

echo "3 tasks sent."