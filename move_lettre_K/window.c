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

static float vx, vy;

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
static void dessin(void) {
    //int i;
  /* créez à l'aide de plusieurs polygones convexes une forme
   * correspondant le mieux possible à la première lettre de votre
   * prénom. */
  gl4dpClearScreen();
  point2di_t p[][4] = { 
          {  {(GLint)_mobiles[0].x, (GLint)_mobiles[0].y},   {(GLint)_mobiles[1].x, (GLint)_mobiles[1].y},  {(GLint)_mobiles[2].x, (GLint)_mobiles[2].y}, {(GLint)_mobiles[3].x, (GLint)_mobiles[3].y} },
          {  {(GLint)_mobiles[4].x, (GLint)_mobiles[4].y},   {(GLint)_mobiles[5].x, (GLint)_mobiles[5].y},  {(GLint)_mobiles[6].x, (GLint)_mobiles[6].y}, {(GLint)_mobiles[7].x, (GLint)_mobiles[7].y} },
          {  {(GLint)_mobiles[8].x, (GLint)_mobiles[8].y},   {(GLint)_mobiles[9].x, (GLint)_mobiles[9].y},  {(GLint)_mobiles[10].x, (GLint)_mobiles[10].y}, {(GLint)_mobiles[11].x, (GLint)_mobiles[11].y} }
  };
  /* mettre la couleur en cours à rouge bordeaux */
  gl4dpSetColor(RGB(180, 40, 0));
  /* appel de la fonction qui dessine les polygones convexes avec, à
   * chaque i, la liste de points p[i] */
  for(int i = 0; i < 3; ++i)
    gl4dpDrawPolygon(p[i], sizeof p[i] / sizeof *p[i], GL_TRUE);
  gl4dpUpdateScreen(NULL);
}
  


int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello World", /* titre */
			 10, 10, 1200, 800, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  vx = (2.0 * (rand() / (RAND_MAX + 1.0)) - 1.0) * 400.0;
  vy = (2.0 * (rand() / (RAND_MAX + 1.0)) - 1.0) * 400.0;
  /* création d'un screen GL4Dummies (texture dans laquelle nous
   * pouvons dessiner) aux dimensions de la fenêtre */
  gl4dpInitScreen();
  /* effacement du screen en cours en utilisant la couleur par défaut,
   * le noir */
 // gl4dpClearScreen();
  mobileInit(3);
  gl4duwDisplayFunc(dessin);
  gl4duwIdleFunc(mobileMove);
    //gl4duwDisplayFunc(dessin);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre */

  gl4duwMainLoop();
  return 0;
}
static void mobileInit(int nb_mobiles) {
  if(_mobiles) {
    mobileFree();
  } else {
    atexit(mobileFree);
  }
  _nb_mobiles = 4 * nb_mobiles;
  _mobiles = malloc(_nb_mobiles * sizeof *_mobiles);
  assert(_mobiles);
  /*point2di_t p[][4] = { 
          {  {10, 10},   {50, 10},  {50, 470}, {10, 470} },
          { {50, 255}, {50, 215}, {305, 470}, {265, 470} },
          { {50, 255}, {50, 215},  {255, 10}, {295, 10} }
  };*/ 
  _mobiles[0].x = 10.0;
  _mobiles[0].y = 10.0;
  _mobiles[1].x = 50.0;
  _mobiles[1].y = 10.0;
  _mobiles[2].x = 50.0;
  _mobiles[2].y = 470.0;
  _mobiles[3].x = 10.0;
  _mobiles[3].y = 470.0;


  _mobiles[4].x = 50.0;
  _mobiles[4].y = 255.0;
  _mobiles[5].x = 50.0;
  _mobiles[5].y = 215.0;
  _mobiles[6].x = 305.0;
  _mobiles[6].y = 470.0;
  _mobiles[7].x = 265.0;
  _mobiles[7].y = 470.0;


  _mobiles[8].x = 55.0;
  _mobiles[8].y = 255.0;
  _mobiles[9].x = 55.0;
  _mobiles[9].y = 215.0;
  _mobiles[10].x = 255.0;
  _mobiles[10].y = 10.0;
  _mobiles[11].x = 295.0;
  _mobiles[11].y = 10.0;
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
    fx = _mobiles[i].x + vx * dt;
    fy = _mobiles[i].y + vy * dt;

    if((fx < 0.0f && vx < 0.0f) || (fx >= w && vx > 0.0f)){
      	vx = -vx;
        //_mobiles[i].x = -(vx * dt);
    }
    if((fy < 0.0f && vy < 0.0f)||(fy >= h && vy > 0.0f)){
      	vy = -vy;
    	//_mobiles[i].y = -(vy * dt);
    }
    _mobiles[i].x += vx * dt;
    _mobiles[i].y += vy * dt;
    
 }
}
