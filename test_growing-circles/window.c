/*!\file window.c 
 *
 *\brief GL4Dummies, dessiner un disque, 2 méthodes de remplissage :
 *par segments horizontaux et par cercles grossissants.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date June 02 2020
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>
/* pour la macro assert */
#include <assert.h>

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>


/* drawCircle avec une sous-fonction testAndPlot  */

static inline void testAndPlot(int x, int y) {
  if(IN_SCREEN(x, y))
    gl4dpPutPixel(x, y);
}

static void drawCircleNoM(int x0, int y0, int r) {
  int x, r2 = r * r;
  float y, finx = ((int)ceil((double)r * M_SQRT1_2)) + 1;
  int yp = r;
  for(x = 0; x < finx; ++x) {
    y = sqrt(r2 - x * x);
    if(yp != ((int)y)) {
      yp = (int)y;
      testAndPlot( (x - 1) + x0,  ((int)y) + y0); // 2eme octan
      testAndPlot(-(x - 1) + x0,  ((int)y) + y0); // 3eme octan
      testAndPlot( (x - 1) + x0, -((int)y) + y0); // 7eme octan
      testAndPlot(-(x - 1) + x0, -((int)y) + y0); // 6eme octan
      testAndPlot( y + x0,  (x - 1) + y0); // 1eme octan
      testAndPlot(-y + x0,  (x - 1) + y0); // 4eme octan
      testAndPlot( y + x0, -(x - 1) + y0); // 8eme octan
      testAndPlot(-y + x0, -(x - 1) + y0); // 5eme octan      
    }
    testAndPlot( x + x0,  ((int)y) + y0); // 2eme octan
    testAndPlot(-x + x0,  ((int)y) + y0); // 3eme octan
    testAndPlot( x + x0, -((int)y) + y0); // 7eme octan
    testAndPlot(-x + x0, -((int)y) + y0); // 6eme octan
    testAndPlot( y + x0,        x + y0); // 1eme octan
    testAndPlot(-y + x0,        x + y0); // 4eme octan
    testAndPlot( y + x0,       -x + y0); // 8eme octan
    testAndPlot(-y + x0,       -x + y0); // 5eme octan
  }
}

static void drawHLine(int x0, int x1, int y) {
  int w = gl4dpGetWidth();
  int h = gl4dpGetHeight();
  assert(x0 <= x1);
  if(y < 0 || y >= h) return;
  if(x0 < 0 && x1 < 0) return;
  if(x0 >= w && x1 >= w) return;
  x0 = MAX(x0, 0);
  x1 = MIN(x1, w - 1);
  for(; x0 <= x1; ++x0)
    gl4dpPutPixel(x0, y);
}

static void drawDisk(int x0, int y0, int r) {
  int x, r2 = r * r;
  float y, finx = ((int)ceil((double)r * M_SQRT1_2)) + 1;
  for(x = 0; x < finx; ++x) {
    y = sqrt(r2 - x * x) + 0.5f;
    drawHLine(-x + x0, x + x0, ((int)y) + y0);
    drawHLine(-x + x0, x + x0, -((int)y) + y0);
    drawHLine(-y + x0, y + x0, ((int)x) + y0);
    drawHLine(-y + x0, y + x0, -((int)x) + y0);
  }
}

static void dessinParCercles(void) {
  int i;
  static int rmax = 2;
  gl4dpClearScreen();
  gl4dpSetColor(RGB(200, 40, 0));
  for(i = 1; i < rmax; ++i)
    drawCircleNoM(320, 240, i);
  gl4dpUpdateScreen(NULL);
  if(++rmax == 240)
    rmax = 2;
}

static void dessinParDisque(void) {
  static int rmax = 1;
  gl4dpClearScreen();
  gl4dpSetColor(RGB(200, 40, 0));
  drawDisk(320, 240, rmax);
  gl4dpUpdateScreen(NULL);
  if(++rmax == 240)
    rmax = 2;
}


/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello World", /* titre */
			 10, 10, 640, 480, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  gl4dpInitScreen();
  /* effacement du screen en cours en utilisant la couleur par défaut,
   * le noir */

  gl4duwDisplayFunc(dessinParCercles);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */
  gl4duwMainLoop();
  return 0;
}
