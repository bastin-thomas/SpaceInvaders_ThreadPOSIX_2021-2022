#include "threadInvader.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  Thread Invader /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadInvader(){
  bool Alienisdead;
  pthread_t handlerFlotteAliens;
  int level = 01;

  mLock(&mutexGrille);
    MajLevel(level);
  mUnLock(&mutexGrille);
  
  do{
      #ifdef DEBUG
        printf("\n(ThreadInvader %ld)... Level: %d Délai : %d...\n\n",getTid(),level, delai);
      #endif

      //Creation threadFlotteAliens
      pthread_create(&handlerFlotteAliens, NULL, (void* (*)(void*))threadFlotteAliens, NULL);

      //Se Synchronise sur la fin du Thread threadFlotteAliens, récupère Valeur Retour
      pthread_join(handlerFlotteAliens, NULL);


      mLock(&mutexFlotteAliens);
      mLock(&mutexGrille);
        freeAlien();
        RestoreShield();
      
        //Si Flotte Tuée
        if(nbAliens == 0){
            delai *= 0.70;

            MajLevel(++level);
            
            mUnLock(&mutexGrille);
            mUnLock(&mutexFlotteAliens);
            continue;
        }
        
        //Si Joueur Perdu  
        pthread_kill(tab[NB_LIGNE-1][colVaisseau].tid,SIGQUIT);
        
        #ifdef DEBUG
          printf("(ThreadInvader %ld)... PERDU ...\n",getTid());
        #endif

      mUnLock(&mutexGrille);
      mUnLock(&mutexFlotteAliens);
  }while(1);

  pthread_exit(NULL);
}

void RestoreShield(){
  setTab(NB_LIGNE-2,11,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,11,1);
  setTab(NB_LIGNE-2,12,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,12,1);
  setTab(NB_LIGNE-2,13,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,13,1);
  setTab(NB_LIGNE-2,17,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,17,1);
  setTab(NB_LIGNE-2,18,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,18,1);
  setTab(NB_LIGNE-2,19,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,19,1);
}


void MajLevel(int level){
  DessineChiffre(13,3,level/10);
  DessineChiffre(13,4,level%10);
}