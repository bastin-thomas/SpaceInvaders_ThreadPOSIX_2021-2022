#include "Define.h"

/////////////////////////////////// Variables Globales ////////////////////////////////////////////

S_CASE tab[NB_LIGNE][NB_COLONNE];

int colVaisseau = 15;
bool fireOn = true;

int delai = DELAI;

int nbAliens = 24;
int ligneHaut = 2;        //Ligne Haut
int colonneGauche = 8;    //ColonneGauche
int ligneBas = 8;         //Ligne Bas
int colonneDroite = 18;   //ColonneDroite

int Score = 0;
bool MajScore;

int nbVies = 3;

int PlacementAmiral;
bool AmiralExist;

///////////////////////////////////////////  Mutex  ///////////////////////////////////////////////
pthread_mutex_t mutexGrille;
pthread_mutex_t mutexFireOn;
pthread_mutex_t mutexFlotteAliens;
pthread_mutex_t mutexScore;
pthread_mutex_t mutexVies;


/////////////////////////////////// Variables Conditions //////////////////////////////////////////
pthread_cond_t condScore;
pthread_cond_t condVies;
pthread_cond_t condFlotteAliens;

/////////////////////////////////// Fonction Thread ///////////////////////////////////////////////
#include "./threadVaisseau/threadVaisseau.h"
#include "./threadEvent/threadEvent.h"
#include "./threadMissile/threadMissile.h"
#include "./threadTimeOut/threadTimeOut.h"
#include "./threadInvader/threadInvader.h"
#include "./threadFlotteAliens/threadFlotteAliens.h"
#include "./threadScore/threadScore.h"
#include "./threadBombe/threadBombe.h"
#include "./threadVaisseauAmiral/threadVaisseauAmiral.h"


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
  #ifdef DEBUG
    printf("(MAIN %ld) Ouverture de la fenetre graphique\n",getTid()); fflush(stdout);
  #endif
  
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

  // Initialisation des boucliers:
  RestoreShield();

  //Initialisation des Vies:
  DessineVaisseau(16,3);
  DessineVaisseau(16,4);

  // Initialisation des mutex
  if((error = mInitDef(&mutexGrille)) != 0){
    printf("(MAIN %ld) Erreur Initialisation mutexGrille: %d\n",getTid(),error); 
    fflush(stdout);
    exit(2);
  }

  if((error = mInitDef(&mutexFireOn)) != 0){
    printf("(MAIN %ld) Erreur Initialisation mutexFireOn: %d\n",getTid(),error); 
    fflush(stdout);
    exit(2);
  }

  if((error = mInitDef(&mutexFlotteAliens)) != 0){
    printf("(MAIN %ld) Erreur Initialisation mutexFlotteAliens: %d\n",getTid(),error); 
    fflush(stdout);
    exit(2);
  }

  if((error = mInitDef(&mutexScore)) != 0){
    printf("(MAIN %ld) Erreur Initialisation mutexVies: %d\n",getTid(),error); 
    fflush(stdout);
    exit(2);
  }

  if((error = mInitDef(&mutexVies)) != 0){
    printf("(MAIN %ld) Erreur Initialisation mutexVies: %d\n",getTid(),error); 
    fflush(stdout);
    exit(2);
  }


  // Init Var Condition
  if((error = pthread_cond_init(&condScore, NULL)) != 0){
    printf("(MAIN %ld) Erreur Initialisation condScore: %d\n",getTid(),error); 
    fflush(stdout);
    exit(2);
  }

  if((error = pthread_cond_init(&condVies, NULL)) != 0){
    printf("(MAIN %ld) Erreur Initialisation condVies: %d\n",getTid(),error); 
    fflush(stdout);
    exit(2);
  }

  if((error = pthread_cond_init(&condFlotteAliens, NULL)) != 0){
    printf("(MAIN %ld) Erreur Initialisation condVies: %d\n",getTid(),error); 
    fflush(stdout);
    exit(2);
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

  // Armement SIGINT
  Action.sa_handler = handlerSIGINT;
  sigfillset(&Action.sa_mask);
  Action.sa_flags = SA_RESTART;
  if(sigaction(SIGINT, &Action, NULL) == -1) printf("(MAIN %ld) Erreur de Sigaction SIGINT\n",getTid());

  // Armement SIGQUIT
  Action.sa_handler = handlerSIGQUIT;
  sigfillset(&Action.sa_mask);
  Action.sa_flags = SA_RESTART;
  if(sigaction(SIGQUIT, &Action, NULL) == -1) printf("(MAIN %ld) Erreur de Sigaction SIGQUIT\n",getTid());

  // Armement SIGALRM
  Action.sa_handler = handlerSIGALRM;
  sigfillset(&Action.sa_mask);
  Action.sa_flags = SA_RESTART;
  if(sigaction(SIGALRM, &Action, NULL) == -1) printf("(MAIN %ld) Erreur de Sigaction SIGALRM\n",getTid());

  // Armement SIGCHLD
  Action.sa_handler = handlerSIGCHLD;
  sigfillset(&Action.sa_mask);
  Action.sa_flags = SA_RESTART;
  if(sigaction(SIGCHLD, &Action, NULL) == -1) printf("(MAIN %ld) Erreur de Sigaction SIGCHLD\n",getTid());




  // Creation des threads
  //Lancement ThreadVaisseau:
  pthread_create(&handler, NULL, (void* (*)(void*))threadVaisseau, NULL);
  pthread_detach(handler);

  //Lancement ThreadEvent:
  pthread_create(&handler, NULL, (void* (*)(void*))threadEvent, NULL);
  pthread_detach(handler);

  //Lancement ThreadInvader:
  pthread_create(&handler, NULL, (void* (*)(void*))threadInvader, NULL);
  pthread_detach(handler);

  //Lancement ThreadScore:
  pthread_create(&handler, NULL, (void* (*)(void*))threadScore, NULL);


  //Lancement Thread Vaisseau Amiral:
  pthread_create(&handler, NULL, (void* (*)(void*))threadVaisseauAmiral, NULL);



  while(1)
  {
    mLock(&mutexVies);
    while(nbVies>0)
    {
      pthread_cond_wait(&condVies, &mutexVies);
      
      if(nbVies != 0){
        //Lancement ThreadVaisseau:
        pthread_create(&handler, NULL, (void* (*)(void*))threadVaisseau, NULL);
        pthread_detach(handler);
      }
    }
    if(nbVies == 0) break;
    
    mUnLock(&mutexVies);
  }

  mLock(&mutexGrille);
    //Suppression Grille Jeux
    freeAlien();

    //Supressions Des Boucliers et des bombes restantes sur la ligne des boucliers
    for(int i = 8 ; i<NB_COLONNE ; i++){
      if(tab[NB_LIGNE-2][i].type == BOUCLIER1 || tab[NB_LIGNE-2][i].type == BOUCLIER2 || tab[NB_LIGNE-2][i].type == BOMBE){
        setTab(NB_LIGNE-2, i, VIDE, 0);
        EffaceCarre(NB_LIGNE-2, i);
      }
    }

    //Supressions des Bombes Restante sur la ligne du Vaisseau
    for(int i = 8 ; i<NB_COLONNE ; i++){
      if(tab[NB_LIGNE-1][i].type == BOMBE){
        setTab(NB_LIGNE-1, i, VIDE, 0);
        EffaceCarre(NB_LIGNE-1, i);
      }
    }


    //Ajout d'un Signal ?

    //Supression Vaisseau Amiral:
    setTab(LIGNEAMIRAL, PlacementAmiral, VIDE, 0);
    setTab(LIGNEAMIRAL, PlacementAmiral+1, VIDE, 0);
    EffaceCarre(LIGNEAMIRAL, PlacementAmiral);
    EffaceCarre(LIGNEAMIRAL, PlacementAmiral+1);


    #ifdef DEBUG
      printf("(MAIN %ld)... GAME OVER ...\n",getTid());
      printf("(MAIN %ld) Fin du ThreadMain\n",getTid());
    #endif

    DessineGameOver(6,11);
  mLock(&mutexGrille);

  pthread_exit(NULL);
}




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
  for(int i = 0; i<NB_LIGNE; i++){
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
