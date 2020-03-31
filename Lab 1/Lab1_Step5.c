// Name: Ethan Paek
// Date: 3/31/2020
// Title: Lab1 - Steps 1-5
// Description: This program is a rewrite of the program given in step two. Instead of using forking, this program uses threading.
#include <stdio.h>      /* printf, stderr */
#include <sys/types.h>  /* pid_t */
#include <unistd.h>     /* fork */
#include <stdlib.h>     /* atoi */
#include <errno.h>      /* errno */
#include <pthread.h>

void *start_routine(void *arg){
  //Parent process
  for (int i = 0; i < 100; i++){
    printf("\t \t \t Parent Process %d \n",i);
    usleep(*(int*)arg);
  }
}

/* main function with command-line arguments to pass */
int main(int argc, char *argv[]) {
    pid_t  pid;
    int i, n = atoi(argv[1]); // n microseconds to input from keyboard for delay
    printf("\n Before forking.\n");
    pthread_t tid;
    pthread_create(&tid, NULL, start_routine, &n);
    // Child process
    for (i=0;i<100;i++) {
      printf("Child process %d\n",i);
      usleep(n);
    }
    pthread_join(tid, NULL);
    return 0;
}

