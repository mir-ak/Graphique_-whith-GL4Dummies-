/*!\file voronoi.c
 *
 *\brief Diagramme de voronoi par cercles grossissants.
 *
 * Ce code est à compléter/corriger, cela fait partie du DM. Une fois
 * corrigé, passer le numéro de version du projet à 1.0. Normalement à
 * chaque fin de création d'un diagramme, un nouveau est relancé (voir
 * window.c / fonction dessin). La fin n'est pas détectée car
 * drawCircleNoM ne fait pas le JOB.
 *
 * Question subsidiaire : ajouter un point presque noir (ex. un tout
 * petit disque coloré en RGB(1, 1, 1)) pour marquer la position du
 * site quand ce dernier passe en état FINISHED.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date June 03 2020
 */

#include "voronoi.h"
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4dp.h>

static int drawCircleNoM(int x0, int y0, int r);

/* \brief énumération des états possibles pour une cellule */
enum vd_state_t {
		GROWING,
		FINISHED
};

/* \brief type de donnée pour une cellule */
typedef struct vd_site_t vd_site_t;

/* \brief structure de données de la cellule : coordonnée 2d, couleur
 * et état. */
struct vd_site_t {
  int x, y;
  float vx ,vy; 
  GLuint color;
  enum vd_state_t state;
};

/* \brief image (SDL) qui servira de "fond" pour la simulation */
static SDL_Surface * _background = NULL;
/* \brief le diagramme de voronoi */
static vd_site_t * _sites = NULL;
/* \brief le nombre de sites du diagramme de voronoi */
static int _nb_sites = 0;
/* \brief le rayon en cours pour tous les sites */
static int _radius = 3;
/* \brief la couleur pour les positions non occupées */
static const GLuint _libre = 0x0;
/* prototypes de fonctions statiques définies plus bas */
//static void drawDisk(int x0, int y0, int r);
/* \brief récupère pour le mobile \a mobileIndex la couleur présente
 *  dans l'image \ref _background */
static void updateColor(int siteIndex) {
  Uint8 r, g, b;
  Uint32 * p = (Uint32 *)_background->pixels;
  int x = ((int)_sites[siteIndex].x);
  int y = _background->h - (((int)_sites[siteIndex].y)+1);
  SDL_GetRGB(p[y * _background->w + x], _background->format, &r, &g, &b);
  _sites[siteIndex].color = RGB(r, g, b);
}
/* \brief initialise (ou réinitialise) le diagramme de Voronoi avec un
 * nombre de sites égal à \a nb_sites.
 *
 * \param nb_sites le nombre de sites de ce diagramme.
 */
void vdInit(int nb_sites,SDL_Surface * bg) {
  int i, w = gl4dpGetWidth(), h = gl4dpGetHeight();
  GLuint * screen = gl4dpGetPixels();
  _background = bg;
  if(_sites) {
    vdFree();
  } else {
    atexit(vdFree);
  }
  _nb_sites = nb_sites;
  _sites = malloc(_nb_sites * sizeof *_sites);
  assert(_sites);
  _radius = 1;
  gl4dpClearScreenWith(_libre);
  for(i = 0; i < _nb_sites; ++i) {
    do {
      _sites[i].x = (int) (gl4dmURand() * w);
      _sites[i].y = (int) (gl4dmURand() * h);
    } while(screen[_sites[i].y * w + _sites[i].x] != _libre);
   do {
      GLubyte r, g, b;
      r = (GLubyte) (gl4dmURand() * 256);
      g = (GLubyte) (gl4dmURand() * 256);
      b = (GLubyte) (gl4dmURand() * 256);
      _sites[i].color = RGB(r, g, b);
    } while(_sites[i].color == _libre);
    _sites[i].state = GROWING;
    _sites[i].vx = (float) (gl4dmSURand() * 20.0);
    _sites[i].vy = (float) (gl4dmSURand() * 20.0);
    updateColor(i);
  }
  gl4dpScreenHasChanged();
}

/* \brief libère la mémoire et réinitialise les paramètres du
 * diagramme de Voronoi. Cette fonction est potentiellement appelée à
 *  l'événement exit.
 */
void vdFree(void) {
  if(_sites) {
    free(_sites);
    _sites = NULL;
    _nb_sites = 0;
  }
}


/* \brief déplace tous les mobiles en fonction de leur position,
 * vitesse et du temps.
 */
void Move_voronoi(void) {
  gl4dpScreenHasChanged();
  
  int i, w = gl4dpGetWidth(), h = gl4dpGetHeight();
  /* récupération du dt */
  static double t0 = 0.1;
  double t = gl4dGetElapsedTime() / 1000.0;
  double dt = t - t0;
  t0 = t;
  /* fin de récupération du dt */
  for(i = 0; i < _nb_sites ; ++i) {
    /* futur x et futur y */
    updateColor(i);
    float fx, fy;
    fx = _sites[i].x + _sites[i].vx * dt;
    fy = _sites[i].y + _sites[i].vy * dt;
    if((fx < _radius && _sites[i].vx < 0.0f) ||
       (fx > w - _radius && _sites[i].vx > 0.0f))
      _sites[i].vx = -_sites[i].vx;
    if((fy < _radius && _sites[i].vy < 0.0f)||
       (fy > h - _radius && _sites[i].vy > 0.0f))
      _sites[i].vy = -_sites[i].vy;
    _sites[i].x += _sites[i].vx * dt;
    _sites[i].y += _sites[i].vy * dt;
    updateColor(i);
  }
}

/* \brief tente pour toutes les cellules du diagramme de les faire
 * grossir en utilisant un rayon supérieur de +1 à la précédente
 * itération.
 *
 * \return vrai (1) si au moins un site est toujours dans l'état
 * growing, faux (0) sinon.
 */
int vdGrow(void) {
  int i, ret = 0;
  for(i = 0; i < _nb_sites; ++i) {
    if(_sites[i].state == FINISHED) continue;
    gl4dpSetColor(_sites[i].color);
    if(drawCircleNoM(_sites[i].x, _sites[i].y, _radius)) /* au moins un pixel dessiné */
      ret = 1;
    else /* aucun pixel dessiné pour ce site */
      _sites[i].state = FINISHED; 
  }
  ++_radius;
  return ret;
}
void vdDraw(void) {
  int i;
  for(i = 0; i < _nb_sites; ++i){
      _sites[i].state = GROWING;
    }
_radius = 1;
  gl4dpClearScreenWith(_libre);
  while(vdGrow());
  gl4dpScreenHasChanged();
}



/* \brief teste si on peut dessiner à une position de l'écran et met 1
 * dans \a ret si réussite.
 *
 * Cette fonction est à modifier (soit le retour, soit les paramètres)
 * pour qu'elle indique si elle a réussi à faire son plot !
 *
 * \param x l'abscisse du point.
 * \param y l'ordonnée du point.
 */
static inline void testAndPlot(int x, int y,int *tmp) {
  if(IN_SCREEN(x, y) && gl4dpGetPixel(x, y) == _libre){
    gl4dpPutPixel(x, y);
    *tmp = 1 ;
  }
}

/* \brief tente de dessiner un cercle sans moiré. Les pixels sont
 * coloriés que si la position est marquée libre.
 *
 *
 * Cette fonction est à modifier pour que son retour indique si elle a
 * réussi à faire au moins un plot !
 *
 * \param x0 l'abscisse du centre du cercle.
 * \param y0 l'ordonnée du centre du cercle.
 * \param le rayon du cercle.
 * \return vrai (1) si au moins un point est dessiné, faux (0) sinon.
 */
static int drawCircleNoM(int x0, int y0, int r) {
  int x, r2 = r * r;
  float y, finx = ((int)ceil((double)r * M_SQRT1_2)) + 1;
  int yp = r;
  int ret = 0; /* retour de la fonction, 1 si au moins un pixel est dessiné */
  for(x = 0; x < finx; ++x) {
    y = sqrt(r2 - x * x);
    if(yp != ((int)y)) {
      yp = (int)y;
      testAndPlot(  (x - 1) + x0,        yp + y0,&ret); // 2eme octan
      testAndPlot( -(x - 1) + x0,        yp + y0,&ret); // 3eme octan
      testAndPlot(  (x - 1) + x0,       -yp + y0,&ret); // 7eme octan
      testAndPlot( -(x - 1) + x0,       -yp + y0,&ret); // 6eme octan
      testAndPlot(        y + x0,   (x - 1) + y0,&ret); // 1eme octan
      testAndPlot(       -y + x0,   (x - 1) + y0,&ret); // 4eme octan
      testAndPlot(        y + x0,  -(x - 1) + y0,&ret); // 8eme octan
      testAndPlot(       -y + x0,  -(x - 1) + y0,&ret); // 5eme octan      
    }
    testAndPlot(  x + x0,  ((int)y) + y0,&ret); // 2eme octan
    testAndPlot( -x + x0,  ((int)y) + y0,&ret); // 3eme octan
    testAndPlot(  x + x0, -((int)y) + y0,&ret); // 7eme octan
    testAndPlot( -x + x0, -((int)y) + y0,&ret); // 6eme octan
    testAndPlot(  y + x0,         x + y0,&ret); // 1eme octan
    testAndPlot( -y + x0,         x + y0,&ret); // 4eme octan
    testAndPlot(  y + x0,        -x + y0,&ret); // 8eme octan
    testAndPlot( -y + x0,        -x + y0,&ret); // 5eme octan

    /* ATTENTION ! Cette ligne ci-après est à supprimer car ici on
     * force le retour à 1 sans avoir réellement testé. */

  }
  return ret; 
}

