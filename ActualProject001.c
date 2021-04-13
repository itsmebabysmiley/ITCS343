#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
typedef int buffer_item;
#define BUFFER_SIZE 5
pthread_mutex_t mutex;              //the mutex lock
sem_t full, empty;                  //the semaphores
buffer_item buffer[BUFFER_SIZE];    //the buffer
int counter;                        //buffer counter
int count_w;                        //counter of wrestler entry the ring.
int count_r;                        //counter of wrestler who was remove the ring.
int bed = 0;                        //status to sleep or awake
pthread_t tid1, tid2;               //Thread ID
pthread_attr_t attr1, attr2;        //Set of thread attributes

char q_wrestler[30][20] = { "Stone Cold", "The Rock", "Triple H", "Shawn Michaels",
                           "Bret Hart", "The Undertaker", "John Cena", "Randy Orton",
                           "Edge", "Batista", "Brock Lesnar", "Kurt Angle", "Hulk Hogan", "Ric Flair",
                           "Yokozuna", "CM Punk", "Goldberg", "Sting", "diesel", "Roman Reigns",
                           "Drew McIntyre", "Bobby Lashley", "Seth Rollins", "Dean Ambrose",
                           "Chris Jericho", "Sheamus", "Eddie Guerrero", "AJ Styles",
                           "Big Show", "Kane"};

char moveset[3][30] = {"toss", "kick", "clotheslined"};     //fancy move
int out_w[30];                                 //store wrestler who already out.
// void *ReleaseWrestler(void *pno);          //the release thread
// void *ReturnWrestler(void *cno);          //the consumer thread

/**
 * initailize all varible.
 * mutex, semaphore, thread attributes.
*/
void initializeData()
{
    pthread_mutex_init(&mutex, NULL);   //create the mutex lock
    //Create the full semaphore and initialize to 0
    sem_init(&full, 0, 0);
    //Create the empty semaphore and initialize to BUFFER_SIZE
    sem_init(&empty, 0, BUFFER_SIZE);
    pthread_attr_init(&attr1);          //get the default attributes
    pthread_attr_init(&attr2);
    counter = 0;
    count_w = 0;
    count_r = 30;
    //init buffer
    for (int i = 0; i < 5; i++)
    {
        int *x;
        x = &buffer[i];
        *x = -1;
    }
}
// Add an item to the buffer
int insert_item()
{
    /*When the ring is not full add the wrestler and increment the counter*/
    if (counter < BUFFER_SIZE)
    {
        printf("%s enters the rumble at slot %d\n", q_wrestler[count_w], counter);
        buffer[counter] = count_w;         //add wrestler in slot.
        counter++;                         //increse the counter of buffer
        count_w++;                         //increse the counter of wrestler who already entry the ring

    }
    //slot is full, set releasewrestler to sleep.
    if(counter == 5){
        bed = 1;
    }
    return 0;
}
// Remove an item from the buffer
int remove_item(buffer_item *item)
{
    /* When the ring is not empty remove the item and decrement the counter */
    if (counter > 0)
    {
        int move = rand() % 3;          //random moveset
        printf("%s is %s out of the ring from slot %d\n", q_wrestler[buffer[*item]],moveset[move],*item);
        buffer[*item] = -1;             //set slot to -1, means that slot is empty.
        counter--;                      //decrease the counter of buffer.
        count_r--;                      //decrease the counter of wrestler who was remove from the ring
    }
    //slot is empty set returnwrestler thread to sleep.
    if(counter == 0){
        bed = 0;
    }
    return 0;
}
// Producer Thread
void *ReleaseWrestler()
{
    buffer_item item;
    time_t t;
    srand((unsigned) time(&t));
    //All wrestlers already join the ring. terminate thread.
    while (count_w != 30)
    {

        if (bed == 1)                   //if ring is full, then sleep.
        {
            sleep(0.5);
        }
        else
        {
            
            sem_wait(&empty);           //start the lock
            pthread_mutex_lock(&mutex); 

            insert_item();       ///CRITICAL SECTION
            
            
            pthread_mutex_unlock(&mutex); 
            sem_post(&full);            //end the lock   
        }
    }
    pthread_exit(NULL);
}
// Consumer Thread
void *ReturnWrestler()
{
    buffer_item item;
    //Only one wrestler is on the ring and it is last round. terminate thread.
    while (count_r != 1)
    {
        if (bed == 0)               //buffer is empty sleep.
        {
            sleep(0.5);
        }
        else
        {
            
            item = rand() % 5;
            while (buffer[item] == -1)  //check wrestler is already remove from the ring.
            {
                item = rand() % 5;
            }

            sem_wait(&full);            //start the lock
            pthread_mutex_lock(&mutex); 

            remove_item(&item);   ///CRITICAL SECTION
            
            pthread_mutex_unlock(&mutex); 
            sem_post(&empty);             //end the lock
        }
    }
    pthread_exit(NULL);
}

void winner()
{
    for (int i = 0; i < 5; i++)
    {
        if (buffer[i] != -1)
        {
            printf("the winner of the rumble match is %s\n", q_wrestler[buffer[i]]);
        }
    }
}

int main(int argc, char *argv[])
{
    initializeData(); //Initialize the app
    // Create the release thread
    pthread_create(&tid1, &attr1, ReleaseWrestler, NULL);
    // Create the return thread
    pthread_create(&tid2, &attr2, ReturnWrestler, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    // Exit the program
    winner();
    exit(0);
}
