#include "threadBombe.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  Thread Bombe //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadBombe(void* pos){
  sigset_t masque;
  S_POSITION Position = *((S_POSITION*)pos);
  free(pos);

  sigfillset(&masque);
  sigdelset(&masque, SIGINT);
  sigprocmask(SIG_SETMASK, &masque, NULL);

  #ifdef DEBUG
    printf("(threadBombe %ld) Lancement Bombe: L=%d C=%d\n",getTid(), Position.L+1, Position.C);
  #endif

  //Tentative de Création de la bombe selon le cas:
  mLock(&mutexGrille);
    switch(tab[Position.L+1][Position.C].type){
      case VIDE:
                setTab(Position.L+1, Position.C, BOMBE, getTid());
                DessineBombe(Position.L+1, Position.C);
              break;

      case BOMBE:
                mUnLock(&mutexGrille);
                pthread_exit(NULL);
              break;

      case MISSILE:
                pthread_kill(tab[Position.L+1][Position.C].tid, SIGINT); //Supression Missile

                setTab(Position.L+1, Position.C, VIDE, 0);   //Effacement Missile
                EffaceCarre(Position.L+1, Position.C);

                mUnLock(&mutexGrille);
                pthread_exit(NULL);
              break;

      case BOUCLIER1:
                setTab(Position.L+1, Position.C, BOUCLIER2, 0);
                DessineBouclier(Position.L+1, Position.C, 2);

                mUnLock(&mutexGrille);
                pthread_exit(NULL);
              break;

      case BOUCLIER2:
                setTab(Position.L+1, Position.C, VIDE, 0);
                EffaceCarre(Position.L+1, Position.C);

                mUnLock(&mutexGrille);
                pthread_exit(NULL);
              break;
      case ALIEN:
              break;
      default:
        printf("Erreur Creation Bombe, Type Inconnus, L = %d, C = %d\n", Position.L, Position.C);
        AfficheTab();
        exit(1);
    }
    Position.L++;
  mUnLock(&mutexGrille);



  //Debut du déplacement après création:
  while(Position.L<NB_LIGNE-1)
  {
    Attente(160);

    mLock(&mutexGrille);

      //Si c'est un Alien on Attend
      if(tab[Position.L+1][Position.C].type == ALIEN){
        mUnLock(&mutexGrille);
        continue;
      }
      
      //Si Non
      setTab(Position.L, Position.C, VIDE, 0);   //Effacement Bombe
      EffaceCarre(Position.L, Position.C);

      switch(tab[Position.L+1][Position.C].type){
        case VIDE:
                  setTab(Position.L+1, Position.C, BOMBE, getTid());  //Affichage Bombe
                  DessineBombe(Position.L+1, Position.C);
                break;

        case MISSILE:
                  pthread_kill(tab[Position.L+1][Position.C].tid, SIGINT); //Supression Missile

                  setTab(Position.L+1, Position.C, VIDE, 0);   //Effacement Missile
                  EffaceCarre(Position.L+1, Position.C);

                  mUnLock(&mutexGrille);
                  pthread_exit(NULL);
                break;

        case BOUCLIER1:
                  setTab(Position.L+1, Position.C, BOUCLIER2, 0); //Changement etat vers Bouclier2
                  DessineBouclier(Position.L+1, Position.C, 2);

                  mUnLock(&mutexGrille);
                  pthread_exit(NULL);
                break;

        case BOUCLIER2:
                  setTab(Position.L+1, Position.C, VIDE, 0); //Effacement Bouclier
                  EffaceCarre(Position.L+1, Position.C);

                  mUnLock(&mutexGrille);
                  pthread_exit(NULL);
                break;

        case VAISSEAU:
                  pthread_kill(tab[Position.L+1][Position.C].tid, SIGQUIT);
                  EffaceCarre(Position.L+1, Position.C);

                  mUnLock(&mutexGrille);
                  pthread_exit(NULL);
                break;

        case BOMBE:
                setTab(Position.L, Position.C, VIDE, 0);
                EffaceCarre(Position.L, Position.C);
                mUnLock(&mutexGrille);
                pthread_exit(NULL);
              break;

        default:
          printf("Erreur Deplacement Bombe, Type Inconnus\n");
          exit(1);
      }
      Position.L++;
    mUnLock(&mutexGrille);    
  }

  Attente(160);
  mLock(&mutexGrille);
    setTab(Position.L, Position.C, VIDE, 0);   //Effacement Bombe
    EffaceCarre(Position.L, Position.C);
  mUnLock(&mutexGrille);

  #ifdef DEBUG
    printf("(threadBombe %ld) Bombe Arrivée en fin de Tableau\n",getTid());
  #endif
  pthread_exit(NULL);
}
