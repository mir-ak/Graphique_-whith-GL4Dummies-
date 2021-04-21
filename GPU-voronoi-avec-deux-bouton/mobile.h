/*!\file mobile.h 
 *
 *\brief Mobiles dessinés à l'aide de disques.
 *
 * \author Farès BELHADJ, amsi@up8.edu
 * \date June 03 2020
 */

#ifndef MOBILE_H_INCLUDED
#  define MOBILE_H_INCLUDED

#include <GL4D/gl4dummies.h>

#  ifdef __cpluplus
extern "C" {
#  endif

  extern void mobileInit(int nb_mobiles);
  extern void mobileFree(void);
  extern void mobileMove(void);
  extern void mobile2Data(void);

#  ifdef __cpluplus
}
#  endif

#endif
