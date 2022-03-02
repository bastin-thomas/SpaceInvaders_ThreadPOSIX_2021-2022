#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "GrilleSDL.h"
#include "Ressources.h"
#include "mylibthread_POSIX.h"

// Dimensions de la grille de jeu
#define NB_LIGNE   18
#define NB_COLONNE 23 

// Direction de mouvement
#define GAUCHE       10
#define DROITE       11
#define BAS          12

// Intervenants du jeu (type)
#define VIDE        0
#define VAISSEAU    1
#define MISSILE     2
#define ALIEN       3
#define BOMBE       4
#define BOUCLIER1   5
#define BOUCLIER2   6
#define AMIRAL      7

typedef struct
{
  int type;
  pthread_t tid;
} S_CASE;

typedef struct 
{
  int L;
  int C;
} S_POSITION;

S_CASE tab[NB_LIGNE][NB_COLONNE];

void Attente(int milli);
void setTab(int l,int c,int type=VIDE,pthread_t tid=0);
///////////////////////////////////////////////////////////////////////////////////////////////////
void Attente(int milli)
{
  struct timespec del;
  del.tv_sec = milli/1000;
  del.tv_nsec = (milli%1000)*1000000;
  nanosleep(&del,NULL);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void setTab(int l,int c,int type,pthread_t tid)
{
  tab[l][c].type = type;
  tab[l][c].tid = tid;
}

/////////////////////////////////// Variables Globales ////////////////////////////////////////////
int colVaisseau = 15;
bool fireOn = true;

///////////////////////////////////       Mutex      //////////////////////////////////////////////
pthread_mutex_t mutexGrille = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexFireOn = PTHREAD_MUTEX_INITIALIZER;
/////////////////////////////////// Variables Conditions //////////////////////////////////////////


/////////////////////////////////// Fonction Thread ///////////////////////////////////////////////
void threadVaisseau();
void threadEvent();
void TermEvent();
void threadMissile(void *);
void threadTimeOut();

/////////////////////////////////// Fonction Perso ////////////////////////////////////////////////


/////////////////////////////////// Handler Signaux ///////////////////////////////////////////////
void handlerSIGUSR1(int sig);
void handlerSIGUSR2(int sig);
void handlerSIGHUP(int sig);



///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Thread Main ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc,char* argv[])
{
  pthread_t handler;
  int error;
  sigset_t masque;
 
  srand((unsigned)time(NULL));

  // Ouverture de la fenetre graphique
  printf("(MAIN %ld) Ouverture de la fenetre graphique\n",getTid()); fflush(stdout);
  if (OuvertureFenetreGraphique() < 0)
  {
    printf("Erreur de OuvrirGrilleSDL\n");
    fflush(stdout);
    exit(1);
  }

  // Initialisation de tab
  for (int l=0 ; l<NB_LIGNE ; l++)
    for (int c=0 ; c<NB_COLONNE ; c++)
      setTab(l,c);

  // Initialisation des boucliers
  setTab(NB_LIGNE-2,11,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,11,1);
  setTab(NB_LIGNE-2,12,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,12,1);
  setTab(NB_LIGNE-2,13,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,13,1);
  setTab(NB_LIGNE-2,17,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,17,1);
  setTab(NB_LIGNE-2,18,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,18,1);
  setTab(NB_LIGNE-2,19,BOUCLIER1,0);  DessineBouclier(NB_LIGNE-2,19,1);




  // Initialisation des mutex et variables de condition
  if(( error = mInitDef(&mutexGrille)) != 0){
    printf("(MAIN %ld) Erreur Initialisation mutexGrille: %d\n",getTid(),error); 
    fflush(stdout);
  }

  if(( error = mInitDef(&mutexFireOn)) != 0){
    printf("(MAIN %ld) Erreur Initialisation mutexGrille: %d\n",getTid(),error); 
    fflush(stdout);
  }

  // Masquage de tout les signaux (pour éviter d'avoir des threads qui reçoivent les mauvais)
  sigfillset(&masque);
  sigprocmask(SIG_SETMASK, &masque, NULL);
  

  // Armement des signaux
  struct sigaction Action;

  // Armement SIGUSR1
  Action.sa_handler = handlerSIGUSR1;
  sigfillset(&Action.sa_mask);
  Action.sa_flags = SA_RESTART;
  if(sigaction(SIGUSR1, &Action, NULL) == -1) printf("(MAIN %ld) Erreur de Sigaction SIGUSR1\n",getTid());
  
  // Armement SIGUSR2
  Action.sa_handler = handlerSIGUSR2;
  sigfillset(&Action.sa_mask);
  Action.sa_flags = SA_RESTART;
  if(sigaction(SIGUSR2, &Action, NULL) == -1) printf("(MAIN %ld) Erreur de Sigaction SIGUSR2\n",getTid());

  // Armement SIGHUP
  Action.sa_handler = handlerSIGHUP;
  sigfillset(&Action.sa_mask);
  Action.sa_flags = SA_RESTART;
  if(sigaction(SIGHUP, &Action, NULL) == -1) printf("(MAIN %ld) Erreur de Sigaction SIGHUP\n",getTid());


  // Creation des threads
  //Lancement ThreadVaisseau:
  pthread_create(&handler, NULL, (void* (*)(void*))threadVaisseau, NULL);
  pthread_detach(handler);

  //Lancement ThreadEvent:
  pthread_create(&handler, NULL, (void* (*)(void*))threadEvent, NULL);
  pthread_detach(handler);



  pthread_exit(NULL);
}





///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Thread Vaisseau /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadVaisseau(){
  printf("(ThreadVaisseau %ld) Lancement du Thread\n",getTid());
  // Démasquage des signaux SIGUSR1, SIGUSR2, SIGHUP
  sigset_t masque;
  sigfillset(&masque);
  sigdelset(&masque, SIGUSR1);
  sigdelset(&masque, SIGUSR2);
  sigdelset(&masque, SIGHUP);
  sigprocmask(SIG_SETMASK, &masque, NULL);


  printf("(ThreadVaisseau %ld) Init du Vaisseau\n",getTid());  
  if (tab[NB_LIGNE-1][colVaisseau].type == VIDE) 
  {
    mLock(&mutexGrille);
      setTab(NB_LIGNE-1, colVaisseau, VAISSEAU, getTid());
    mUnLock(&mutexGrille);
    DessineVaisseau(NB_LIGNE-1, colVaisseau);
  }
  else{
    printf("(ThreadVaisseau %ld) ERROR Init du Vaisseau: Case Non Vide\n",getTid()); 
  } 
  
  //Attente Touche
  while(1){
    pause();
  }

  printf("(ThreadVaisseau %ld) Fermeture du Thread\n",getTid());
  pthread_exit(NULL);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Handler Signaux /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void handlerSIGUSR1(int sig){
  printf("(ThreadVaisseau %ld) Déplacement Droite\n",getTid());

  if(colVaisseau == NB_COLONNE - 1) return; //Si on est au bord de la colonne on skip l'instruction

  if (tab[NB_LIGNE-1][colVaisseau+1].type == VIDE) 
  {
    //Supression du sprite actuelle, et ajout du sprite a droite comme étant le vaisseau.
    EffaceCarre(NB_LIGNE-1, colVaisseau);
    mLock(&mutexGrille);
      setTab(NB_LIGNE-1, colVaisseau, VIDE, 0);
      setTab(NB_LIGNE-1, colVaisseau+1, VAISSEAU, getTid());
    mUnLock(&mutexGrille);
    DessineVaisseau(NB_LIGNE-1, colVaisseau+1);

    //Update Emplacement Vaisseau connus de façons Global
    colVaisseau++;
  }
}

void handlerSIGUSR2(int sig){
  printf("(ThreadVaisseau %ld) Déplacement Gauche\n",getTid());

  if(colVaisseau == 8 ) return; //Si on est au bord de la colonne on skip l'instruction

  if (tab[NB_LIGNE-1][colVaisseau-1].type == VIDE) 
  {
    //Supression du sprite actuelle, et ajout du sprite a gauche comme étant le vaisseau.
    EffaceCarre(NB_LIGNE-1, colVaisseau);
    mLock(&mutexGrille);
      setTab(NB_LIGNE-1, colVaisseau, VIDE, 0);
      setTab(NB_LIGNE-1, colVaisseau-1, VAISSEAU, getTid());
    mUnLock(&mutexGrille);
    DessineVaisseau(NB_LIGNE-1, colVaisseau-1);

    //Update Emplacement Vaisseau connus de façons Global
    colVaisseau--;
  }
}

void handlerSIGHUP(int sig){
  printf("(ThreadVaisseau %ld) Envoi Missile\n",getTid());
  pthread_t handler;
  S_POSITION* Position = new S_POSITION;
  Position->L = NB_LIGNE-2;
  Position->C = colVaisseau;

  if(fireOn == true){
    //Lancement ThreadEvent:
    pthread_create(&handler, NULL, (void* (*)(void*))threadMissile, (void*) Position);
    pthread_detach(handler);

    mLock(&mutexFireOn);
      fireOn = false;
    mUnLock(&mutexFireOn);
    
    pthread_create(&handler, NULL, (void* (*)(void*))threadTimeOut, NULL);
    pthread_detach(handler);
  }  
}





///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  Thread Event ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadEvent(){
  pthread_cleanup_push( (void(*)(void*)) TermEvent, NULL);
  printf("(ThreadEvent %ld) Creation ThreadEvent\n",getTid());  
  EVENT_GRILLE_SDL event;
  bool ok = false;
  while(!ok)
  {
    event = ReadEvent();
    if (event.type == CROIX) ok = true;
    if (event.type == CLIC_GAUCHE)
    {
      /*
      if (tab[event.ligne][event.colonne].type == VIDE) 
      {
        DessineVaisseau(event.ligne,event.colonne);
        setTab(event.ligne,event.colonne,VAISSEAU,getTid());
      }
      */
    }
    if (event.type == CLAVIER)
    {
      switch(event.touche){
        case KEY_RIGHT:
                      //printf("(ThreadEvent %ld) Flèche droite enfoncée\n",getTid());
                      kill(getpid(),SIGUSR1);
                    break;

        case KEY_LEFT:
                      //printf("(ThreadEvent %ld) Flèche Gauche enfoncée\n",getTid());
                      kill(getpid(),SIGUSR2);
                    break;

        case KEY_SPACE:
                      //printf("(ThreadEvent %ld) Espace enfoncée\n",getTid());
                      kill(getpid(),SIGHUP);
                    break;
      }
    }
  }
  pthread_cleanup_pop(1);
}

void TermEvent(){
  // Fermeture de la fenetre
  printf("(ThreadEvent %ld) Fermeture de la fenetre graphique...",getTid()); fflush(stdout);
  FermetureFenetreGraphique();
  printf("OK\n");
  exit(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  Thread Missile /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadMissile(void* pos){
  S_POSITION Position = *((S_POSITION*)pos);
  free(pos);
  sigset_t masque;
  
  sigfillset(&masque);
  sigprocmask(SIG_SETMASK, &masque, NULL);

  //Entrée dans la grille de jeu:
  switch(tab[Position.L][Position.C].type){ 
    //Si case Vide apparition du missile
    case VIDE:  
                mLock(&mutexGrille);
                  setTab(Position.L, Position.C, MISSILE, getTid());
                mUnLock(&mutexGrille);
                DessineMissile(Position.L, Position.C);
            break;
    
    //Si case Bouclier1 Transformation en Bouclier2
    case BOUCLIER1:
                mLock(&mutexGrille);
                  setTab(Position.L, Position.C, BOUCLIER2, 0);
                mUnLock(&mutexGrille);
                DessineBouclier(Position.L, Position.C, 2);
                pthread_exit(NULL);
              break;
    //Si case Bouclier2 Supression Bouclier2
    case BOUCLIER2:
                mLock(&mutexGrille);
                  setTab(Position.L, Position.C, VIDE, 0);
                mUnLock(&mutexGrille);
                EffaceCarre(Position.L, Position.C);
                pthread_exit(NULL);
              break;
  }
  
  while(Position.L > 0){

    Attente(80);

    if(tab[Position.L-1][Position.C].type == VIDE){  //Case si la case devant est vide, on avance de 1
      EffaceCarre(Position.L, Position.C);
      mLock(&mutexGrille);
        setTab(Position.L, Position.C, VIDE, 0);
        Position.L--;
        setTab(Position.L, Position.C, MISSILE, getTid());
      mUnLock(&mutexGrille);
      DessineMissile(Position.L, Position.C);
    }
    else{
      //Si il y a quelques  chose devant nous, alors la chose devant meurt. et on sort
      mLock(&mutexGrille);
        setTab(Position.L-1, Position.C, VIDE, 0);
      mUnLock(&mutexGrille);
      EffaceCarre(Position.L-1, Position.C);
      break;
    }
  }

  //Supression du missile si il est arrivé tout en haut, ou qu'il est tombé sur un ennemi
  mLock(&mutexGrille);
    setTab(Position.L, Position.C, VIDE, 0);
  mUnLock(&mutexGrille);
  EffaceCarre(Position.L, Position.C);

  pthread_exit(NULL);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  Thread TimeOut /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadTimeOut(){
  Attente(600);
  mLock(&mutexFireOn);
    fireOn = true;
  mUnLock(&mutexFireOn);
  pthread_exit(NULL);
}
