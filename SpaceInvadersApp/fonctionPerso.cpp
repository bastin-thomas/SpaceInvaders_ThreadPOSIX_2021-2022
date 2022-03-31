#include "fonctionPerso.h"


void Attente(int milli)
{
  struct timespec del;
  del.tv_sec = milli/1000;
  del.tv_nsec = (milli%1000)*1000000;
  nanosleep(&del,NULL);
}


void setTab(int l,int c,int type,pthread_t tid)
{
  tab[l][c].type = type;
  tab[l][c].tid = tid;
}


void freeAlien(){
  for(int i = 0 ; i < NB_LIGNE-2 ; i++){
    for(int j = 8 ; j < NB_COLONNE ; j++){
      if(tab[i][j].type == ALIEN){
        setTab(i,j,VIDE,0);
        EffaceCarre(i,j);
      }

      if(tab[i][j].type == MISSILE || tab[i][j].type == BOMBE){
        pthread_kill(tab[i][j].tid, SIGINT);
        setTab(i,j,VIDE,0);
        EffaceCarre(i,j);
      }
    }
  }
}


void AfficheTab(){
  printf("\n");
  printf("\n");
  for(int i = 0; i<NB_LIGNE-1; i++){
    for(int j = 8; j<NB_COLONNE; j++){
      printf("%d\t",tab[i][j].type);
    }
    printf("\n");
    printf("\n");
  }
  printf("\n");
  printf("\n");
}


void MajVies(){
  if(nbVies == 2){
    EffaceCarre(16,4);
  }
  if(nbVies == 1){
    EffaceCarre(16,3);
  }
}
