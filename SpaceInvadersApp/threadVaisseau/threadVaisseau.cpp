#include "threadVaisseau.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Thread Vaisseau /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadVaisseau(){
  #ifdef DEBUG
    printf("(ThreadVaisseau %ld) Lancement du Thread\n",getTid());
  #endif

  pthread_cleanup_push( (void (*)(void*)) TermVaisseau, NULL);

  // Démasquage des signaux SIGUSR1, SIGUSR2, SIGHUP
  sigset_t masque;
  sigfillset(&masque);
  sigdelset(&masque, SIGUSR1);
  sigdelset(&masque, SIGUSR2);
  sigdelset(&masque, SIGHUP);
  sigdelset(&masque, SIGQUIT);
  sigprocmask(SIG_SETMASK, &masque, NULL);

  #ifdef DEBUG
    printf("(ThreadVaisseau %ld) Init du Vaisseau\n",getTid()); 
  #endif 

  colVaisseau = 15;


  mLock(&mutexGrille);
  if (tab[NB_LIGNE-1][colVaisseau].type == VIDE || tab[NB_LIGNE-1][colVaisseau].type == BOMBE) 
  {
    setTab(NB_LIGNE-1, colVaisseau, VAISSEAU, getTid());
    DessineVaisseau(NB_LIGNE-1, colVaisseau);
  }
  else{
    printf("(ThreadVaisseau %ld) ERROR Init du Vaisseau: Case Non Vide\n",getTid()); 
    exit(1);
  } 
  mUnLock(&mutexGrille);
  
  //Attente Signal
  while(1)
  {
    pause();
  }

  #ifdef DEBUG
    printf("(ThreadVaisseau %ld) Fermeture du Thread\n",getTid()); fflush(stdout);
  #endif

  pthread_cleanup_pop(1);
  pthread_exit(NULL);
}


void TermVaisseau(){
  #ifdef DEBUG
    printf("(ThreadVaisseau %ld) Mort du Vaisseau\n",getTid()); 
  #endif 

  mLock(&mutexVies);
    nbVies--;
    MajVies();
    pthread_cond_signal(&condVies);
  mUnLock(&mutexVies);
}


//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Handler SIGUSR1 ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void handlerSIGUSR1(int sig){
  //printf("(ThreadVaisseau %ld) Déplacement Droite\n",getTid());

  if(colVaisseau == NB_COLONNE - 1) return; //Si on est au bord de la colonne on skip l'instruction

  if (tab[NB_LIGNE-1][colVaisseau+1].type == VIDE) 
  {
    mLock(&mutexGrille);
      //Supression du sprite actuelle, et ajout du sprite a droite comme étant le vaisseau.
      EffaceCarre(NB_LIGNE-1, colVaisseau);
      setTab(NB_LIGNE-1, colVaisseau, VIDE, 0);
      setTab(NB_LIGNE-1, colVaisseau+1, VAISSEAU, getTid());
      DessineVaisseau(NB_LIGNE-1, colVaisseau+1);
    mUnLock(&mutexGrille);
    

    //Update Emplacement Vaisseau connus de façons Global
    colVaisseau++;
  }
}


//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Handler SIGUSR2 ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void handlerSIGUSR2(int sig){
  //printf("(ThreadVaisseau %ld) Déplacement Gauche\n",getTid());

  if(colVaisseau == 8 ) return; //Si on est au bord de la colonne on skip l'instruction

  if (tab[NB_LIGNE-1][colVaisseau-1].type == VIDE) 
  {
    mLock(&mutexGrille);
      //Supression du sprite actuelle, et ajout du sprite a gauche comme étant le vaisseau.
      EffaceCarre(NB_LIGNE-1, colVaisseau);
      setTab(NB_LIGNE-1, colVaisseau, VIDE, 0);
      setTab(NB_LIGNE-1, colVaisseau-1, VAISSEAU, getTid());
      DessineVaisseau(NB_LIGNE-1, colVaisseau-1);
    mUnLock(&mutexGrille);

    //Update Emplacement Vaisseau connus de façons Global
    colVaisseau--;
  }
}



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Handler SIGHUP ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
void handlerSIGHUP(int sig){
  //printf("(ThreadVaisseau %ld) Envoi Missile\n",getTid());
  pthread_t handler;
  S_POSITION* Position = new S_POSITION;
  Position->L = NB_LIGNE-2;
  Position->C = colVaisseau;

  mLock(&mutexFireOn);
  if(fireOn == true){
    //Lancement ThreadEvent:
    pthread_create(&handler, NULL, (void* (*)(void*))threadMissile, (void*) Position);
    pthread_detach(handler);

    fireOn = false;
    
    pthread_create(&handler, NULL, (void* (*)(void*))threadTimeOut, NULL);
    pthread_detach(handler);
  } 
  mUnLock(&mutexFireOn);
}


//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Handler SIGQUIT ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void handlerSIGQUIT(int sig){
  #ifdef DEBUG
    printf("(ThreadVaisseau %ld) Fin ThreadVaisseau\n",getTid()); 
  #endif
  
  mLock(&mutexGrille);
    EffaceCarre(NB_LIGNE-1, colVaisseau); 
    setTab(NB_LIGNE-1,colVaisseau, VIDE,0);
  mUnLock(&mutexGrille);
  pthread_exit(NULL);
}
