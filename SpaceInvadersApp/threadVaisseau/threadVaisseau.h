#ifndef THREADVAISSEAU_H
#define THREADVAISSEAU_H

#include "../GlobVar.h"
#include "../fonctionPerso.h"
#include "../threadMissile/threadMissile.h"
#include "../threadTimeOut/threadTimeOut.h"

void threadVaisseau();

void TermVaisseau();

void handlerSIGUSR1(int sig);

void handlerSIGUSR2(int sig);

void handlerSIGHUP(int sig);

void handlerSIGQUIT(int sig);

#endif