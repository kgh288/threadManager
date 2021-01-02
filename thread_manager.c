#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "Node.h"

//thread mutex lock for access to the log index
pthread_mutex_t tlock1 = PTHREAD_MUTEX_INITIALIZER;
//thread mutex lock for critical sections of allocating THREADDATA
pthread_mutex_t tlock2 = PTHREAD_MUTEX_INITIALIZER; 


void* thread_runner(void*);
void print_time(void);
pthread_t tid1, tid2;
struct THREADDATA_STRUCT
{
  pthread_t creator;
};
typedef struct THREADDATA_STRUCT THREADDATA;

THREADDATA* p=NULL;


//variable for indexing of messages by the logging function
int logindex=0;
int *logip = &logindex;


//A flag to indicate if the reading of input is complete, 
//so the other thread knows when to stop
bool is_reading_complete = false;
LineNode* head;



/*********************************************************
// function main  ------------------------------------------------- 
*********************************************************/
int main()
{
  head = (LineNode*)malloc(sizeof(LineNode));
  printf("create first thread\n");
  pthread_create(&tid1,NULL,thread_runner,NULL);
  
  printf("create second thread\n");
  pthread_create(&tid2,NULL,thread_runner,NULL);
  
  printf("wait for first thread to exit\n");
  pthread_join(tid1,NULL);
  printf("first thread exited\n");

  printf("wait for second thread to exit\n");
  pthread_join(tid2,NULL);
  printf("second thread exited\n");

  exit(0);

}//end main



/**********************************************************************
// function thread_runner runs inside each thread -------------------------------------------------- 
**********************************************************************/
void* thread_runner(void* x)
{

    char buf[100];
    char *output;

    pthread_t me;

    me = pthread_self();
    printf("This is thread %ld (p=%p)\n",me,p);

    pid_t pid = getpid(); 
    
    
    //enter
    printf("This is thread %ld and I created the THREADDATA %p\n",me,p);
    printf("logindex: %d, thread %ld, PID %d, ", logindex++, me, getpid());
    print_time();
    printf(": Entered Thread Runner %p\n", p);
    
    
    pthread_mutex_lock(&tlock2); // critical section starts
    if (p==NULL) {
        p = (THREADDATA*) malloc(sizeof(THREADDATA));
        p->creator=me;
    }
    pthread_mutex_unlock(&tlock2);  // critical section ends
    
    bool isfirstCreate = true;
    
    
    //reading lines---------------
    if (p!=NULL && p->creator==me)
	{
		while((output = fgets(buf, 100, stdin)) != NULL)
        {
			if(*output == '\n') 
            {
                printf("reading is done. Exiting thread\n");
                is_reading_complete = true;
                break;
            }
            //printf("This is thread %ld and I created the THREADDATA %p\n",me,p);
            
            if(isfirstCreate) 
            {
                CreateLineNode(head, buf, NULL);
                isfirstCreate = false;
            }
            else
            {
                LineNode* newNode = (LineNode*)malloc(sizeof(LineNode));
                CreateLineNode(newNode, buf, NULL);
                push(&head, newNode);
            }
            
            pthread_mutex_lock(&tlock2);
            printf("logindex: %d, thread %ld, PID %d, ", logindex++, me, getpid());
            pthread_mutex_unlock(&tlock2);
            print_time();
            printf(": Created and inserted node\n");
		}
	}
	else
	{
		//printf("This is thread %ld and I can access the THREADDATA %p\n",me,p);
        printf("logindex: %d, thread %ld, PID %d ", logindex++, me, getpid());
        print_time();
        printf(": can access the THREADDATA %p\n", p);
        

	}

    // TODO use mutex to make this a start of a critical section 
    pthread_mutex_lock(&tlock1);
    if (p!=NULL && p->creator==me)
    {
//         printf("logindex: %d, thread %ld, PID %d ", logindex++, me, getpid());
//         print_time();
//         printf(": did not touch THREADDATA %p\n", p);
        free(p);
    }
    else
    {
        /**
        * TODO Free the THREADATA object. Freeing should be done by the other thread from the one that created it.
        * See how the THREADDATA was created for an example of how this is done.
        */
        //printf("This is thread %ld\n",me);
        
        char tmp[100];
        strcpy(tmp, head->line);
        
        while(!is_reading_complete) 
        {
            if(!strcmp(tmp, head->line))
            {
                sleep(0.5);
            }
            else 
            {
                sleep(1);
                //printf("checking if reading is done\n");
                printf("logindex: %d, thread %ld, PID %d ", logindex++, me, getpid());
                print_time();
                //printf(": deleted the THREADDATA %p\n", p);
                printf(": Head of linked list contains: %s", head->line);
                strcpy(tmp, head->line);
            }
        }
        pthread_mutex_lock(&tlock2);
        LineNode* tempNode = (LineNode*)malloc(sizeof(LineNode));
        while ((tempNode = head) != NULL) 
        { 
            head = head->next;          
            free (tempNode);                
        }
        free(tempNode);
        printf("logindex: %d, thread %ld, PID %d ", logindex++, me, getpid());
        print_time();
        printf(": Deallocated linked list\n");
        
       
        
        pthread_mutex_unlock(&tlock2);
    }
    // TODO critical section ends
    pthread_mutex_unlock(&tlock1);

    pthread_exit(NULL);
    return NULL;

}//end thread_runner

// Print current date and time in C
void print_time(void)
{
	// variables to store date and time components
	int hours, minutes, seconds, day, month, year;

	// time_t is arithmetic time type
	time_t now;
	
	// Obtain current time
	// time() returns the current time of the system as a time_t value
	time(&now);

	// Convert to local time format and print to stdout
	//printf("Today: %s", ctime(&now));

	// localtime converts a time_t value to calendar time and 
	// returns a pointer to a tm structure with its members 
	// filled with the corresponding values
	struct tm *local = localtime(&now);

    hours = local->tm_hour;      	// get hours since midnight (0-23)
    minutes = local->tm_min;     	// get minutes passed after the hour (0-59)
    seconds = local->tm_sec;     	// get seconds passed after minute (0-59)

    day = local->tm_mday;        	// get day of month (1 to 31)
    month = local->tm_mon + 1;   	// get month of year (0 to 11)
    year = local->tm_year + 1900;	// get year since 1900
    
	// print current date
	printf("Date: %02d/%02d/%d ", day, month, year);
	// print local time
	if (hours < 12)	// before midday
		printf("Time: %02d:%02d:%02d am ", hours, minutes, seconds);
	else	// after midday
		printf("Time: %02d:%02d:%02d pm ", hours - 12, minutes, seconds);

}
