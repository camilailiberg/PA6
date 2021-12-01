#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <time.h>
#define BUFFER_SIZE /* 2000000000 */ 10000000 /* 20000000L */ //20000000L
#define THREADS 90

//? defining 4 flags
int l = 0 ;
int nt = 0 ;
int v = 0 ;
int t = 0 ;

int currIndexOfLengths = 0, numOfThreads = 1, lengths[1000];

char ** allFoundWords;
int currentIndexOfAllFoundWords;
long timer, totalBinarySearchTime;
pthread_mutex_t mutex;

char* buffer[THREADS]; //? a pointer char array that stores up to 90 Strings
int dictionary_size;
char* dict[100000]; //? array of chat pointers of length 100,000

static long get_nanos(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
	return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

//* Uses bjinary search to check if keyword is in dictionaryWords.
//* Returns -1 if we did not find the word.
int binsearch(char* dictionaryWords[],int listSize,char* keyword){
	// printf("keyword = %s\n" , keyword); //TODO: DELETE
	int mid, low = 0, high = listSize - 1;
	while (high >= low) {
		mid = (high + low) / 2;
		if (strcmp(dictionaryWords[mid],keyword) < 0) //? if the word is in the uppper half of dictionaryWords, use that hal
			low = mid + 1;
		else if (strcmp(dictionaryWords[mid],keyword)>0) //? else use the lower half
			high = mid - 1;
		else
			return mid; //? else we found the word
	}
	return -1; //? we did not found the word
}

//* funcion que le voy a pasar al pthread_create(...)
void* word_puzzle_solver(void* id){
	int i, f;
	
	char* buf = buffer[(int)id];

	//? verbose message
	if(v)
	{
		fprintf(stderr, "Note: Thread #%d: %s\n", (int) id, "started!"); //? prints the message to stderr
	}

	//! wrap arounf for loop as long as the size of lengths array 
	for(f = 0 ; f < currIndexOfLengths ; f++)
	{
		char c = 0;
		int n = BUFFER_SIZE/numOfThreads-lengths[f]; //! pasarle each length store in lengths en vez de LEN
		for(i = 0 ; i < n ; i++){
			if(c) //? first pass c = 0 so this if statement does not happen. Because I don't have anything to restore
				buf[i + lengths[f] - 1] = c; //? restores las character that was previously replaces to \0
			c = buf[i + lengths[f]]; //? saves las character to c
			buf[i + lengths[f]] = '\0'; //? replaces las character to \0

			//? starts timer for binary search
			timer = get_nanos(); 
			//? if search is successful! ( + 1 because if search not successful it returns -1 and -1 plus 1 is 0 so FALSE)
			if(binsearch(dict, dictionary_size, buf + i) + 1)
			{
				//? printing word found
				printf("Thread #%d: %s\n",(int)id, buf + i);
			}
			//? calculates time for binary search
			timer = get_nanos() - timer; 
			
			//? lock mutex
			pthread_mutex_lock(&mutex);
			//? calculates total binary serarch time
			totalBinarySearchTime += timer;
			//? unlock mutex
			pthread_mutex_unlock(&mutex);
		}
	}
}

void main(int argc, char** argv){
	long startTime = get_nanos(); //? start the timer

	//? initialize mutex 
	pthread_mutex_init(&mutex,NULL);
	
	// int thread_number, j, size = BUFFER_SIZE/numOfThreads;
	// char temp[100]; //? String of length 100
	// pthread_t threadID[numOfThreads]; //? array of threads
	// char line[1000]; //? String of length 1,000
	//? pointer: pointer to char
	//? holding: holds the hole line of lengths inputed by the user Ex: 3,4
	//? lenOfWordByUserInput: holds each length at a time
	char * pointer, *holding, *holdingNThreads, *lenOfWordByUserInput ; 
	//? lengths: array of integers that will hold the lengths inputed by the user after command line argument -len
	// int lengths[1000], lengthsDefault[] = {8,9}; //! move these to be global

	int k = 1 /*,currIndexOfLengths = 0, numOfThreads = 1 ;*/; //! move these to be global
	//* handling command line arguments
	while(argv[k] != NULL)
    {
        if ( strcmp(argv[k], "-len") == 0 )
        {
			printf("\n\n-len\n"); //TODO: DELETE

            //? setting l flag to true
            l = 1 ;

			//? grabbing the hole line with lengths inputed by the user
			holding = argv[k+1];
			//? grab the next number inputed by the user
			lenOfWordByUserInput = strtok(holding, ",");


			//? adds all of the lengths inputed by the user to the array lengths
			//! KEEP IN MIND: currIndexOfLengths will be correct for humans in terms of the lenght of the array lengths but index-wise it will be one more.
			while(lenOfWordByUserInput != NULL)
			{
				lengths[currIndexOfLengths++] = atoi(lenOfWordByUserInput);
				lenOfWordByUserInput = strtok(NULL, ",");
			}
			printf("\ncurrIndexOfLengths = %d\n", currIndexOfLengths); //TODO: DELETE
			printf("\nlengths[0] = %d\n", lengths[0]); //TODO: DELETE
			printf("lengths[1] = %d\n", lengths[1]); //TODO: DELETE

        }
        else if ( strcmp(argv[k], "-nthreads") == 0 )
        {
            //? setting nt flag to true
            nt = 1 ;
			printf("\n-nthreads\n"); //TODO: DELETE

			//? grabbing the number of threds the user wants and saving it in holdingNThreads 
			holdingNThreads = argv[k+1];

			//? converting to number
			numOfThreads = atoi(holdingNThreads);
			
        }
        else if ( strcmp(argv[k], "-verbose") == 0 )
        {
            //? setting v flag to true
            v = 1 ;
        }
        else if ( strcmp(argv[k], "-time") == 0 )
        {
			//? setting t flag to true
            t = 1 ;
        } 
        k++;
    }
	printf("\n\n--------------------------------------------------\n\n"); //TODO: DELETE
	//* finish handling command line arguments

	if(!l)
	{
		lengths[0]=8;
		lengths[1]=9;
		currIndexOfLengths = 2 ;
	}

	int thread_number, j, size = BUFFER_SIZE/numOfThreads;
	char temp[100]; //? String of length 100
	pthread_t threadID[numOfThreads]; //? array of threads
	char line[1000]; //? String of length 1,000
	
	//* Reading the dict.txt file and storing all of the words from dict.txt in the array dict
	FILE* f = fopen("dict.txt", "r"); //? opening file in reading mode
	dictionary_size = 0;
	while(fgets(line, 1000, f)){ //? reading a max of 1,000 characters from file f(dict.txt) and saving them to line. So while it can read . . .
		sscanf(line, "%s\n", temp); //? Read from line a string and save it in temp
		if(strlen(temp) == 0)
			continue;
		dict[dictionary_size] = (char*) malloc(sizeof(temp)+1); //? allocates memory in index dictionary_size for string temp plus 1
		strcpy(dict[dictionary_size++], temp); //? copies temp in index dictionary_size of dict and adds 1 to dictionary_size
	}
	fclose(f);
	//* Finish reading the dic.txt file

	printf("numOfThreads = %d\n", numOfThreads) ; //TODO: DELETE

	for(thread_number = 0; thread_number < ((numOfThreads/currIndexOfLengths) * currIndexOfLengths) ; thread_number++)
	{
		buffer[thread_number] = (char*) malloc(size + 1); //? allocating memory
		
		//? reading from stdin a max of size + 1 characters and storing it in buffer[thread_number]. It returns 1 or 0
		if(!fgets(buffer[thread_number], size + 1, stdin)) 
		{
			//? error handling
			fprintf(stderr, "Error: can't read the input stream!\n\n");
			break;
		}
		
		// printf("\n\nbuffer[0] = \n%s\n\n", buffer[0]); //TODO: DELETE

		//? calls word_puzzle solver to check if word is in dictionary
		//? it does threadID + thread_number brcause is like accessing each addess of each index of the thread array basically it works fine.
		if(pthread_create(threadID + thread_number, NULL, word_puzzle_solver, (void *) thread_number)) 
		{
			//? error handling in case pthread_create is not successful
			fprintf(stderr, "Error: Too many threads are created!\n");
			break;
		}
	}
	printf("\nthread_number = %d\n\n", thread_number); //TODO: DELETE

	for(j = 0; j < thread_number;j++){
		//? passes the thread id on index j of the array of threads called threadID. We pass NULL because we do not care about the return value. pthread_join blocks the calling thread until the specified threadID[j] terminates
		pthread_join(threadID[j], NULL); 

		//? verbose message
		if(v)
		{
			fprintf(stderr, "Note: Thread %d joined!\n", j);
		}
	}
	pthread_mutex_destroy(&mutex);

	//? verbose message
	if(v)
	{
		fprintf(stderr, "Note: Total time: %ld seconds using %d threads!\n", 
		(get_nanos()-startTime) / 1000000000, numOfThreads);
	}

	//? if users asks for time (-time)
	if(t)
	{
		printf("Total Binary Search Time in seconds = %ld\n", totalBinarySearchTime / 1000000000 );
	}
}