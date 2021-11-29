#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <time.h>
#define BUFFER_SIZE 20000000L //20000000L
#define THREADS 2
#define LEN 9
// #include <randomgen.h>

char ** allFoundWords;
int currentIndexOfAllFoundWords;
long timer, totalBinarySearchTime;
pthread_mutex_t mutex;

char* buffer[THREADS]; //? a pointer char array that stores up to THREADS Strings strings
int dictionary_size;
char* dict[100000]; //? array of chat pointers of length 100,000

static long get_nanos(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
	return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

//* Uses bjinary search to check if keyword is in dictionaryWords.
//* Returns -1 if we did not found the word.
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
//* 
void* word_puzzle_solver(void* id){
	char c = 0;
	int i, n = BUFFER_SIZE/THREADS-LEN;
	char* buf = buffer[(int)id];
	fprintf(stderr, "Note: Thread #%d: %s\n", (int) id, "started!"); //? prints the message to stderr
	for(i = 0 ; i < n ; i++){
		if(c) //? first pass c = 0 so this if statement does not happen.
			buf[i + LEN - 1] = c;
		c = buf[i + LEN]; //?
		// printf("\nbuf[ i + LEN ] = %c\n", buf[ i + LEN ]); //TODO: DELETE
		// printf("\nc = %c\n", c); //TODO: DELETE
		buf[i + LEN] = '\0';
		// printf("\nbuf + i = %s\n", buf + i); //TODO: DELETE

		timer = get_nanos();
		if(binsearch(dict, dictionary_size, buf + i) + 1)//? if search is successful!
			printf("Thread #%d: %s\n",(int)id, buf + i);
		timer = get_nanos() - timer;
		
		pthread_mutex_lock(&mutex);//!lock mutex
		totalBinarySearchTime += timer;
		pthread_mutex_unlock(&mutex);//! unlock mutex
	}
}
void main(int argc, char** argv){
	long startTime = get_nanos(); //? start the timer
	int thread_number, j, size = BUFFER_SIZE/THREADS;
	char temp[100]; //? String of length 100
	pthread_t threadID[THREADS]; //? array of threads
	char line[1000]; //? String of length 1,000
	//? pointer: pointer to char
	//? holding:
	//? lenOfWordByUserInput:
	char * pointer, *holding, *lenOfWordByUserInput ; 
	//? lengths: array of integers that will hold the lengths inputed by the user after command line argument -len
	int lengths[1000] ; 


	//? defining 4 flags
    int l = 0 ;
    int nt = 0 ;
    int v = 0 ;
    int t = 0 ;

	int k = 1, currIndexOfLEngths = 0 ;
	while(argv[k] != NULL)
    {
        if ( strcmp(argv[k], "-len") == 0 )
        {
			printf("\n\n-len\n"); //TODO: DELETE

            //? setting l flag to true
            l = 1 ;

			//? grabbing the hole line with lengths inputed by the user
			holding = argv[k+1];
			printf("\nholding = %s\n", holding); //TODO: DELETE
			//? grab the next number inputed by the user
			lenOfWordByUserInput = strtok(holding, ",");
			printf("\nlenOfWordByUserInput = %d\n", atoi(lenOfWordByUserInput)); //TODO: DELETE

			//? add it to the array that will hold the lenghts inputed by the user

			while(lenOfWordByUserInput != NULL)
			{
				lengths[currIndexOfLEngths++] = atoi(lenOfWordByUserInput);
				lenOfWordByUserInput = strtok(NULL, ",");
			}
			printf("\nlengths[0] = %d\n", lengths[0]); //TODO: DELETE
			printf("lengths[1] = %d\n", lengths[1]); //TODO: DELETE

        }
        else if ( strcmp(argv[k], "-nthreads") == 0 )
        {
            //? setting nt flag to true
            nt = 1 ;
			printf("-nthreads"); //TODO: DELETE
            //? getting the name for the second player.
            // strcpy(secondPlayerName, argv[k+1]);
        }
        else if ( strcmp(argv[k], "-verbose") == 0 )
        {
            //? setting v flag to true
            v = 1 ;
			printf("-verbose"); //TODO: DELETE
        }
        else if ( strcmp(argv[k], "-time") == 0 )
        {
			//? setting t flag to true
            t = 1 ;
			printf("-time"); //TODO: DELETE
        } 
        k++;
    }
	printf("\n\n--------------------------------------------------\n\n"); //TODO: DELETE
	
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
	
	// printf("\ndic[ultimo] = %s\n\n", dict[dictionary_size-1]); //TODO: DELETE

	//? error handling
	printf("\nthread_number BEFORE for loop = %d\n\n", thread_number); //TODO: DELETE
	for(thread_number = 0; thread_number < THREADS ;thread_number++)
	{
		printf("\nthread_number INSIDE for loop = %d\n\n", thread_number); //TODO: DELETE
		buffer[thread_number] = (char*) malloc(size + 1); //? allocating memory

		printf("\nsize + 1 = %d\n\n", size + 1); //TODO: DELETE
		if(!fgets(buffer[thread_number], size + 1, stdin)) //? reading from stdin a max of size + 1 characters and storing it in buffer[thread_number]
		{
			// printf("Inside if right before error printing !fgets = %d\n", !fgets(buffer[thread_number], size + 1, stdin)); //TODO: DELETE
			fprintf(stderr, "Error: can't read the input stream!\n\n");
			break;
		}

		//TODO: DELETE BLOCK
		// printf("Outside if right after !fgets = %d\n\n", !fgets(buffer[thread_number], size + 1, stdin)); //TODO: DELETE
		printf("\n\nbuffer[0] = \n%s\n\n", buffer[0]); //TODO: DELETE
		printf("\nthreadID = %d\n\n", threadID); //TODO: DELETE
		printf("\nthread_number = %d\n\n", thread_number); //TODO: DELETE
		printf("\nthreadID + thread_number = %d\n\n", threadID + thread_number); //TODO: DELETE
		//TODO: END OF DELETE BLOCK

		//? initialize mutex
		pthread_mutex_init(&mutex,NULL);
		//? calls word_puzzle solver to check if word is in dictionary
		if(pthread_create(threadID + thread_number, NULL, word_puzzle_solver, (void *) thread_number)) 
		{
			//? error handling in case pthread_create is not successful
			fprintf(stderr, "Error: Too many threads are created!\n");
			break;
		}
	}
	//? finish error handling

	printf("\nthread_number = %d\n\n", thread_number); //TODO: DELETE
	for(j = 0; j < thread_number;j++){
		pthread_join(threadID[j], NULL); //? passes the thread id on index j of the array of threads called threadID. We pass NULL because we do not care about the return value. pthread_join blocks the calling thread until the specified threadID[j] terminates
		fprintf(stderr, "Note: Thread %d joined!\n", j);
		pthread_mutex_destroy(&mutex);
	}


	fprintf(stderr, "Note: Total time: %ld nanoseconds using %d threads!\n", 
		get_nanos()-startTime, thread_number);
}