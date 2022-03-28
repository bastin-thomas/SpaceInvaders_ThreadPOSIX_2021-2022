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
#define DELAI 1000

int colVaisseau = 15;
bool fireOn = true;

int delai = DELAI;

int nbAliens = 24;
int ligneHaut = 2;        //Ligne Haut
int colonneGauche = 8;    //ColonneGauche
int ligneBas = 8;         //Ligne Bas
int colonneDroite = 18;   //ColonneDroite

///////////////////////////////////       Mutex      //////////////////////////////////////////////
pthread_mutex_t mutexGrille = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexFireOn = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexFlotteAliens = PTHREAD_MUTEX_INITIALIZER;

/////////////////////////////////// Variables Conditions //////////////////////////////////////////


/////////////////////////////////// Fonction Thread ///////////////////////////////////////////////
void threadVaisseau();
void threadEvent();
void TermEvent();
void threadMissile(void *);
void threadTimeOut();
void threadInvader();
void threadFlotteAliens();

/////////////////////////////////// Fonction Perso ////////////////////////////////////////////////
void PoseFlotte();
void ShiftDroiteFlotte();
void ShiftGaucheFlotte();
void ShiftBasFlotte();
void RechercheBordure();
void freeAlien();
void RestoreShield();
void MajLevel(int level);
void AfficheTab();

/////////////////////////////////// Handler Signaux ///////////////////////////////////////////////
void handlerSIGUSR1(int sig);
void handlerSIGUSR2(int sig);
void handlerSIGHUP(int sig);
void handlerSIGINT(int sig);
void handlerSIGQUIT(int sig);



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
  RestoreShield();




  // Initialisation des mutex et variables de condition
  if((error = mInitDef(&mutexGrille)) != 0){
    printf("(MAIN %ld) Erreur Initialisation mutexGrille: %d\n",getTid(),error); 
    fflush(stdout);
  }

  if((error = mInitDef(&mutexFireOn)) != 0){
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

  pthread_exit(NULL);
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
  sigdelset(&masque, SIGQUIT);
  sigprocmask(SIG_SETMASK, &masque, NULL);

  printf("(ThreadVaisseau %ld) Init du Vaisseau\n",getTid());  
  mLock(&mutexGrille);
  if (tab[NB_LIGNE-1][colVaisseau].type == VIDE) 
  {
    setTab(NB_LIGNE-1, colVaisseau, VAISSEAU, getTid());
    DessineVaisseau(NB_LIGNE-1, colVaisseau);
  }
  else{
    printf("(ThreadVaisseau %ld) ERROR Init du Vaisseau: Case Non Vide\n",getTid()); 
  } 
  mUnLock(&mutexGrille);
  
  //Attente Signal
  while(1){
    pause();
  }

  printf("(ThreadVaisseau %ld) Fermeture du Thread\n",getTid()); fflush(stdout);
  pthread_exit(NULL);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Handler Signaux /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
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

void handlerSIGQUIT(int sig){
  printf("(ThreadVaisseau %ld) Fin ThreadVaisseau\n",getTid()); 
  mLock(&mutexGrille);
    EffaceCarre(NB_LIGNE-1, colVaisseau); 
    setTab(NB_LIGNE-1,colVaisseau, VIDE,0);
  mUnLock(&mutexGrille);
  pthread_exit(NULL);
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
  sigdelset(&masque, SIGINT);
  sigprocmask(SIG_SETMASK, &masque, NULL);

  //Entrée dans la grille de jeu:
  switch(tab[Position.L][Position.C].type){ 
    //Si case Vide apparition du missile
    case VIDE:  
                mLock(&mutexGrille);
                  setTab(Position.L, Position.C, MISSILE, getTid());
                  DessineMissile(Position.L, Position.C);
                mUnLock(&mutexGrille);
            break;
    
    //Si case Bouclier1 Transformation en Bouclier2
    case BOUCLIER1:
                mLock(&mutexGrille);
                  setTab(Position.L, Position.C, BOUCLIER2, 0);
                  DessineBouclier(Position.L, Position.C, 2);
                mUnLock(&mutexGrille);
                pthread_exit(NULL);
              break;
    //Si case Bouclier2 Supression Bouclier2
    case BOUCLIER2:
                mLock(&mutexGrille);
                  setTab(Position.L, Position.C, VIDE, 0);
                  EffaceCarre(Position.L, Position.C);
                mUnLock(&mutexGrille);
                pthread_exit(NULL);
              break;
  }
  
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
            RechercheBordure();
            //AfficheTab();
            printf("(threadMissile %ld) Un Aliens est Mort, Il reste %d Aliens\n",getTid(), nbAliens); fflush(stdout);
          
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
      mUnLock(&mutexGrille);  
      mUnLock(&mutexFlotteAliens);
  }

  //Supression du missile si il est arrivé tout en haut
  mLock(&mutexGrille);
    setTab(Position.L, Position.C, VIDE, 0);
    EffaceCarre(Position.L, Position.C);
  mUnLock(&mutexGrille);

  pthread_exit(NULL);
}
////////////////////////////////// Handler Signaux //////////////////////////////////////////////
void handlerSIGINT(int sig){
  mUnLock(&mutexFlotteAliens);
  mUnLock(&mutexGrille);
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
      printf("\n(ThreadInvader %ld)... Level: %d Délai : %d...\n\n",getTid(),level, delai);
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
        printf("(ThreadInvader %ld)... PERDU ...\n",getTid());
        pthread_kill(tab[NB_LIGNE-1][colVaisseau].tid,SIGQUIT);

        mUnLock(&mutexGrille);
        mUnLock(&mutexFlotteAliens);
        break;
  }while(1);
}

void freeAlien(){
  for(int i = 0 ; i < NB_LIGNE-2 ; i++){
    for(int j = 8 ; j < NB_COLONNE ; j++){
      if(tab[i][j].type == ALIEN){
        setTab(i,j,VIDE,0);
        EffaceCarre(i,j);
      }

      if(tab[i][j].type == MISSILE){
        pthread_kill(tab[i][j].tid, SIGINT);
        setTab(i,j,VIDE,0);
        EffaceCarre(i,j);
      }
    }
  }
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

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  Thread Flotte Aliens //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadFlotteAliens(){
  printf("(ThreadFlotteAliens %ld) Pose de la Flotte\n",getTid()); fflush(stdout);
  //Init Flotte    
  PoseFlotte();

  //Deplacement Flotte
  while(1){
    while(colonneDroite < NB_COLONNE-1)
    {
      Attente(delai);
      mLock(&mutexFlotteAliens);
        mLock(&mutexGrille);
          
          ShiftDroiteFlotte(); 
          RechercheBordure(); //Vérification des nouvelles bordures.
          AfficheTab();

        mUnLock(&mutexGrille);

        if(nbAliens == 0)
        {
          printf("(ThreadFlotteAliens %ld) Tout les Aliens sont Morts\n",getTid()); fflush(stdout);
          mUnLock(&mutexFlotteAliens);
          pthread_exit(NULL);
        }
      mUnLock(&mutexFlotteAliens);
    }

    while(colonneGauche > 8 )
    {
      Attente(delai);
      mLock(&mutexFlotteAliens);
        mLock(&mutexGrille);
          
          ShiftGaucheFlotte();
          RechercheBordure(); //Vérification des nouvelles bordures.
          AfficheTab();

        mUnLock(&mutexGrille);

        if(nbAliens == 0)
        {
          printf("(ThreadFlotteAliens %ld) Tout les Aliens sont Morts\n",getTid()); fflush(stdout);
          mUnLock(&mutexFlotteAliens);
          pthread_exit(NULL);
        }
      mUnLock(&mutexFlotteAliens);
    }

    Attente(delai);

    mLock(&mutexFlotteAliens);
      mLock(&mutexGrille);
        
        ShiftBasFlotte();
        RechercheBordure(); //Vérification des nouvelles bordures.
        AfficheTab();
        
      mUnLock(&mutexGrille);

      if(nbAliens == 0)
      {
        printf("(ThreadFlotteAliens %ld) Tout les Aliens sont Morts\n",getTid()); fflush(stdout);
        mUnLock(&mutexFlotteAliens);
        pthread_exit(NULL);
      }

      if(ligneBas == NB_LIGNE-3){
        printf("(ThreadFlotteAliens %ld) %d Aliens ont envahi la terre !!!\n",getTid(), nbAliens); fflush(stdout);
        mUnLock(&mutexFlotteAliens);
        pthread_exit(NULL);
      }
    mUnLock(&mutexFlotteAliens);
  }
  printf("(ThreadFlotteAliens %ld) Erreur Exit\n",getTid()); fflush(stdout);
  pthread_exit(NULL);
}



///////////////////// Fonction Perso Flotte ////////////////////
void PoseFlotte(){
  mLock(&mutexFlotteAliens);
    mLock(&mutexGrille);   
      nbAliens = 24;
      ligneHaut = 2;        //Ligne Haut
      colonneGauche = 8;    //ColonneGauche
      ligneBas = 8;         //Ligne Bas
      colonneDroite = 18;   //ColonneDroite

      for(int i = ligneHaut ; i <= ligneBas ; i += 2)
      {
        for(int j = colonneGauche ; j <= colonneDroite ; j += 2)
        {
          setTab(i,j, ALIEN, getpid());
          DessineAlien(i,j);
        }
      }
    mUnLock(&mutexGrille);
  mUnLock(&mutexFlotteAliens);
}


void ShiftDroiteFlotte(){
    for(int i = ligneHaut ; i <= ligneBas ; i +=2){
      for(int j = colonneGauche ; j <= colonneDroite ; j +=2){
        if(tab[i][j].type == ALIEN)
        {  
          //Si on tombe sur une case vide, on se déplace juste
          if(tab[i][j+1].type == VIDE)
          { 
            EffaceCarre(i,j);
            setTab(i,j, VIDE, 0);
            setTab(i,j+1, ALIEN, getTid());
            DessineAlien(i,j+1);
            continue;
          }

          //Si on tombe sur un missile, on 'éteint' le thread missile correspondant, et on le supprime de l'interface
          if(tab[i][j+1].type == MISSILE)
          {
            pthread_kill(tab[i][j+1].tid, SIGINT);
            EffaceCarre(i,j);
            EffaceCarre(i,j+1);
            setTab(i,j, VIDE, 0);
            setTab(i,j+1, VIDE, 0);
            nbAliens --;

            printf("(ThreadFlotteAliens %ld) Un Aliens est Mort, Il reste %d Aliens\n",getTid(), nbAliens); fflush(stdout);
          }          
        }
      }
    }
  colonneGauche++;
  colonneDroite++;  
}


void ShiftGaucheFlotte(){

    for(int i = ligneHaut ; i <= ligneBas ; i +=2){
      for(int j = colonneGauche ; j <= colonneDroite ; j +=2){
        if(tab[i][j].type == ALIEN)
        {  
          //Si on tombe sur une case vide, on se déplace juste
          if(tab[i][j-1].type == VIDE)
          { 
            EffaceCarre(i,j);
            setTab(i,j, VIDE, 0);
            setTab(i,j-1, ALIEN, getTid());
            DessineAlien(i,j-1);
            continue;
          }

          //Si on tombe sur un missile, on 'éteint' le thread missile correspondant, et on le supprime de l'interface
          if(tab[i][j-1].type == MISSILE)
          {
            pthread_kill(tab[i][j-1].tid, SIGINT);

            EffaceCarre(i,j);
            EffaceCarre(i,j-1);
            setTab(i,j, VIDE, 0);
            setTab(i,j-1, VIDE, 0);
            nbAliens --;

            printf("(ThreadFlotteAliens %ld) Un Aliens est Mort, Il reste %d Aliens\n",getTid(), nbAliens); fflush(stdout);
          }
        }
      }
    }
  colonneGauche--;
  colonneDroite--;
}

void ShiftBasFlotte(){
    for(int i = ligneHaut ; i <= ligneBas ; i +=2){
      for(int j = colonneGauche ; j <= colonneDroite ; j +=2){
        if(tab[i][j].type == ALIEN)
        {  
          //Si on tombe sur une case vide, on se déplace juste
          if(tab[i+1][j].type == VIDE)
          { 
            EffaceCarre(i,j);
            setTab(i,j, VIDE, 0);
            setTab(i+1,j, ALIEN, getTid());
            DessineAlien(i+1,j);
            continue;
          } 

          //Si on tombe sur un missile, on 'éteint' le thread missile correspondant, et on le supprime de l'interface
          if(tab[i+1][j].type == MISSILE)
          {
            pthread_kill(tab[i+1][j].tid, SIGINT);

            EffaceCarre(i,j);
            EffaceCarre(i+1,j);
            setTab(i,j, VIDE, 0);
            setTab(i+1,j, VIDE, 0);
            nbAliens --;

            printf("(ThreadFlotteAliens %ld) Un Aliens est Mort, Il reste %d Aliens\n",getTid(), nbAliens); fflush(stdout);
          }
        }
      }
    }
  ligneBas++;
  ligneHaut++;
}

void RechercheBordure(){
  bool test = true;

  if(nbAliens == 0) return;

  while(1){
      //Si les bords externe de lignehaut sont vide, on regarde si toutes les cases sont vide, 
        test = true;
        for(int i = colonneGauche ; i<= colonneDroite ; i++){
          if(tab[ligneHaut][i].type == ALIEN){
            test = false;
            break;
          }
        }

        //si oui, on retire une ligne, et on réentre dans la boucle
        if(test){
          ligneHaut+=2;
          continue;
        }


      //Si les bords externe de ligneBas sont vide, on regarde si toutes les cases sont vide, 
        test = true;
        for(int i = colonneGauche ; i<= colonneDroite ; i++){
          if(tab[ligneBas][i].type == ALIEN){
            test = false;
            break;
          }
        }

        //si oui, on retire une ligne, et on réentre dans la boucle
        if(test){
          ligneBas-=2;
          continue;
        }
      
      //Si non on passe aux cas suivant.

      //Si les bords externe de colonneGauche sont vide, on regarde si toutes les cases du vecteur colonneGauche sont vide, 
        test = true;
        for(int i = ligneHaut ; i<= ligneBas ; i++){
          if(tab[i][colonneGauche].type == ALIEN){
            test = false;
            break;
          }
        }

        //si oui, on retire une colonne, et on réentre dans la boucle
        if(test){
          colonneGauche+=2;
          continue;
        }
      
      //Si non on passe aux cas suivant.

      //Si les bords externe de colonneDroite sont vide, on regarde si toutes les cases du vecteur colonneGauche sont vide,
        test = true;
        for(int i = ligneHaut ; i<= ligneBas ; i++){
          if(tab[i][colonneDroite].type == ALIEN){
            test = false;
            break;
          }
        }
        
        if(test){
          //si oui, on retire une colonne, et on réentre dans la boucle
          colonneDroite-=2;
          continue;
        }
      //Si aucun cas n'est satisfait, on quitte la boucle.
      break;
  }
}

