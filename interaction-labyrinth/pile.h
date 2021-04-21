/*!\file pile.h
 * \brief Bibliothèque de gestion de (une) pile de taille fixe
 * \author Farès Belhadj amsi@up8.edu
 * \date September 27, 2019
 */
#ifndef _PILE_H
#define _PILE_H
/*!\brief taille de la pile (statique) */
#define PILE_MAX 8192

#ifdef __cplusplus
extern "C" {
#endif
  
  extern void push(int v);
  extern int  pop(void);
  extern int  empty(void);
  
#ifdef __cplusplus
}
#endif

#endif
