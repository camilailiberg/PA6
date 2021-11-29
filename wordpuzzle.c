#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>
#include <time.h>
#define BUFFER_SIZE 20000000L //20000000L
#define THREADS 90
#define LEN 9
// #include <randomgen.h>
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
		if(binsearch(dict, dictionary_size, buf + i) + 1)//if search is successful!
			printf("Thread #%d: %s\n",(int)id, buf + i);
	}
}
void main(){
	long startTime = get_nanos(); //? start the timer
	int thread_number, j, size = BUFFER_SIZE/THREADS;
	char temp[100]; //? String of length 100
	pthread_t threadID[THREADS]; //? array of threads
	char line[1000]; //? String of length 1,000
	char * pointer; //? pointer to char
	
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
	for(thread_number = 0; thread_number < THREADS;thread_number++)
	{
		buffer[thread_number] = (char*) malloc(size + 1); //? allocating memory

		if(!fgets(buffer[thread_number], size + 1, stdin)) //? reading from stdin a max of size + 1 characters and storing it in buffer[thread_number]
		{
			// printf("Inside if right before error printing !fgets = %d\n", !fgets(buffer[thread_number], size + 1, stdin)); //TODO: DELETE
			fprintf(stderr, "Error: can't read the input stream!\n\n");
			break;
		}
		// printf("Outside if right after !fgets = %d\n\n", !fgets(buffer[thread_number], size + 1, stdin)); //TODO: DELETE
		printf("Outside if right after buffer[0] = %s\n\n", buffer[0]); //TODO: DELETE


		if(pthread_create(threadID + thread_number, NULL, word_puzzle_solver, (void *) thread_number)) //? error handling in case pthread_create is not successful
		{
			fprintf(stderr, "Error: Too many threads are created!\n");
			break;
		}
	}
	//? finish error handling

	for(j = 0; j < thread_number;j++){
		pthread_join(threadID[j], NULL); //? passes the thread id on index j of the array of threads called threadID. We pass NULL because we do not care about the return value. pthread_join blocks the calling thread until the specified threadID[j] terminates
		fprintf(stderr, "Note: Thread %d joined!\n", j);
	}
	fprintf(stderr, "Note: Total time: %ld nanoseconds using %d threads!\n", 
		get_nanos()-startTime, thread_number);
}