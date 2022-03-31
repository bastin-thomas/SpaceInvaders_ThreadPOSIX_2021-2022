#include "threadEvent.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  Thread Event ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadEvent(){
  pthread_cleanup_push( (void(*)(void*)) TermEvent, NULL);

  #ifdef DEBUG
    printf("(ThreadEvent %ld) Creation ThreadEvent\n",getTid());  
  #endif

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
        default:
              break;
      }
    }
  }
  pthread_cleanup_pop(1);
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  Fonction Terminaison Event /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void TermEvent(){
  // Fermeture de la fenetre
  #ifdef DEBUG
    printf("(ThreadEvent %ld) Fermeture de la fenetre graphique...",getTid()); fflush(stdout);
  #endif

  FermetureFenetreGraphique();

  #ifdef DEBUG
    printf("OK\n");
  #endif
  exit(0);
}