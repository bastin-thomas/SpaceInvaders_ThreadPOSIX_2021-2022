#include "threadMissile.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  Thread Missile /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadMissile(void* pos){
  S_POSITION Position = *((S_POSITION*)pos);
  free(pos);
  sigset_t masque;
  
  sigfillset(&masque);
  sigdelset(&masque, SIGINT);
  sigprocmask(SIG_SETMASK, &masque, NULL);

  //Entrée dans la grille de jeu:
  mLock(&mutexGrille);
  switch(tab[Position.L][Position.C].type){ 
    //Si case Vide apparition du missile
    case VIDE:  
                  setTab(Position.L, Position.C, MISSILE, getTid());
                  DessineMissile(Position.L, Position.C);
            break;
    
    //Si case Bouclier1 Transformation en Bouclier2
    case BOUCLIER1:
                  setTab(Position.L, Position.C, BOUCLIER2, 0);
                  DessineBouclier(Position.L, Position.C, 2);
                mUnLock(&mutexGrille);
                pthread_exit(NULL);
              break;

    //Si case Bouclier2 Supression Bouclier2
    case BOUCLIER2:
                  setTab(Position.L, Position.C, VIDE, 0);
                  EffaceCarre(Position.L, Position.C);
                mUnLock(&mutexGrille);
                pthread_exit(NULL);
              break;
  }
  mUnLock(&mutexGrille);


  while(Position.L > 0){
    Attente(80);
    
    //Supression du missile si il tombe sur un Alien
    mLock(&mutexFlotteAliens);
    mLock(&mutexGrille);
    
      if(tab[Position.L-1][Position.C].type == ALIEN)
      {
        
          //Avance du missile d'une case
          EffaceCarre(Position.L, Position.C);

          setTab(Position.L, Position.C, VIDE, 0);
          Position.L--;
          setTab(Position.L, Position.C, VIDE, 0);
          
          
            nbAliens--;

            //Réveil Thread Score pour l'affichage + Incrément de 1.
            mLock(&mutexScore);
              Score++;
              MajScore = true;
            mUnLock(&mutexScore);
            pthread_cond_signal(&condScore);

            RechercheBordure();
            
            #ifdef DEBUG
              AfficheTab();
              printf("(threadMissile %ld) Un Aliens est Mort, Il reste %d Aliens\n",getTid(), nbAliens); fflush(stdout);
            #endif

          EffaceCarre(Position.L, Position.C);
        mUnLock(&mutexGrille);  
        mUnLock(&mutexFlotteAliens);
        pthread_exit(NULL);
      }

      if(tab[Position.L-1][Position.C].type == VIDE)
      {
          //Avance du missile d'une case
          EffaceCarre(Position.L, Position.C);

          setTab(Position.L, Position.C, VIDE, 0);
          Position.L--;
          setTab(Position.L, Position.C, MISSILE, getTid());

          DessineMissile(Position.L, Position.C);
      }    

      if(tab[Position.L-1][Position.C].type == BOMBE || tab[Position.L-1][Position.C].type == MISSILE)
      {
          //Tuage threadBombe
          pthread_kill(tab[Position.L-1][Position.C].tid, SIGINT);

          //Effacement Bombe
          EffaceCarre(Position.L-1, Position.C);
          setTab(Position.L-1, Position.C, VIDE, 0);

          //Effacement du missile
          EffaceCarre(Position.L, Position.C);
          setTab(Position.L, Position.C, VIDE, 0);
          
          mUnLock(&mutexGrille);
          mUnLock(&mutexFlotteAliens);
          pthread_exit(NULL);
      }    
      mUnLock(&mutexGrille);  
      mUnLock(&mutexFlotteAliens);
  }

  Attente(80);
  
  //Supression du missile si il est arrivé tout en haut
  mLock(&mutexGrille);
    setTab(Position.L, Position.C, VIDE, 0);
    EffaceCarre(Position.L, Position.C);
  mUnLock(&mutexGrille);

  pthread_exit(NULL);
}


//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Handler SIGINT ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void handlerSIGINT(int sig){
  mUnLock(&mutexGrille);
  mUnLock(&mutexFlotteAliens);
  pthread_exit(NULL);
}