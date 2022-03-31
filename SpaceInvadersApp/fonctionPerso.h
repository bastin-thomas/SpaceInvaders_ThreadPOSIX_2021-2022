#ifndef FONCTIONPERSO_H
#define FONCTIONPERSO_H
#include "GlobVar.h"

void Attente(int milli);

void setTab(int l,int c,int type=VIDE,pthread_t tid=0);

void freeAlien();	//Supprime les Aliens, Missiles et Bombes de la grille du Jeu

void AfficheTab();	//Affiche en console la grille de jeu

void MajVies();		//Met à jour l'affichage de la vie selon nbVies

#endif
