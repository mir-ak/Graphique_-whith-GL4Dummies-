/*!\file window.c 
 *
 *\brief Diagramme de voronoi par cercles grossissants.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date June 03 2020
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>
/* pour la macro assert */
#include <assert.h>
/* pour la fonction time (supprimer dans le srand si non disponible
 * sur votre OS / compilateur) */
#include <time.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/* prototypes des fonctions de gestion de sites (de Voronoi) */
#include "voronoi.h"

/* \brief nombre de sites demandés pour le diagramme de voronoi */
static const int nb_sites = 4200;

/* \brief image (SDL) qui servira de "fond" pour la simulation */
static SDL_Surface * _background = NULL;

/* \brief fonction appelée par le callBack display */
static void dessin(void) {

  /* bloc de code avec grossissement visible */
  /*if(!vdGrow()) {
    gl4dpUpdateScreen(NULL);
    vdInit(nb_sites,_background);
  } else
    gl4dpUpdateScreen(NULL);*/

  /* bloc de code avec état finale à chaque fois */
  //while(vdGrow());
  /*je sais pas trop si j'ai réussi la fonction vdDrow */ 
  vdDraw(); 
  gl4dpUpdateScreen(NULL);
  //vdInit(nb_sites,_background);
}

/* \brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  SDL_Surface * s = SDL_LoadBMP("bg01.bmp");

  _background = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_ABGR8888, 0);
   //SDL_FreeSurface(s);  
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Voronoï Diagram", /* titre */
			 10, 10, 1200, 800, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  gl4dpInitScreen();
  /* initialiser la graine aléatoire */
  srand(time(NULL));
  /* initialisation du diagramme de Voronoï */
  vdInit(nb_sites,_background);
   /* mise en place de la callBack idle */
  gl4duwIdleFunc(Move_voronoi);
  /* mise en place de la callBack display */
  gl4duwDisplayFunc(dessin);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
