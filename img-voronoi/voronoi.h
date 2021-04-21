/*!\file voronoi.h 
 *
 *\brief Diagramme de voronoi par cercles grossissants.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date June 03 2020
 */

#ifndef VORONOI_H_INCLUDED
#  define VORONOI_H_INCLUDED
#include <GL4D/gl4dummies.h>
#  ifdef __cpluplus
extern "C" {
#  endif

  extern void vdInit(int nb_sites,SDL_Surface * bg);
  extern void vdFree(void);
  extern int  vdGrow(void);
  extern void vdDraw(void);
  extern void Move_voronoi(void);

#  ifdef __cpluplus
}
#  endif

#endif
