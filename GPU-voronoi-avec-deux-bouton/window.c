/*!\file window.c 
 *
 *\brief Simulation basique de points mobiles à l'écran avec rendu GLSL.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date June 05 2020
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

#include <unistd.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/* prototypes des fonctions de gestion des mobiles */
#include "mobile.h"

/* \brief fonction appelée lors du exit */
static void quit(void);

static void init(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 800, _wH = 800;
/*!\brief identifiant du quadrilatère faisant office de "rectangle écran" */
static GLuint _quad = 0;
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;
/* \brief nombre de mobiles demandés pour la simulation */
static const int nb_mobiles = 42;
/*!\brief toggle Voronoï mode */
static GLint _voronoi = GL_FALSE;
/*!\brief toggle Voronoï mode */
static GLboolean _vrn = GL_FALSE;
/* \brief fonction appelée par le callBack display */
static void dessin(void) {
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme de rendu _pId */
  glUseProgram(_pId);
  /* activer l'étage de texture 0 */
  glActiveTexture(GL_TEXTURE0);
  mobile2Data();  
  glUniform1i(glGetUniformLocation(_pId, "voronoi"), _voronoi);
  glUniform1i(glGetUniformLocation(_pId, "vrn"), _vrn);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  glUniform1i(glGetUniformLocation(_pId, "nbmob"), nb_mobiles);
  /* dessin de la géométrie du quadrilatère */
  gl4dgDraw(_quad);
  /* désactiver le programme shader */
  glUseProgram(0);
}
/*!\brief manages keyboard-down event */
static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  case 'v':
    _voronoi = !_voronoi;
    break;
  case 'b':
    _vrn = !_vrn;
    break;
  }
}
/* \brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Mobile simulation", /* titre */
			 10, 10, _wW, _wH, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  init();
  /* initialiser la graine aléatoire */
  srand(time(NULL));
  /* initialisation du diagramme de Voronoï */
  mobileInit(nb_mobiles);
  /*chnage mode couleur on clic sur la touche v ou q pour quitter */ 
   gl4duwKeyDownFunc(keydown);
  /* mise en place de la callBack idle */
  gl4duwIdleFunc(mobileMove);
  /* mise en place de la callBack display */
  gl4duwDisplayFunc(dessin);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  
  return 0;
}

/*!\brief initialise les paramètres OpenGL et les données. 
 */
static void init(void) {
  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL, l'écran est effacé
   * avec du rouge. Ici : 1.0f en r => 100% ; 0.2f en g => 20% ; 0.2f
   * en b => 20% ; 1.0f en a => 100% ; */
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  /* dans quelle partie de l'écran on effectue le rendu : l'ensemble
   * de l'écran */
  glViewport(0, 0, _wW, _wH);
  /* génération de la géométrie du quadrilatère */
  _quad = gl4dgGenQuadf();
  atexit(quit);
}

static void quit(void) {
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
