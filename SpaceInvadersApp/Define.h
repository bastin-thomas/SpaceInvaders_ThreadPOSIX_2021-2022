#ifndef DEFINE_H
#define DEFINE_H

// Librairie Utile
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#include "../GrilleSDL/GrilleSDL.h"
#include "../MyLibThread_POSIX/mylibthread_POSIX.h"
#include "../Ressources/Ressources.h"


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


// Type Personnalisé (Structure)
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

#define DELAI 1000

#endif