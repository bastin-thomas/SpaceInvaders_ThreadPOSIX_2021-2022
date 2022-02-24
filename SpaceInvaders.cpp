#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "GrilleSDL.h"
#include "Ressources.h"

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

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc,char* argv[])
{
  EVENT_GRILLE_SDL event;
 
  srand((unsigned)time(NULL));

  // Ouverture de la fenetre graphique
  printf("(MAIN %d) Ouverture de la fenetre graphique\n",pthread_self()); fflush(stdout);
  if (OuvertureFenetreGraphique() < 0)
  {
    printf("Erreur de OuvrirGrilleSDL\n");
    fflush(stdout);
    exit(1);
  }

  // Initialisation des mutex et variables de condition
  // TO DO

  // Armement des signaux
  // TO DO

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

  // Creation des threads
  // TO DO

  // Exemples d'utilisation du module Ressources --> a supprimer
  DessineChiffre(13,4,7);
  DessineMissile(4,12);
  DessineAlien(2,12);
  DessineVaisseauAmiral(0,15);
  DessineBombe(8,16);

  printf("(MAIN %d) Attente du clic sur la croix\n",pthread_self());  
  bool ok = false;
  while(!ok)
  {
    event = ReadEvent();
    if (event.type == CROIX) ok = true;
    if (event.type == CLIC_GAUCHE)
    {
      if (tab[event.ligne][event.colonne].type == VIDE) 
      {
        DessineVaisseau(event.ligne,event.colonne);
        setTab(event.ligne,event.colonne,VAISSEAU,pthread_self());
      }
    }
    if (event.type == CLAVIER)
    {
      if (event.touche == KEY_RIGHT) printf("Flèche droite enfoncée\n");
      printf("Touche enfoncee : %c\n",event.touche);
    }
  }

  // Fermeture de la fenetre
  printf("(MAIN %d) Fermeture de la fenetre graphique...",pthread_self()); fflush(stdout);
  FermetureFenetreGraphique();
  printf("OK\n");

  exit(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
