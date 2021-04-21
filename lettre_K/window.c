/*!\file window.c 
 *\brief GL4Dummies, dessiner soit-même un polygone et faire le
 * remplissage.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date May 29 2020
 */

/* inclusion des entêtes de fonctions de gestion de primitives simples
 * de dessin. La lettre p signifie aussi bien primitive que
 * pédagogique. */
#include <GL4D/gl4dp.h>
/* pour la macro assert */
#include <assert.h>

/*\brief type de donnée pour un sommet 2d à coordonnées entières */
typedef struct point2di_t point2di_t; 

/*\brief structure de sommet 2d à coordonnées entières */
struct point2di_t {
  GLint x, y;
  GLubyte r, g, b;
};
/* \brief type de donnée pour un mobile */
typedef struct mobile_t mobile_t;

/* \brief structure de données du mobile : coordonnée 2d flottante,
 * vitesse 2d flottante et couleur. */
struct mobile_t {
  float x, y;
  float vx, vy;
  GLuint color;
};

/* \brief l'ensemble des mobiles */
static mobile_t * _mobiles = NULL;
/* \brief le nombre de mobiles dans la simulation (c'est un multiple
 * de 3) */
static int _nb_mobiles = 0;
static void mobileInit(int nb_mobiles);
static void mobileFree(void);
static void mobileMove(void);

/*\brief dessine, dans le screen en cours, à partir d'une liste de
 * sommets (points 2D) les segments reliant ces sommets deux à deux
 * puis referme la forme en reliant le dernier sommet au premier.
 *
 * Cette fonction ne gère que des polygones convexes.
 *
 *\param vertices (pluriel de vertex qui signifie sommet) le tableau
 * contenant les n sommets du polygone.
 *\param n le nombre de sommets du polygone (n doit être >= à 3).
 *\param fill booléen permettant d'indiquer si la forme est remplie
 *(colorée à l'intérieur) ou dessinée en mode fil de fer (il n'y a que
 *le contour qui est dessiné.
 */
void gl4dpDrawPolygon(point2di_t * vertices, int n, GLboolean fill) {
  int i,xmax,xmin,ymax,ymin,c,j;//tmp_c=0,nb=0;
  xmax=vertices[0].x;
  ymax=vertices[0].y;
  xmin=vertices[0].x;
  ymin=vertices[0].y;
  assert(n >= 3);
  for(i = 0; i < n; ++i) {
    gl4dpLine(vertices[i].x, vertices[i].y, vertices[(i + 1) % n].x, vertices[(i + 1) % n].y);
    if(vertices[(i+1)%n].y> ymax){
      ymax=vertices[(i+1)%n].y;
    }
    if(vertices[(i+1)%n].y< ymin){
      ymin=vertices[(i+1)%n].y;
    }
    if(vertices[(i+1)%n].x> xmax){
      xmax=vertices[(i+1)%n].x;
    }
    if(vertices[(i+1)%n].x< xmin){
      xmin=vertices[(i+1)%n].x;
    }
  }
  for (j = ymax-1; j > ymin; j--)
  {
    for(c = xmax; c>= xmin;c--)
      if (gl4dpGetPixel(c,j)==gl4dpGetColor())
        for (c = c-1; gl4dpGetPixel(c,j) != gl4dpGetColor(); c--)
          gl4dpPutPixel(c,j);
  }
  
};


/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
/*static void vLine(int x, int y0, int y1, GLuint color) {
  int w = gl4dpGetWidth(), h = gl4dpGetHeight(), i, p;
  GLuint * pixels = gl4dpGetPixels();
   tous les cas à éviter, ne donnant rien */
  /*if(x < 0 || x >= w || (y0 < 0 && y1 < 0) || (y0 >= h && y1 >= h))
    return;
  y0 = MIN(MAX(y0, 0), h - 1);
  y1 = MIN(MAX(y1, 0), h - 1);
  p = y0 < y1 ? w : -w;
  y0 = y0 * w + x;
  y1 = y1 * w + x;
  for(i = y0; i != y1; i += p)
    pixels[i] = color;
}*/
/*static void dessin(void) {
  /* créez à l'aide de plusieurs triangle une forme correspondant le*/
   /* point2di_t p[][4] = { 
          {  {10, 10},   {50, 10},  {50, 470}, {10, 470} },
          { {50, 470}, {300, 470}, {260, 430}, {50, 430} },
          { {50, 250}, {260, 250}, {260, 210}, {50, 210} }
  }; 
  for(int i = 0; i < 3; ++i)
    gl4dpDrawPolygon(p[i], sizeof p[i] / sizeof *p[i], GL_TRUE);
  gl4dpUpdateScreen(NULL);
}*/
  


int main(int argc, char ** argv) {
  //int i;
  /* créez à l'aide de plusieurs polygones convexes une forme
   * correspondant le mieux possible à la première lettre de votre
   * prénom. */
  point2di_t p[][4] = { 
		      {  {10, 10},   {50, 10},  {50, 470}, {10, 470} },
		      { {50, 255}, {50, 215}, {305, 470}, {265, 470} },
		      { {50, 255}, {50, 215},  {255, 10}, {295, 10} }
  }; 
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
 // gl4dpClearScreen();

  /* mettre la couleur en cours à rouge bordeaux */
  gl4dpSetColor(RGB(180, 40, 0));
  /* appel de la fonction qui dessine les polygones convexes avec, à
   * chaque i, la liste de points p[i] */
  for(int i = 0; i < 3; ++i)
    gl4dpDrawPolygon(p[i], sizeof p[i] / sizeof *p[i], GL_TRUE);
  gl4dpUpdateScreen(NULL);
  mobileInit(1);
 // gl4duwDisplayFunc(dessin);
  gl4duwIdleFunc(mobileMove);
    //gl4duwDisplayFunc(dessin);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */

  gl4duwMainLoop();
  return 0;
}
static void mobileInit(int nb_mobiles) {
  int i, w = gl4dpGetWidth(), h = gl4dpGetHeight();
  if(_mobiles) {
    mobileFree();
  } else {
    atexit(mobileFree);
  }
  _nb_mobiles = 3 * nb_mobiles;
  _mobiles = malloc(_nb_mobiles * sizeof *_mobiles);
  assert(_mobiles);
  for(i = 0; i < _nb_mobiles; ++i) {
    _mobiles[i].x = (float) (gl4dmURand() * w);
    _mobiles[i].y = (float) (gl4dmURand() * h);
    _mobiles[i].vx = (float) (gl4dmSURand() * 400.0);
    _mobiles[i].vy = (float) (gl4dmSURand() * 400.0);
    _mobiles[i].color = RGB(rand()&255, rand()&255, rand()&255);
  }
}

/* \brief libère la mémoire et réinitialise les paramètres de la
 *  simulation. Cette fonction est potentiellement appelée à
 *  l'événement exit.
 */
static void mobileFree(void) {
  if(_mobiles) {
    free(_mobiles);
    _mobiles = NULL;
    _nb_mobiles = 0;
  }
}

/* \brief déplace tous les mobiles en fonction de leur position,
 * vitesse et du temps.
 */
static void mobileMove(void) {
  int i, w = gl4dpGetWidth(), h = gl4dpGetHeight();
  /* récupération du dt */
  static double t0 = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0;
  double dt = t - t0;
  t0 = t;
  /* fin de récupération du dt */
  for(i = 0; i < _nb_mobiles; ++i) {
    /* futur x et futur y */
    float fx, fy;
    fx = _mobiles[i].x + _mobiles[i].vx * dt;
    fy = _mobiles[i].y + _mobiles[i].vy * dt;
    if((fx < 0.0f && _mobiles[i].vx < 0.0f) ||
       (fx >= w && _mobiles[i].vx > 0.0f))
      _mobiles[i].vx = -_mobiles[i].vx;
    if((fy < 0.0f && _mobiles[i].vy < 0.0f)||
       (fy >= h && _mobiles[i].vy > 0.0f))
      _mobiles[i].vy = -_mobiles[i].vy;
    _mobiles[i].x += _mobiles[i].vx * dt;
    _mobiles[i].y += _mobiles[i].vy * dt;
  }
}
