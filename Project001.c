#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
typedef int buffer_item;
#define BUFFER_SIZE 5
#define RAND_DIVISOR 1000000
#define TRUE 1
pthread_mutex_t mutex;           //the mutex lock
sem_t full, empty;               //the semaphores
buffer_item buffer[BUFFER_SIZE]; //the buffer
int counter;                     //buffer counter
int count_w;
int count_r = 30;
pthread_t tid1, tid2;        //Thread ID
pthread_attr_t attr1, attr2; //Set of thread attributes

char q_wrestler[31][20] = {"", "Stone Cold", "The Rock", "Triple H", "Shawn Michaels",
                           "Bret Hart", "The Undertaker", "John Cena", "Randy Orton",
                           "Edge", "Batista", "Brock Lesnar", "Kurt Angle", "Hulk Hogan", "Ric Flair",
                           "Yokozuna", "CM Punk", "Goldberg", "Sting", "diesel", "Roman Reigns",
                           "Drew McIntyre", "Bobby Lashley", "Seth Rollins", "Dean Ambrose",
                           "Chris Jericho", "Sheamus", "Eddie Guerrero", "AJ Styles",
                           "Big Show", "Kane"};
char moveset[3][30] = {"toss", "kick", "clotheslined"};
int out_w[30];
void *producer(void *pno); //the producer thread
void *consumer(void *cno); //the consumer thread
void initializeData()
{
    pthread_mutex_init(&mutex, NULL); //create the mutex lock
    //Create the full semaphore and initialize to 0
    sem_init(&full, 0, 0);
    //Create the empty semaphore and initialize to BUFFER_SIZE
    sem_init(&empty, 0, BUFFER_SIZE);
    pthread_attr_init(&attr1); //get the default attributes
    pthread_attr_init(&attr2);
    counter = 0;
    count_w = 0; //init buffer
    for (int i = 0; i < 5; i++)
    {
        int *x;
        x = &buffer[i];
        *x = -1;
    }
    for (int i = 0; i < 30; i++)
    {
        int *x;
        x = &out_w[i];
        *x = -1;
    }
}
// Add an item to the buffer
int insert_item(buffer_item item)
{
    //When the buffer is not full add the item and increment the counter
    if (counter < BUFFER_SIZE)
    {
        buffer[counter] = item;
        int *out;
        out = &out_w[count_w];
        *out = item;
        counter++;
        count_w++;
        return 0;
    }
    else
    { //Error if the buffer is full
        return -1;
    }
}
// Remove an item from the buffer
int remove_item(buffer_item *item)
{
    /* When the buffer is not empty remove the item
 and decrement the counter */
    if (counter > 0)
    {
        int move = rand() % 3;
        //printf("%s is %s out of the ring from slot %d\n", q_wrestler[buffer[*item]],moveset[move],*item);
        printf("%s was removed from ring [counter = %d]\n", q_wrestler[buffer[*item]], *item);
        buffer[*item] = -1;
        *item = buffer[(counter - 1)];
        counter--;
        count_r--;
        return 0;
    }
    else
    { // Error buffer empty
        return -1;
    }
}
// Producer Thread
void *producer(void *pno)
{

    buffer_item item;
    while (TRUE)
    {
        if (count_w == 30)
        {
            printf("producer exit\n");
            pthread_exit(NULL);
        }
        item = rand() % 31; // generate a random number
        //check ring has same wrestler.
        int temp_count = 0;
        while (temp_count <= 5)
        {
            if (item == buffer[temp_count])
            {
                item = rand() % 31;
                temp_count = 0;
            }
            else
            {
                temp_count++;
            }
        }
        //check wrestler already out?
        temp_count = 0;
        while (temp_count <= 31)
        {
            if (item == out_w[temp_count])
            {
                item = rand() % 31;
                temp_count = 0;
            }
            else
            {
                temp_count++;
            }
        }
        sem_wait(&empty);           // acquire the empty lock
        pthread_mutex_lock(&mutex); // acquire the mutex lock
        if (insert_item(item) == 1)
        {
            fprintf(stderr, "same wrestler random again\n");
        }
        else
        {
            // printf("%s enters the rumble at slot %d\n", q_wrestler[item], counter);
            printf("ring[%d] add %s (%d) [count = %d]\n", counter, q_wrestler[item], count_w, counter);
        }
        pthread_mutex_unlock(&mutex); //release the mutex lock
        sem_post(&full);              //signal full
    }
}
// Consumer Thread
void *consumer(void *cno)
{
    buffer_item item;
    //srand((unsigned)time(&t));

    while (TRUE)
    {
            if (count_r == 1)
            {
                //printf("consumer exit(count_r = %d)\n",count_r);
                pthread_exit(NULL);
            }
            //check already remove wrestler.
            item = rand() % 5;
            while (buffer[item] == -1)
            {
                item = rand() % 5;
            }

            sem_wait(&full);            //acquire the full lock
            pthread_mutex_lock(&mutex); //acquire the mutex lock

            if (remove_item(&item) == -1)
            {
                fprintf(stderr, "error\n");
            }
            pthread_mutex_unlock(&mutex); //release the mutex lock
            sem_post(&empty);             //signal empty
    }
}
void winner()
{
    for (int i = 0; i < 5; i++)
    {
        if (buffer[i] != -1)
        {
            printf("the winner is %s\n", q_wrestler[buffer[i]]);
        }
    }
}
int main(int argc, char *argv[])
{
    initializeData(); //Initialize the app
    // Create the producer threads
    pthread_create(&tid1, &attr1, producer, (void *)0);
    // Create the consumer threads
    pthread_create(&tid2, &attr2, consumer, (void *)1);
    // Sleep for the specified amount of time in milliseconds
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    // Exit the program
    winner();
    printf("Exit the program\n");
    exit(0);
}
