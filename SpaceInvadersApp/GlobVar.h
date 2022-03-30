#ifndef GLOBVAR_H
#define GLOBVAR_H 

#include "Define.h"
#include "./fonctionPerso.h"

/////////////////////////////////// Variables Globales ////////////////////////////////////////////

extern S_CASE tab[NB_LIGNE][NB_COLONNE];

extern int colVaisseau;
extern bool fireOn;

extern int delai;

extern int nbAliens;
extern int ligneHaut;        //Ligne Haut
extern int colonneGauche;    //ColonneGauche
extern int ligneBas;         //Ligne Bas
extern int colonneDroite;   //ColonneDroite

extern int Score;
extern bool MajScore;

extern int nbVies;



///////////////////////////////////       Mutex      //////////////////////////////////////////////
extern pthread_mutex_t mutexGrille;
extern pthread_mutex_t mutexFireOn;
extern pthread_mutex_t mutexFlotteAliens;
extern pthread_mutex_t mutexScore;
extern pthread_mutex_t mutexVies;



/////////////////////////////////// Variables Conditions //////////////////////////////////////////
extern pthread_cond_t condScore;
extern pthread_cond_t condVies;

#endif