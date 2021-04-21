/*!\file window.c
 * \brief GL4Dummies, interaction dans un labyrinthe
 * \author Farès BELHADJ, amsi@up8.edu
 * \date May 28 2020
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/* pour la macro assert */
#include <assert.h>

/* pour la pile (push, pop, empty) */
#include "pile.h"
#include <unistd.h>
/* prototype de la fonction labyrinth définie dans makeLabyrinth.c */
extern unsigned int * labyrinth(int w, int h);
  
#define W 81
#define H 81

#define WW 800
#define WH 800

static void simu(void);
static void dessin(void);
static void keydown(int keycode);
static void mousefunc(int button, int state, int x, int y);
static int  move(int dx, int dy);
static void pcc(int x0, int y0, int x1, int y1);
static void quit(void);

typedef struct point2di_t point2di_t;

struct point2di_t {
  int x, y;
};

static point2di_t _perso = {1, 1};
static unsigned int * _laby = NULL;


/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* test si la largeur et la hauteur du screen sont impaires */
  assert( (W&1) && (H&1) );
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello World", /* titre */
			 10, 10, WW, WH, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible et resizable */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  gl4dpInitScreenWithDimensions(W, H);
  /* générer un labyrinthe aux dimensions du screen */
  _laby = labyrinth(gl4dpGetWidth(), gl4dpGetHeight());
  fprintf(stderr, "%lu %lu\n", sizeof *_laby, sizeof (GLuint));
  /* ajouter quit dans la pile des choses à faire au moment du exit */
  atexit(quit);
  /* effacement du screen en cours en utilisant la couleur par défaut,
   * le noir */
  gl4dpClearScreen();
  /* ajout de la fonction callBack pour touche du clavier pressée */
  gl4duwKeyDownFunc(keydown);
  gl4duwMouseFunc(mousefunc);
  gl4duwIdleFunc(simu);
  gl4duwDisplayFunc(dessin);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  /* à partir de là, zone morte, jamais exécutée */
  return 0;
}
static void departure() {
    static double new_dt, last_dt = 0.0, step = 0.02;
    new_dt = gl4dGetElapsedTime() / 1000.0;
    if (!empty() && ((new_dt - last_dt) > step)) {
        last_dt = new_dt;
        _perso.x = pop();
        _perso.y = pop();
        _laby[_perso.y * gl4dpGetWidth() + _perso.x] = 0;
    }
}
static void simu(void) {
  gl4dpScreenHasChanged();
  departure();
}

static void dessin(void) {
  /* effacement du screen en cours en utilisant la couleur par défaut,
   * le noir */
  gl4dpClearScreen();
  memcpy(gl4dpGetPixels(), _laby, gl4dpGetWidth() * gl4dpGetHeight() * sizeof *_laby);
  gl4dpSetColor(RGB(255, 0, 0));
  gl4dpPutPixel(_perso.x, _perso.y);
  gl4dpUpdateScreen(NULL);
}

static void keydown(int keycode) {
  switch(keycode) {
  case GL4DK_ESCAPE:
    exit(0);
    break;
  case GL4DK_LEFT:
    if(_perso.x > 0)
      move(-1, 0);
    break;
  case GL4DK_RIGHT:
    if(_perso.x < W - 1)
      move(1, 0);
    break;
  case GL4DK_DOWN:
    if(_perso.y > 0)
      move(0, -1);
    break;
  case GL4DK_UP:
    if(_perso.y < H - 1)
      move(0, 1);
    break;
  default:
    fprintf(stderr, "La touche <%d> est pressee\n", keycode);
    break;
  }
}

/* \brief déplace \ref _perso de \a dx et \a dy si possible. 
 *
 * \param dx déplacement en x
 * \param dy déplacement en y
 * \return teste si le déplacement est possible, alors il est effectué
 * et 1 et retourné, sinon 0 est retourné.
 */
static int move(int dx, int dy) {
  point2di_t n = { _perso.x + dx, _perso.y + dy };
  if(IN_SCREEN(n.x, n.y) && _laby[n.y * gl4dpGetWidth() + n.x] == 0) {
    _perso = n;
    return 1;
  }
  return 0;
}
  
static void mousefunc(int button, int state, int x, int y) {
  fprintf(stderr, "button (%d), state (%d), x (%d), y (%d)\n", button, state, x, y);
  if(state) {
    point2di_t n;
    n.x = (x * W) / WW;
    n.y = ( ((WH - 1) - y) * H) / WH;
    if(IN_SCREEN(n.x, n.y) && _laby[n.y * gl4dpGetWidth() + n.x] == 0) {
      if(button == GL4D_BUTTON_LEFT)
	_perso = n;
      else if(button == GL4D_BUTTON_RIGHT) {
	/* on lance un plus court chemin (profondeur d'abord) */
	pcc(_perso.x, _perso.y, n.x, n.y);
      }
    }
  }
}

static void subpcc(int * l, int v, int x0, int y0, int x1, int y1, int w) {
  int i;
  const int d[][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
  l[y0 * w + x0]  = v++;
  if(x0 == x1 && y0 == y1) return;
  for(i = 0; i < 4; ++i) {
    int nx = x0 + d[i][0];
    int ny = y0 + d[i][1];
    if(l[ny * w + nx] == 0 || l[ny * w + nx] > v)
      subpcc(l, v, nx, ny, x1, y1, w);
  }
}

static void pcc(int x0, int y0, int x1, int y1) {
  int * copy = malloc(gl4dpGetWidth() * gl4dpGetHeight() * sizeof *copy);
  memcpy(copy, (int *)_laby, gl4dpGetWidth() * gl4dpGetHeight() * sizeof *copy);
  subpcc(copy, 1, x0, y0, x1, y1, gl4dpGetWidth());
  while(x1 != x0 || y1 != y0) {
    int i;
    const int d[][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    push(y1);
    push(x1);
    _laby[y1 * gl4dpGetWidth() + x1] = RGB(0, 255, 0);
    for(i = 0; i < 4; ++i) {
      int nx = x1 + d[i][0];
      int ny = y1 + d[i][1];
      if(copy[ny * gl4dpGetWidth() + nx] == copy[y1 * gl4dpGetWidth() + x1] - 1) {
	x1 = nx;
	y1 = ny;
	break;
      }
    }
  }
  free(copy);
}


static void quit(void) {
  if(_laby) {
    free(_laby);
    _laby = NULL;
  }
}
