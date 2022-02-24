.SILENT:

myOBJ = .obj

PROGRAMS = SpaceInvaders.app

ALL: $(PROGRAMS)

SpaceInvaders.app:	SpaceInvaders.cpp .obj/GrilleSDL.o .obj/Ressources.o
	echo Creation de SpaceInvaders...
	g++ -DSUN -I ./GrilleSDL -I ./Ressources SpaceInvaders.cpp .obj/GrilleSDL.o .obj/Ressources.o -lrt -lpthread -lSDL -o SpaceInvaders.app 


.obj/GrilleSDL.o:	GrilleSDL/GrilleSDL.cpp GrilleSDL/GrilleSDL.h
		echo Creation de GrilleSDL.o ...
		g++ -DSUN -I ./GrilleSDL -I ./Ressources -c GrilleSDL/GrilleSDL.cpp -o .obj/GrilleSDL.o

.obj/Ressources.o:	Ressources/Ressources.cpp Ressources/Ressources.h
		echo Creation de Ressources.o ...
		g++ -DSUN -I ./GrilleSDL -I ./Ressources -c Ressources/Ressources.cpp -o .obj/Ressources.o

clean:
	rm .obj/Ressources.o
	rm .obj/GrilleSDL.o
	rm SpaceInvaders.app
