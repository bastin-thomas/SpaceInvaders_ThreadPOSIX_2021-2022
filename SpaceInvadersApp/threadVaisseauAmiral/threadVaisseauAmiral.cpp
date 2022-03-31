#include "threadVaisseauAmiral.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  Thread Vaisseau Amiral  /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadVaisseauAmiral(){
  int pastNbAliens;
  int random;

    // Démasquage des signaux SIGUSR1, SIGUSR2, SIGHUP
    sigset_t masque;
    sigfillset(&masque);
    sigdelset(&masque, SIGALRM);
    sigdelset(&masque, SIGCHLD);
    sigprocmask(SIG_SETMASK, &masque, NULL);

  #ifdef DEBUG
    printf("(threadVaisseauAmiral %ld) Démarage threadVaisseauAmiral\n",getTid());
  #endif

  while(1){
      
    mLock(&mutexFlotteAliens);
    if(pastNbAliens == nbAliens) //Si le Nombre D'alien est le même on n'entre pas dans
    {  
      mUnLock(&mutexFlotteAliens);
      continue;
    }
    mUnLock(&mutexFlotteAliens);


    mLock(&mutexFlotteAliens);
      while(nbAliens%6 != 0 || nbAliens == 0)
      {
        pthread_cond_wait(&condFlotteAliens, &mutexFlotteAliens);
      }
      pastNbAliens = nbAliens;

      #ifdef DEBUG
        printf("(threadVaisseauAmiral %ld) Fin Attente, Creation Amiral\n",getTid());
      #endif

      mLock(&mutexGrille);
        PlacementAmiral = ((rand()%14)+8);

        DessineVaisseauAmiral(LIGNEAMIRAL, PlacementAmiral);

        setTab(LIGNEAMIRAL, PlacementAmiral, AMIRAL, getTid());
        setTab(LIGNEAMIRAL, PlacementAmiral+1, AMIRAL, getTid());

        AmiralExist = true;

        random = (rand()%9) + 4;
        alarm(random);
        #ifdef DEBUG
          printf("(threadVaisseauAmiral %ld) Alarm(%d)\n",getTid(),random);
        #endif

      mUnLock(&mutexGrille);
    mUnLock(&mutexFlotteAliens);


    #ifdef DEBUG
      printf("(threadVaisseauAmiral %ld) Deplacement Amiral\n",getTid());
    #endif
    Deplacement(); //Deplacement vers droite ou gauche    
  }
  pthread_exit(NULL);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Fct Gère le Déplacement /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void Deplacement(){
  int Direction = rand()%2;

  //Premier Déplacement
  if(Direction == 0) DeplacementGauche();
  if(Direction != 0) DeplacementDroite();

  #ifdef DEBUG
    printf("(threadVaisseauAmiral %ld) PlacementAmiral: %d\n",getTid(), PlacementAmiral);
  #endif

  while(AmiralExist){
    //Si après Premier Déplacement on est à Gauche, on repart vers la Droite
    if(PlacementAmiral == 8){
      DeplacementDroite();
    }

    //Si après Premier Déplacement on est à Droite, on repart vers la Gauche
    if(PlacementAmiral == NB_COLONNE-2){
      DeplacementGauche();
    }
  }

  //Lorsque la Variable Global AmiralExist passe à False, on sort des boucles et supprime le vaisseau.
  mLock(&mutexGrille);
    //Destruction arrivée en fin de Grille
    setTab(LIGNEAMIRAL, PlacementAmiral, VIDE, 0);
    setTab(LIGNEAMIRAL, PlacementAmiral+1, VIDE, 0);
    EffaceCarre(LIGNEAMIRAL, PlacementAmiral);
    EffaceCarre(LIGNEAMIRAL, PlacementAmiral+1);
  mUnLock(&mutexGrille);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Déplacement Gauche //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void DeplacementGauche()
{  
  while(PlacementAmiral>8)
  {
    Attente(DELAI_AMIRAL);

    if(AmiralExist == false) break;

    mLock(&mutexGrille);
    switch(tab[LIGNEAMIRAL][PlacementAmiral-1].type){
        case VIDE:
                EffaceCarre(LIGNEAMIRAL, PlacementAmiral);
                EffaceCarre(LIGNEAMIRAL, PlacementAmiral+1);

                setTab(LIGNEAMIRAL, PlacementAmiral+1, VIDE, 0);

                setTab(LIGNEAMIRAL, PlacementAmiral-1, AMIRAL, getTid());
                DessineVaisseauAmiral(LIGNEAMIRAL, PlacementAmiral-1);

                PlacementAmiral--;
              break;
        default:
              break;
    }
    mUnLock(&mutexGrille);
  }  
}



///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Deplacement Droite //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void DeplacementDroite(){
  while(PlacementAmiral<NB_COLONNE-2)
  {
    Attente(DELAI_AMIRAL);

    if(AmiralExist == false) break;

    mLock(&mutexGrille);
    switch(tab[LIGNEAMIRAL][PlacementAmiral+2].type){
        case VIDE:
                EffaceCarre(LIGNEAMIRAL, PlacementAmiral);
                EffaceCarre(LIGNEAMIRAL, PlacementAmiral+1);

                setTab(LIGNEAMIRAL, PlacementAmiral, VIDE, 0);

                setTab(LIGNEAMIRAL, PlacementAmiral+2, AMIRAL, getTid());
                DessineVaisseauAmiral(LIGNEAMIRAL, PlacementAmiral+1);

                PlacementAmiral++;
              break;
        default:
              break;
    }
    mUnLock(&mutexGrille);
  }  
}



///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Handler SIGALRM /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void handlerSIGALRM(int sig){
  #ifdef DEBUG
    printf("(threadVaisseauAmiral %ld) Réception SigAlarm\n",getTid());
  #endif
    
  AmiralExist = false;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  Handler SIGCHLD /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void handlerSIGCHLD(int sig){
  alarm(0);
  AmiralExist = false;
}