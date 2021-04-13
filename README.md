# ITCS343
Principles of Operating Systems
------
# I use ActualProject001 
---
## Project001 details
I modify code from the lab5. Don't copy hahah. <br>
some line might not make sense. some line I don't need it but I don't want to delete it. That's Baby's style.<br>
I use only 2 threads.<br>
 1. First thread, put items into buffer until buffer is full then thread go sleep.
 2. Second thread, remove items from buffer until buffer is empty then thread go sleep.
I use mutex lock, semaphores, and sleep. <br>
```
pthread_mutex_t mutex;      //mutex lock
sem_t full, empty;          //semaphonres
```
*** Note: I use array size 31 because rand() function don't random number 0 for me so I add an empty string to skip index 0.<br>
```
char q_wrestler[31][20] = {"", "Stone Cold", "The Rock", "Triple H", "Shawn Michaels",
                           "Bret Hart", "The Undertaker", "John Cena", "Randy Orton",
                           "Edge", "Batista", "Brock Lesnar", "Kurt Angle", "Hulk Hogan", "Ric Flair",
                           "Yokozuna", "CM Punk", "Goldberg", "Sting", "diesel", "Roman Reigns",
                           "Drew McIntyre", "Bobby Lashley", "Seth Rollins", "Dean Ambrose",
                           "Chris Jericho", "Sheamus", "Eddie Guerrero", "AJ Styles",
                           "Big Show", "Kane"};
```
----
