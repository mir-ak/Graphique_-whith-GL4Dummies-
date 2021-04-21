/*!\file pile.c
 * \brief Bibliothèque de gestion de (une) pile de taille fixe
 * \author Farès Belhadj amsi@up8.edu
 * \date September 27, 2019
 */
#include "pile.h"
#include <stdlib.h>
#include <assert.h>

/*! \brief indice indiquant le haut de la pile. */
static int _haut = -1;
/*! \brief tableau static utilisé pour le stockage de la pile. */
static int _pile[PILE_MAX];

/*!\brief Empiler la valeur \a v dans la pile.
 * \param v la valeur à empiler */
void push(int v) {
  _pile[++_haut] = v;
}

/*!\brief dépiler et renvoyer la valeur de l'élément en haut de la pile.
 * \return la valeur en haut de la pile. */
int pop(void) {
  return _pile[_haut--];
}

/*!\brief Indique si la pile est vide.
 * \return vrai si la pile est vide, faux sinon. */
int empty(void) {
  return _haut < 0;
}
