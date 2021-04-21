/*!\file mobile.c
 *
 *\brief Simulation basique de déplacement de mobiles dessinés à
 *l'aide d'un disque.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date June 03 2020
 */

#include "mobile.h"
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4dp.h>

/* \brief type de donnée pour un mobile */
typedef struct mobile_t mobile_t;

/* \brief structure de données du mobile : coordonnée 2d flottante,
 * vitesse 2d flottante et couleur. */
struct mobile_t {
  float x, y;
  float vx, vy;
  GLuint color;
};

/* \brief image (SDL) qui servira de "fond" pour la simulation */
static SDL_Surface * _background = NULL;
/* \brief l'ensemble des mobiles */
static mobile_t * _mobiles = NULL;
/* \brief le nombre de mobile dans la simulation */
static int _nb_mobiles = 0;
/* \brief le rayon constant pour tous les mobiles */
static const int _radius = 3;
/* \brief la couleur d'effacement */
static const GLuint _libre = 0x0;

/* prototypes de fonctions statiques définies plus bas */
static void drawDisk(int x0, int y0, int r);
static void updateColor(int mobileIndex);

/* \brief initialise (ou réinitialise) l'ensemble des mobiles de la
 * simulation, leur nombre est donné par \a nb_mobiles.
 *
 * \param nb_mobiles le nombre de mobiles de la simulation.
 */
void mobileInit(int nb_mobiles, SDL_Surface * bg) {
  int i, w = gl4dpGetWidth(), h = gl4dpGetHeight();
  GLuint * screen = gl4dpGetPixels();
  _background = bg;
  if(_mobiles) {
    mobileFree();
  } else {
    atexit(mobileFree);
  }
  _nb_mobiles = nb_mobiles;
  _mobiles = malloc(_nb_mobiles * sizeof *_mobiles);
  assert(_mobiles);
  for(i = 0; i < _nb_mobiles; ++i) {
    do {
      _mobiles[i].x = (float) (gl4dmURand() * w);
      _mobiles[i].y = (float) (gl4dmURand() * h);
    } while(screen[((int)_mobiles[i].y) * w + ((int)_mobiles[i].x)] != _libre);
    _mobiles[i].vx = (float) (gl4dmSURand() * 20.0);
    _mobiles[i].vy = (float) (gl4dmSURand() * 20.0);
    updateColor(i);
  }
}

/* \brief libère la mémoire et réinitialise les paramètres de la
 *  simulation. Cette fonction est potentiellement appelée à
 *  l'événement exit.
 */
void mobileFree(void) {
  if(_mobiles) {
    free(_mobiles);
    _mobiles = NULL;
    _nb_mobiles = 0;
  }
}

/* \brief déplace tous les mobiles en fonction de leur position,
 * vitesse et du temps.
 */
void mobileMove(void) {
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
    if((fx < _radius && _mobiles[i].vx < 0.0f) ||
       (fx > w - _radius && _mobiles[i].vx > 0.0f))
      _mobiles[i].vx = -_mobiles[i].vx;
    if((fy < _radius && _mobiles[i].vy < 0.0f)||
       (fy > h - _radius && _mobiles[i].vy > 0.0f))
      _mobiles[i].vy = -_mobiles[i].vy;
    _mobiles[i].x += _mobiles[i].vx * dt;
    _mobiles[i].y += _mobiles[i].vy * dt;
    updateColor(i);
  }
}

/* \brief dessine tous les mobiles en utilisant \ref drawDisk, la
 * position et le rayon \ref _radius.
 */
void mobileDraw(void) {
  int i;
  gl4dpClearScreenWith(_libre);
  for(i = 0; i < _nb_mobiles; ++i) {
    gl4dpSetColor(_mobiles[i].color);
    drawDisk((int)_mobiles[i].x, (int)_mobiles[i].y, _radius);
  }
  gl4dpScreenHasChanged();
}

/* \brief dessine un segment horizontal du point (\a x0, \a y) au
 * point (\a x1, \a y). */
static void drawHLine(int x0, int x1, int y) {
  int w = gl4dpGetWidth();
  int h = gl4dpGetHeight();
  int yw = y * w;
  GLuint * screen = gl4dpGetPixels();
  GLuint c = gl4dpGetColor();
  assert(x0 <= x1);
  if(y < 0 || y >= h) return;
  if(x0 < 0 && x1 < 0) return;
  if(x0 >= w && x1 >= w) return;
  x0 = MAX(x0, 0);
  x1 = MIN(x1, w - 1);
  for(; x0 <= x1; ++x0)
    screen[yw + x0] = c;
}

/* \brief dessine un disque en (\a x0, \a y0) de rayon \a r. Cette
 * fonction utilise \ref drawHLine pour remplir ligne par ligne le
 * cercle et en faire un disque. */
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

/* \brief récupère pour le mobile \a mobileIndex la couleur présente
 *  dans l'image \ref _background */
static void updateColor(int mobileIndex) {
  Uint8 r, g, b;
  Uint32 * p = (Uint32 *)_background->pixels;
  int x = ((int)_mobiles[mobileIndex].x);
  int y = _background->h - (((int)_mobiles[mobileIndex].y)+1);
  SDL_GetRGB(p[y * _background->w + x], _background->format, &r, &g, &b);
  _mobiles[mobileIndex].color = RGB(r, g, b);
}
