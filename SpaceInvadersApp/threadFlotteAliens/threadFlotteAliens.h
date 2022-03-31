#ifndef THREADFLOTTEALIENS_H
#define THREADFLOTTEALIENS_H

#include "../GlobVar.h"
#include "../threadBombe/threadBombe.h"

void threadFlotteAliens();

void ShiftDroiteFlotte();
void ShiftGaucheFlotte();
void ShiftBasFlotte();

void PoseFlotte();
void RechercheBordure();

void LacherBombe();
#endif