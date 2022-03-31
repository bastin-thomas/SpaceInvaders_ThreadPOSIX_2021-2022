#ifndef THREADVAISSEAUAMIRAL_H
#define THREADVAISSEAUAMIRAL_H

#include "../GlobVar.h"

void threadVaisseauAmiral();

void Deplacement();

void DeplacementGauche();

void DeplacementDroite();

void handlerSIGALRM(int sig);

void handlerSIGCHLD(int sig);

#endif