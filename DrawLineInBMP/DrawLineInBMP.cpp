/*!\file DrawLineInBmp.cpp
 *
 * \brief dessin d'un segement de droite en mémoire et sauvegarde dans un fichier BMP.
 *
 * Ce code ne fonctionne que pour une droite définie dans le premier
 * octan.
 *
 * Voir les exercices en fin de fichier.
 *
 * \author Farès Belhadj amsi@up8.edu
 * \date May 26, 2020
 */
#include <iostream>
#include <string.h> /* pour memset */
#include <math.h>
#include "images_io.h"

#define W 80
#define H 60

/* drawLine corrigée le 29 mai 2020 */

static void drawLine(unsigned char * image, int w, int x0, int y0, int x1, int y1, unsigned char color) {
  float y = 0.0f, pente;
  int u = x1 - x0, v = y1 - y0, pas, depassement;
  if(abs(u) > abs(v)) { // 1er octan
    pente = v / (float)abs(u);
    pas = u < 0 ? -1 : 1;
    depassement = u + pas;
    for (int x = 0; x != depassement; x += pas) {
      int nx = x + x0;
      int ny = ((int)y) + y0;
      if(nx >= 0 && nx < W && ny >= 0 && ny < H)
	image[ny * w + nx] = color;
      y += pente;
    }
  } else { // 2eme octan
    pente = u / (float)abs(v);
    pas = v < 0 ? -1 : 1;
    depassement = v + pas;
    for (int x = 0; x != depassement; x += pas) {
      int nx = ((int)y) + x0;
      int ny = x + y0;
      if(nx >= 0 && nx < W && ny >= 0 && ny < H)
	image[ny * w + nx] = color;
      y += pente;
    }
  }
}

/* drawCircle avec une sous-fonction testAndPlot  */

static void testAndPlot(unsigned char * image, int w, int x, int y, unsigned char color) {
  if(x >= 0 && x < W && y >= 0 && y < H)
    image[y * w + x] = color;
}

static void drawCircle(unsigned char * image, int w, int x0, int y0, int r, unsigned char color) {
  int x, r2 = r * r;
  float y, finx = ((int)ceil((double)r * M_SQRT1_2)) + 1;
  for(x = 0; x < finx; ++x) {
    y = sqrt(r2 - x * x) + 0.5f;
    testAndPlot(image, w,  x + x0,  ((int)y) + y0, color); // 2eme octan
    testAndPlot(image, w, -x + x0,  ((int)y) + y0, color); // 3eme octan
    testAndPlot(image, w,  x + x0, -((int)y) + y0, color); // 7eme octan
    testAndPlot(image, w, -x + x0, -((int)y) + y0, color); // 6eme octan
    testAndPlot(image, w,  y + x0,  ((int)x) + y0, color); // 1eme octan
    testAndPlot(image, w, -y + x0,  ((int)x) + y0, color); // 4eme octan
    testAndPlot(image, w,  y + x0, -((int)x) + y0, color); // 8eme octan
    testAndPlot(image, w, -y + x0, -((int)x) + y0, color); // 5eme octan
  }
}

/* Des tests pour monter comment dessiner une fonction quelconque 
 * version complètement non optimale
 */

void experimentation(unsigned char * image) {
  float x, y, x0 = 40.0f, y0 = 30.0f, r = 29.0;
  for(x = 0; x < W; x += .01f) {
    //y = (H >> 1) + ((H >> 1) - 1.0f) * sin(x / 10.0f);
    y = sqrt(r * r - (x - x0) * (x - x0)) + y0;
    image[((int)y) * W + ((int)x)] = 255;
    y = -sqrt(r * r - (x - x0) * (x - x0)) + y0;
    image[((int)y) * W + ((int)x)] = 255;
  }

  // (x - x0)^2 + (y - y0)^2 = r^2;
  // (y - y0)^2 = r^2 - (x - x0)^2;
  // y - y0 = sqrt(r^2 - (x - x0)^2);
  // y = sqrt(r^2 - (x - x0)^2) + y0;
}

int main(void) {
  /* on part sur une image 8 bits / 256 niveaux de gris */
  unsigned char image[H * W];
  double alpha;
  /* on met tous les pixels à zéro */
  memset(image, 0, sizeof image);
  /* on dessine une droite de (10, 20) à (600, 400) 
   * la couleur utilisée est le blanc (255)
   */
  //drawLine(image, W, 5, 10, 70, 50, 255);
  //drawLine(image, W, 75, 10, 0, 50, 255);
  //drawLine(image, W, 10, 10, 20, 50, 255);
  //drawLine(image, W, -10, -10, 120, 150, 255);

  for(alpha = 0; alpha < 2.0 * M_PI; alpha += 0.1) {
    drawLine( image, W,
  	      (W >> 1), (H >> 1), /* x0, y0 */
  	      (W >> 1) + 25 * cos(alpha), (H >> 1) + 25 * sin(alpha), /* x1, y1 */
	      rand()&255 );
  }
  drawCircle( image, W, (W >> 1), (H >> 1), (H >> 1) - H / 10, 255 );
  //experimentation(image);

  imageSaveBMP("resu.bmp", image, W, H, 1, 8);
  return EXIT_SUCCESS;
}

/*
  Exercices :

  exercice 1 : essayez de dessiner le segment (75, 10, 0, 50, 255), ça ne fonctionne pas. Corriger drawLine.
  exercice 2 : essayez de dessiner le segment (10, 10, 20, 50, 255), ça ne fonctionne pas. Corriger drawLine.
  y = f(x) = p * x,
  x = (1/p) * y
  exercice 3 : essayez de dessiner le segment (-10, -10, 120, 150, 255), ça ne fonctionne pas. Corriger drawLine.
  exercice 4 : dessiner (si ça marche) toute les positions d'une aiguille trotteuse en variant les intensités de gris (vous pouvez utiliser cos et sin dans math.h).
  exercice 5 : l'équation d'un cercle est donnée par y = (+ ou -) sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0)); écrire drawCircle.
*/
