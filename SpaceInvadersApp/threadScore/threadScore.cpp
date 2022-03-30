#include "threadScore.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  Thread Score //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadScore(){
  #ifdef DEBUG
    printf("(threadScore %ld) Score: %d \n",getTid(), Score);
  #endif

  DessineChiffre(10,2,0);
  DessineChiffre(10,3,0);
  DessineChiffre(10,4,0);
  DessineChiffre(10,5,0);

  //Init Score 0
  Score = 0;
  MajScore = false;

  while(1){
    mLock(&mutexScore);
    while(!MajScore){ //Tant que MajScore est False...
      pthread_cond_wait(&condScore, &mutexScore);

      #ifdef DEBUG
        printf("(threadScore %ld) Score: %d \n",getTid(), Score);
      #endif

      DessineChiffre(10,2, Score/1000);
      DessineChiffre(10,3, (Score%1000)/100);
      DessineChiffre(10,4, ((Score%1000)%100)/10);
      DessineChiffre(10,5, ((Score%1000)%100)%10); //Mise à jour Graphique du Score
    }
    MajScore = false; //Reset MajScore
    mUnLock(&mutexScore);
  }
  pthread_exit(NULL);
}