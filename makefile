.SILENT:

threadHeader = SpaceInvadersApp/threadVaisseau/threadVaisseau.h SpaceInvadersApp/threadEvent/threadEvent.h SpaceInvadersApp/threadMissile/threadMissile.h SpaceInvadersApp/threadTimeOut/threadTimeOut.h SpaceInvadersApp/threadInvader/threadInvader.h SpaceInvadersApp/threadFlotteAliens/threadFlotteAliens.h SpaceInvadersApp/threadScore/threadScore.h SpaceInvadersApp/threadBombe/threadBombe.h SpaceInvadersApp/Define.h SpaceInvadersApp/threadVaisseauAmiral/threadVaisseauAmiral.h
thread = obj/threadVaisseau.o obj/threadEvent.o obj/threadTimeOut.o obj/threadFlotteAliens.o obj/threadScore.o obj/threadBombe.o obj/threadInvader.o obj/threadMissile.o obj/threadVaisseauAmiral.o 
myOBJ = obj/MyLib.o obj/GrilleSDL.o obj/Ressources.o $(thread)
PROGRAMS = SpaceInvaders.app


ALL: $(PROGRAMS)

SpaceInvaders.app:	$(myOBJ) $(threadHeader) SpaceInvadersApp/SpaceInvaders.cpp MyLibThread_POSIX/mylibthread_POSIX.h GrilleSDL/GrilleSDL.h Ressources/Ressources.h SpaceInvadersApp/Define.h
					echo Creation de SpaceInvaders...
					g++ SpaceInvadersApp/SpaceInvaders.cpp $(myOBJ) -lrt -lpthread -lSDL -o SpaceInvaders.app


# Chaque Thread est une librairie, contenant les fonctions utiles à leur exécution:
obj/threadVaisseau.o:	SpaceInvadersApp/threadVaisseau/threadVaisseau.h SpaceInvadersApp/threadVaisseau/threadVaisseau.cpp SpaceInvadersApp/GlobVar.h SpaceInvadersApp/Define.h
						echo Creation de threadVaisseau.o ...
						g++ -c SpaceInvadersApp/threadVaisseau/threadVaisseau.cpp -lrt -lpthread -lSDL -o obj/threadVaisseau.o

obj/threadEvent.o:		SpaceInvadersApp/threadEvent/threadEvent.h SpaceInvadersApp/threadEvent/threadEvent.cpp SpaceInvadersApp/GlobVar.h SpaceInvadersApp/Define.h
						echo Creation de threadEvent.o ...
						g++ -c SpaceInvadersApp/threadEvent/threadEvent.cpp -lrt -lpthread -lSDL -o obj/threadEvent.o

obj/threadMissile.o:	SpaceInvadersApp/threadMissile/threadMissile.h SpaceInvadersApp/threadMissile/threadMissile.cpp SpaceInvadersApp/GlobVar.h SpaceInvadersApp/Define.h
						echo Creation de threadMissile.o ...
						g++ -c SpaceInvadersApp/threadMissile/threadMissile.cpp -lrt -lpthread -lSDL -o obj/threadMissile.o

obj/threadTimeOut.o:	SpaceInvadersApp/threadTimeOut/threadTimeOut.h SpaceInvadersApp/threadTimeOut/threadTimeOut.cpp SpaceInvadersApp/GlobVar.h SpaceInvadersApp/Define.h
						echo Creation de threadTimeOut.o ...
						g++ -c SpaceInvadersApp/threadTimeOut/threadTimeOut.cpp -lrt -lpthread -lSDL -o obj/threadTimeOut.o

obj/threadInvader.o:	SpaceInvadersApp/threadInvader/threadInvader.h SpaceInvadersApp/threadInvader/threadInvader.cpp SpaceInvadersApp/GlobVar.h SpaceInvadersApp/Define.h
						echo Creation de threadInvader.o ...
						g++ -c SpaceInvadersApp/threadInvader/threadInvader.cpp -lrt -lpthread -lSDL -o obj/threadInvader.o

obj/threadFlotteAliens.o:	SpaceInvadersApp/threadFlotteAliens/threadFlotteAliens.h SpaceInvadersApp/threadFlotteAliens/threadFlotteAliens.cpp SpaceInvadersApp/GlobVar.h SpaceInvadersApp/Define.h
						echo Creation de threadFlotteAliens.o ...
						g++ -c SpaceInvadersApp/threadFlotteAliens/threadFlotteAliens.cpp -lrt -lpthread -lSDL -o obj/threadFlotteAliens.o

obj/threadScore.o:	SpaceInvadersApp/threadScore/threadScore.h SpaceInvadersApp/threadScore/threadScore.cpp SpaceInvadersApp/GlobVar.h SpaceInvadersApp/Define.h
						echo Creation de threadScore.o ...
						g++ -c SpaceInvadersApp/threadScore/threadScore.cpp -lrt -lpthread -lSDL -o obj/threadScore.o

obj/threadBombe.o:	SpaceInvadersApp/threadBombe/threadBombe.h SpaceInvadersApp/threadBombe/threadBombe.cpp SpaceInvadersApp/GlobVar.h SpaceInvadersApp/Define.h
						echo Creation de threadBombe.o ...
						g++ -c SpaceInvadersApp/threadBombe/threadBombe.cpp -lrt -lpthread -lSDL -o obj/threadBombe.o

obj/threadVaisseauAmiral.o:	SpaceInvadersApp/threadVaisseauAmiral/threadVaisseauAmiral.h SpaceInvadersApp/threadVaisseauAmiral/threadVaisseauAmiral.cpp SpaceInvadersApp/GlobVar.h SpaceInvadersApp/Define.h
						echo Creation de threadVaisseauAmiral.o ...
						g++ -c SpaceInvadersApp/threadVaisseauAmiral/threadVaisseauAmiral.cpp -lrt -lpthread -lSDL -o obj/threadVaisseauAmiral.o



# Librairie Perso pour simplifier la syntaxe Posix lorsque c'est possible:
obj/MyLib.o:	MyLibThread_POSIX/mylibthread_POSIX.h MyLibThread_POSIX/mylibthread_POSIX.cpp
				echo Creation de MyLib.o ...
				g++ -c MyLibThread_POSIX/mylibthread_POSIX.cpp -o obj/MyLib.o -lpthread


# Librairie du Prof pour le moteur de Jeu:
obj/GrilleSDL.o:	GrilleSDL/GrilleSDL.cpp GrilleSDL/GrilleSDL.h
					echo Creation de GrilleSDL.o ...
					g++ -DSUN -I ./GrilleSDL -I ./Ressources -c GrilleSDL/GrilleSDL.cpp -o obj/GrilleSDL.o

obj/Ressources.o:	Ressources/Ressources.cpp Ressources/Ressources.h
					echo Creation de Ressources.o ...
					g++ -DSUN -I ./GrilleSDL -I ./Ressources -c Ressources/Ressources.cpp -o obj/Ressources.o

			

clear: clean

clean:
	echo "Supression *.o ..."
	rm obj/Ressources.o
	rm obj/GrilleSDL.o
	rm obj/MyLib.o
	rm obj/threadVaisseau.o
	rm obj/threadEvent.o
	rm obj/threadMissile.o
	rm obj/threadTimeOut.o
	rm obj/threadInvader.o
	rm obj/threadFlotteAliens.o
	rm obj/threadScore.o
	rm obj/threadBombe.o
	rm obj/threadVaisseauAmiral.o
	rm SpaceInvaders.app
