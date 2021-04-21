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
  GLfloat x, y;
  GLfloat r, g, b;
  GLfloat vx, vy;
};

/* \brief l'ensemble des mobiles */
static mobile_t * _mobiles = NULL;
/* \brief le nombre de mobile dans la simulation */
static int _nb_mobiles = 0;

/* identifiant de texture OpenGL */
static GLuint _texId = 0;

/* \brief initialise (ou réinitialise) l'ensemble des mobiles de la
 * simulation, leur nombre est donné par \a nb_mobiles.
 *
 * \param nb_mobiles le nombre de mobiles de la simulation.
 */
void mobileInit(int nb_mobiles) {
  int i;
  if(_mobiles) {
    mobileFree();
  } else {
    atexit(mobileFree);
  }
  _nb_mobiles = nb_mobiles;
  _mobiles = malloc(_nb_mobiles * sizeof *_mobiles);
  assert(_mobiles);
  for(i = 0; i < _nb_mobiles; ++i) {
    _mobiles[i].x = (GLfloat)gl4dmURand();
    _mobiles[i].y = (GLfloat)gl4dmURand();
    _mobiles[i].r = (GLfloat)gl4dmURand();
    _mobiles[i].g = (GLfloat)gl4dmURand();
    _mobiles[i].b = (GLfloat)gl4dmURand();
    _mobiles[i].vx = (float) (gl4dmSURand() * 0.5);
    _mobiles[i].vy = (float) (gl4dmSURand() * 0.5);
  }
  glGenTextures(1, &_texId);
  glBindTexture(GL_TEXTURE_1D, _texId);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_1D, 0);
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
  if(_texId) {
    glDeleteTextures(1, &_texId);
    _texId = 0;
  }
}

/* \brief déplace tous les mobiles en fonction de leur position,
 * vitesse et du temps.
 */
void mobileMove(void) {
  int i;
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
       (fx > 1.0f && _mobiles[i].vx > 0.0f))
      _mobiles[i].vx = -_mobiles[i].vx;
    if((fy < 0.0f && _mobiles[i].vy < 0.0f)||
       (fy > 1.0f && _mobiles[i].vy > 0.0f))
      _mobiles[i].vy = -_mobiles[i].vy;
    _mobiles[i].x += _mobiles[i].vx * dt;
    _mobiles[i].y += _mobiles[i].vy * dt;
  }
}

void mobile2Data(void) {
  GLfloat * data = calloc(2 * 4 * _nb_mobiles, sizeof *data);
  assert(data);
  for(int i = 0; i < _nb_mobiles; ++i) {
    /* texel 1 */
    data[8 * i + 0] = _mobiles[i].x;
    data[8 * i + 1] = _mobiles[i].y;
    /* texel 2 */
    data[8 * i + 4] = _mobiles[i].r;
    data[8 * i + 5] = _mobiles[i].g;
    data[8 * i + 6] = _mobiles[i].b;    
  }
  /* activer la texture _texId */
  glBindTexture(GL_TEXTURE_1D, _texId);
  /* transférer depuis CPU à GPU */  
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 2 * _nb_mobiles, 0, GL_RGBA, GL_FLOAT, data);
  free(data);
}

