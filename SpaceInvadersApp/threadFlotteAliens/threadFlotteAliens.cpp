#include "threadFlotteAliens.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  Thread Flotte Aliens //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void threadFlotteAliens(){
  int deplacement = 0; //Variable qui permettra de calculer les déplacement, pour définir le lancement de bombe

  #ifdef DEBUG
    printf("(ThreadFlotteAliens %ld) Pose de la Flotte\n",getTid()); fflush(stdout);
  #endif

  //Init Flotte    
  PoseFlotte();

  //Deplacement Flotte
  while(1){
    while(colonneDroite < NB_COLONNE-1)
    {
      
      Attente(delai);
      
      mLock(&mutexFlotteAliens);
        mLock(&mutexGrille);
          
          deplacement++;
          ShiftDroiteFlotte();
          RechercheBordure(); //Vérification des nouvelles bordures.

          #ifdef DEBUG
            AfficheTab();
          #endif


          #ifdef DEBUG
            AfficheTab();
          #endif

          if(nbAliens == 0)
          {
            #ifdef DEBUG
              printf("(ThreadFlotteAliens %ld) Tout les Aliens sont Morts\n",getTid()); fflush(stdout);
            #endif

            mUnLock(&mutexGrille);
            mUnLock(&mutexFlotteAliens);
            pthread_exit(NULL);
          }
        
          if(deplacement%2 == 0){ //Lancement de Bombe si deplacement est paire
            LacherBombe();
          }
        mUnLock(&mutexGrille);
      mUnLock(&mutexFlotteAliens);
    }

    while(colonneGauche > 8 )
    {
     
      Attente(delai);
     
      mLock(&mutexFlotteAliens);
        mLock(&mutexGrille);
          
          deplacement++;
          ShiftGaucheFlotte();
          RechercheBordure(); //Vérification des nouvelles bordures.
          
          #ifdef DEBUG
            AfficheTab();
          #endif


          if(nbAliens == 0)
          {
            #ifdef DEBUG
              printf("(ThreadFlotteAliens %ld) Tout les Aliens sont Morts\n",getTid()); fflush(stdout);
            #endif

            mUnLock(&mutexGrille);
            mUnLock(&mutexFlotteAliens);
            pthread_exit(NULL);
          }
        
          if(deplacement%2 == 0){ //Lancement de Bombe si deplacement est paire
            LacherBombe();
          }
        mUnLock(&mutexGrille);
      mUnLock(&mutexFlotteAliens);
    }


    Attente(delai);
   
    mLock(&mutexFlotteAliens);
      mLock(&mutexGrille);
        
        ShiftBasFlotte();
        RechercheBordure(); //Vérification des nouvelles bordures.

          #ifdef DEBUG
            AfficheTab();
          #endif

        
        #ifdef DEBUG
          AfficheTab();
        #endif

        if(nbAliens == 0)
        {
          #ifdef DEBUG
            printf("(ThreadFlotteAliens %ld) Tout les Aliens sont Morts\n",getTid()); fflush(stdout);
          #endif

          mUnLock(&mutexGrille);
          mUnLock(&mutexFlotteAliens);
          pthread_exit(NULL);
        }

        if(ligneBas == NB_LIGNE-3)
        {
          #ifdef DEBUG
            printf("(ThreadFlotteAliens %ld) %d Aliens ont envahi la terre !!!\n",getTid(), nbAliens); fflush(stdout);
          #endif

          mUnLock(&mutexGrille);
          mUnLock(&mutexFlotteAliens);
          pthread_exit(NULL);
        }

        if(deplacement%2 == 0){ //Lancement de Bombe si deplacement est paire
            LacherBombe();
        }

      mUnLock(&mutexGrille);
    mUnLock(&mutexFlotteAliens);
    

    //  Petit reset de déplacement à chaque fin de séquence de déplacement. 
    //  Tout en inversant la "parité" pour éviter un bombardement toujours dans les mêmes cases
    if(deplacement%2 == 0){
      deplacement = 1; 
    }
    else{
      deplacement = 0;
    }
  }

  #ifdef DEBUG
    printf("(ThreadFlotteAliens %ld) Erreur\n",getTid()); fflush(stdout);
  #endif
  pthread_exit(NULL);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  Fct Deplacement Droite Flotte Aliens //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ShiftDroiteFlotte(){
  pthread_t handler;
  S_POSITION* Position;

    for(int i = ligneHaut ; i <= ligneBas ; i +=2){
      for(int j = colonneGauche ; j <= colonneDroite ; j +=2){
        if(tab[i][j].type == ALIEN)
        {  
          //Si on tombe sur une case vide, on se déplace juste
          if(tab[i][j+1].type == VIDE)
          { 
            EffaceCarre(i,j);
            setTab(i,j, VIDE, 0);
            setTab(i,j+1, ALIEN, getTid());
            DessineAlien(i,j+1);
            continue;
          }

          //Si on tombe sur un missile, on 'éteint' le thread missile correspondant, et on le supprime de l'interface
          if(tab[i][j+1].type == MISSILE)
          {
            //Tue le missile
            pthread_kill(tab[i][j+1].tid, SIGINT);
            
            EffaceCarre(i,j);
            EffaceCarre(i,j+1);
            setTab(i,j, VIDE, 0);
            setTab(i,j+1, VIDE, 0);
            
            nbAliens --;

            //Réveil Thread Score pour l'affichage + Incrément de 1.
            mLock(&mutexScore);
              Score++;
              MajScore = true;
            mUnLock(&mutexScore);
            pthread_cond_signal(&condScore);


            #ifdef DEBUG
              printf("(ThreadFlotteAliens %ld) Un Aliens est Mort, Il reste %d Aliens\n",getTid(), nbAliens); fflush(stdout);
            #endif
          }

          //Si on tombe sur une Bombe, on 'éteint' le threadBombe correspondant, et on le crée un nouveau en dessous du tueur
          if(tab[i][j+1].type == BOMBE)
          {
            //Tue la Bombe
            pthread_kill(tab[i][j+1].tid, SIGINT);


            //Deplacement Alien
            EffaceCarre(i,j);
            setTab(i,j, VIDE, 0);
            setTab(i,j+1, ALIEN, getTid());
            DessineAlien(i,j+1);

            //Creation Nouveau ThreadBombe
            Position = new S_POSITION;
            Position->L = i;
            Position->C = j+1;
            pthread_create(&handler, NULL, (void* (*)(void*))threadBombe, (void*) Position);
            pthread_detach(handler);
          }          
        }
      }
    }
  colonneGauche++;
  colonneDroite++;  
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  Fct Deplacement Gauche Flotte Aliens //////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ShiftGaucheFlotte(){
  pthread_t handler;
  S_POSITION* Position;

    for(int i = ligneBas ; i >= ligneHaut ; i -=2){
      for(int j = colonneDroite ; j >=  colonneGauche ; j -=2){
        if(tab[i][j].type == ALIEN)
        {  
          //Si on tombe sur une case vide, on se déplace juste
          if(tab[i][j-1].type == VIDE)
          { 
            EffaceCarre(i,j);
            setTab(i,j, VIDE, 0);
            setTab(i,j-1, ALIEN, getTid());
            DessineAlien(i,j-1);
            continue;
          }

          //Si on tombe sur un missile, on 'éteint' le thread missile correspondant, et on le supprime de l'interface
          if(tab[i][j-1].type == MISSILE)
          {
            pthread_kill(tab[i][j-1].tid, SIGINT);

            EffaceCarre(i,j);
            EffaceCarre(i,j-1);
            setTab(i,j, VIDE, 0);
            setTab(i,j-1, VIDE, 0);

            nbAliens --;

            //Réveil Thread Score pour l'affichage + Incrément de 1.
            mLock(&mutexScore);
              Score++;
              MajScore = true;
            mUnLock(&mutexScore);
            pthread_cond_signal(&condScore);


            #ifdef DEBUG
              printf("(ThreadFlotteAliens %ld) Un Aliens est Mort, Il reste %d Aliens\n",getTid(), nbAliens); fflush(stdout);
            #endif
          }

          //Si on tombe sur une Bombe, on 'éteint' le threadBombe correspondant, et on le crée un nouveau en dessous du tueur
          if(tab[i][j-1].type == BOMBE)
          {
            //Tue le threadBombe
            pthread_kill(tab[i][j-1].tid, SIGINT);

            //Deplacement Alien
            EffaceCarre(i,j);
            setTab(i,j, VIDE, 0);
            setTab(i,j-1, ALIEN, getTid());
            DessineAlien(i,j-1);


            //Creation Nouveau ThreadBombe
            Position = new S_POSITION;
            Position->L = i;
            Position->C = j-1;
            pthread_create(&handler, NULL, (void* (*)(void*))threadBombe, (void*) Position);
            pthread_detach(handler);
          } 
        }
      }
    }
  colonneGauche--;
  colonneDroite--;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  Fct Deplacement Bas Flotte Aliens /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ShiftBasFlotte(){
  pthread_t handler;
  S_POSITION* Position;

    for(int i = ligneBas ; i >= ligneHaut ; i -=2){
      for(int j = colonneDroite ; j >= colonneGauche ; j -=2){
        if(tab[i][j].type == ALIEN)
        {  
          //Si on tombe sur une case vide, on se déplace juste
          if(tab[i+1][j].type == VIDE)
          { 
            EffaceCarre(i,j);
            setTab(i,j, VIDE, 0);
            setTab(i+1,j, ALIEN, getTid());
            DessineAlien(i+1,j);
            continue;
          } 

          //Si on tombe sur un missile, on 'éteint' le thread missile correspondant, et on le supprime de l'interface
          if(tab[i+1][j].type == MISSILE)
          {
            pthread_kill(tab[i+1][j].tid, SIGINT);

            EffaceCarre(i,j);
            EffaceCarre(i+1,j);
            setTab(i,j, VIDE, 0);
            setTab(i+1,j, VIDE, 0);
            
            nbAliens --;

            //Réveil Thread Score pour l'affichage + Incrément de 1.
            mLock(&mutexScore);
              Score++;
              MajScore = true;
            mUnLock(&mutexScore);
            pthread_cond_signal(&condScore);


            #ifdef DEBUG
              printf("(ThreadFlotteAliens %ld) Un Aliens est Mort, Il reste %d Aliens\n",getTid(), nbAliens); fflush(stdout);
            #endif
          }

          //Si on tombe sur une Bombe, on 'éteint' le threadBombe correspondant, et on le crée un nouveau en dessous du tueur
          if(tab[i+1][j].type == BOMBE)
          {
            //Tue le Thread Bombe
            pthread_kill(tab[i+1][j].tid, SIGINT);

            //Deplacement Alien
            EffaceCarre(i,j);
            setTab(i,j, VIDE, 0);
            setTab(i+1,j, ALIEN, getTid());
            DessineAlien(i+1,j);

            //Creation Nouveau ThreadBombe
            Position = new S_POSITION;
            Position->L = i+1;
            Position->C = j;
            
            pthread_create(&handler, NULL, (void* (*)(void*))threadBombe, (void*) Position);
            pthread_detach(handler);
          }
        }
      }
    }
  ligneBas++;
  ligneHaut++;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////  Fct Deplacement Pose Innitial de la Flotte Aliens /////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PoseFlotte(){
  mLock(&mutexFlotteAliens);
    mLock(&mutexGrille);   
      nbAliens = 24;
      ligneHaut = 2;        //Ligne Haut
      colonneGauche = 8;    //ColonneGauche
      ligneBas = 8;         //Ligne Bas
      colonneDroite = 18;   //ColonneDroite

      for(int i = ligneHaut ; i <= ligneBas ; i += 2)
      {
        for(int j = colonneGauche ; j <= colonneDroite ; j += 2)
        {
          setTab(i,j, ALIEN, getpid());
          DessineAlien(i,j);
        }
      }
    mUnLock(&mutexGrille);
  mUnLock(&mutexFlotteAliens);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////  Fct Mise à Jour des Bordures de la Flotte Aliens /////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void RechercheBordure(){
  bool test = true;

  if(nbAliens == 0) return;

  while(1){
      //Si les bords externe de lignehaut sont vide, on regarde si toutes les cases sont vide, 
        test = true;
        for(int i = colonneGauche ; i<= colonneDroite ; i++){
          if(tab[ligneHaut][i].type == ALIEN){
            test = false;
            break;
          }
        }

        //si oui, on retire une ligne, et on réentre dans la boucle
        if(test){
          ligneHaut+=2;
          continue;
        }


      //Si les bords externe de ligneBas sont vide, on regarde si toutes les cases sont vide, 
        test = true;
        for(int i = colonneGauche ; i<= colonneDroite ; i++){
          if(tab[ligneBas][i].type == ALIEN){
            test = false;
            break;
          }
        }

        //si oui, on retire une ligne, et on réentre dans la boucle
        if(test){
          ligneBas-=2;
          continue;
        }
      
      //Si non on passe aux cas suivant.

      //Si les bords externe de colonneGauche sont vide, on regarde si toutes les cases du vecteur colonneGauche sont vide, 
        test = true;
        for(int i = ligneHaut ; i<= ligneBas ; i++){
          if(tab[i][colonneGauche].type == ALIEN){
            test = false;
            break;
          }
        }

        //si oui, on retire une colonne, et on réentre dans la boucle
        if(test){
          colonneGauche+=2;
          continue;
        }
      
      //Si non on passe aux cas suivant.

      //Si les bords externe de colonneDroite sont vide, on regarde si toutes les cases du vecteur colonneGauche sont vide,
        test = true;
        for(int i = ligneHaut ; i<= ligneBas ; i++){
          if(tab[i][colonneDroite].type == ALIEN){
            test = false;
            break;
          }
        }
        
        if(test){
          //si oui, on retire une colonne, et on réentre dans la boucle
          colonneDroite-=2;
          continue;
        }
      //Si aucun cas n'est satisfait, on quitte la boucle.
      break;
  }

  #ifdef DEBUG
    printf("ligneHaut = %d, ligneBas = %d, ColGauche = %d, ColDroite = %d\n", ligneHaut, ligneBas, colonneGauche, colonneDroite);
    fflush(stdout);
  #endif
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////  Fct Qui lance de façon Random des ThreadBombes  //////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void LacherBombe(){
  int random;
  bool stop;
  int  i,j,k;
  pthread_t handler;
  S_POSITION* Position = new S_POSITION;

    stop = false;
    random = (rand() % nbAliens)+1;
    k = 0;

    //Recherche d'un Alien aux hazards dans les aliens vivant
    for(i = 0 ; i <= ligneBas; i++){
      for(j = 8 ; j <= colonneDroite; j++){
        if(tab[i][j].type == ALIEN){ //Si un élément est un alien on incrémente k (le nombre d'alien déjà rencontré)
          k++;

          if(k == random){ //Si en plus, le nombre d'alien déjà rencontré est égale au numéro d'alien prit alléatoirement, alors on sort de la boucle
            stop = true;
            break;
          }
        }
      }
      if(stop == true) break;
    }

  if(stop == false){
    printf("Erreur, aucune Bombe Lancée car erreur de recherche random\n");
    printf("i = %d;\nj = %d;\nrandom = %d;\nk = %d;\nnbAlien = %d;\n",i,j,random,k,nbAliens);
    exit(2);
  }

  Position->L = i;
  Position->C = j;

  pthread_create(&handler, NULL, (void* (*)(void*))threadBombe, (void*) Position);
  pthread_detach(handler);
}
