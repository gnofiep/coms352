						
	/* Include Files */
	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
	/* External References
	*/		
extern int *world(void ); 
extern int *hello(void );
extern int *exclamation(void);
pthread_mutex_t mutex, mutex2;
pthread_t thread[3];

int main( int argc, char *argv[] ) { 
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex2, NULL);
	pthread_mutex_lock(&mutex2);
	pthread_create(&thread[0], NULL, world, NULL);
	pthread_create(&thread[1], NULL, hello, NULL);
	pthread_create(&thread[2], NULL, exclamation, NULL);
	// world();
	
	pthread_join(thread[0],NULL);	
	// hello();
	pthread_join(thread[1],NULL);
	// exclamation(); 
	pthread_join(thread[2],NULL);
	printf( "\n" );	
	// pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutex2);
	return( 0 ); 
}
	/* world - print the "world" part. */

int *world( void  ) {
	pthread_mutex_lock(&mutex2);
	printf( "world" );
	pthread_mutex_unlock(&mutex2);					
	pthread_exit(NULL);
}

	/* hello - print the "hello" part. */

int *hello( void ) {
	pthread_mutex_lock(&mutex);
	printf( "hello " );
	pthread_mutex_unlock(&mutex2);
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

	/* exclamation – print “!”.*/ 
int *exclamation(){ 
	pthread_mutex_lock(&mutex);
	printf("!");
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);				
} 

