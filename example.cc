// C includes
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <vector>

using namespace std;
		   
class Number {

public:
  int num;
  bool isStored;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
    
  Number() {
    this->num = 0;
    this->isStored = false;
    
  }

  int getNum();
  void setNum(int num);

};

int Number::getNum() {

  pthread_mutex_lock(&mutex);

  while(!isStored)
    pthread_cond_wait(&cond, &mutex);

  cout << "Reading " << num << endl;
  
  pthread_mutex_unlock(&mutex);
  
  return num;
}

void Number::setNum(int num) {

  pthread_mutex_lock(&mutex);

  cout << "Storing " << num << endl;
  
  this->isStored = true;
  this->num = num;

  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
}

// prototype for thread routine
void* store(void*);
void* read(void*);

pthread_cond_t cond;

int
main(int argc, char **argv) {
    int option;

    // setup default arguments
    int num_threads = 10;

    // process command line options using getopt()
    // see "man 3 getopt"
    while ((option = getopt(argc,argv,"n:")) != -1) {
        switch (option) {
            case 'n':
                num_threads = atoi(optarg);
                break;
            default:
                cout << "server [-p port]" << endl;
                exit(EXIT_FAILURE);
        }
    }

    // initialize random seed:
    srand(time(NULL));
    
    pthread_t* thread = new pthread_t;
    pthread_t* readThread = new pthread_t;

    Number* num = new Number();
    
    pthread_mutex_init(&num->mutex, NULL);
    pthread_cond_init(&cond, NULL);
  
    pthread_create(thread, NULL, &store, (void *) num);
      
    pthread_create(readThread, NULL, &read, (void *) num);

    pthread_join(*thread, NULL);
    delete thread;    

    pthread_join(*readThread, NULL);
    delete readThread;
    delete num;
}

void* read(void* ptr) {
  Number* num;
  num = (Number*) ptr;  

  num->getNum();

  pthread_exit(0);
}


void* store(void* ptr) {
  Number* num;
  num = (Number*) ptr;
 
  // generate random number
  int r = rand()%100+1;
  
  num->setNum(r);
 
  pthread_exit(0);
}
