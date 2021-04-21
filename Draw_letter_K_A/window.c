/*!\file window.c
 *
 * \brief GL4Dummies, exemple 3D simple avec dessin d'un quadrilatère
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 11 2018
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du (futur) Vertex Array Object */
static GLuint _vao = 0;
/*!\brief identifiant du (futur) buffer de data - VBO ou Vertex Buffer Object */
static GLuint _buffer = 0;
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;

/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL et les données. 
 *
 * Exercice (corrigé en 1.2) : entrelacer les données de sommet dans
 * data afin d'avoir la 1ere coordonnée spaciale suivie de sa couleur,
 * suivi de la 2ème coordonnée ... modifier les appels à
 * glVertexAttribPointer en corrigeant le 5ème et 6ème argument (voir
 * doc de glVertexAttribPointer sur
 * https://www.khronos.org/registry/OpenGL-Refpages/gl4).
 *
 * puis ajouter une variable d'angle (en radians) qui s'incrémente
 * dans draw, envoyer cette variable à GLSL (le vertex shader) comme
 * variable "uniform float angle;" en utilisant les fonctions
 * glGetUniformLocation et glUniform1f (voir les doc sur
 * https://www.khronos.org/registry/OpenGL-Refpages/gl4).  Enfin
 * utiliser cet angle côté vertex shader pour construire une matrice
 * de rotation 4x4 autour de l'axe y et l'utiliser sur le sommet.
 */
static void init(void) {
  /* données envoyées par tranches (différent du mode interleaved
   * array) dans le VBO */
  GLfloat data[] = {
    /* 4 coordonnées de sommets en 2D */
    // la lettre A 

    -0.1f,  0.5f, 
    -0.4f, -0.5f, 
    -0.2f, -0.5f,

    -0.1f,  0.5f, 
    -0.2f, -0.5f,
     0.1f,  0.5f,
  
    -0.1f,  0.5f,
     0.1f,  0.5f,
     0.2f, -0.5f,
    
     0.1f,  0.5f,
     0.2f, -0.5f,
     0.4f, -0.5f,
  
    -0.082f,-0.1f, 
     0.082f,-0.1f, 
    -0.12f, -0.23f,

     0.082f,-0.1f, 
    -0.12f, -0.23f,
     0.12f, -0.23f,

    // la lettre K

    -0.4f,  0.5f, 
    -0.2f,  0.5f, 
    -0.4f, -0.5f,

    -0.2f,  0.5f, 
    -0.4f, -0.5f,
    -0.2f, -0.5f,
  
    -0.2f,  0.0f,
    -0.2f, -0.2f,
     0.2f,  0.5f,
    
     0.2f,  0.5f,
     0.4f,  0.5f,
    -0.2f, -0.2f,

    -0.084f,-0.065f, 
     0.034f,0.074f, 
     0.4f, -0.5f,

     0.4f, -0.5f,
    -0.084f,-0.065f, 
     0.2f, -0.5f,

    /*couleurs pour A*/

    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,

    /*couleurs pour K */

    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, 0.0f, 0.0f, 
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    
  };
  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
   /* Création des matrices GL4Dummies, une pour la projection, une pour la modélisation-vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  /* on active la matrice de projection créée précédemment */
  gl4duBindMatrix("projectionMatrix");
  /* la matrice en cours reçoit identité (matrice avec des 1 sur la
   * diagonale et que des 0 sur le reste) */
  gl4duLoadIdentityf();
  /* on multiplie la matrice en cours par une matrice de projection
   * orthographique ou perspective */
  /* décommenter pour orthographique gl4duOrthof(-1, 1, -1, 1, 0, 100); */
  gl4duFrustumf(-1, 1, -1, 1, 2, 100);
  /* dans quelle partie de l'écran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
  /* Génération d'un identifiant de VAO */
  glGenVertexArrays(1, &_vao);
  /* Lier le VAO-machine-GL à l'identifiant VAO généré */
  glBindVertexArray(_vao);
  /* Activation des 2 premiers indices d'attribut de sommet */
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  /* Génération d'un identifiant de VBO */
  glGenBuffers(1, &_buffer);
  /* Lier le VBO-machine-GL à l'identifiant VBO généré */
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  /* Transfert des données VBO */
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  /* Paramétrage 2 premiers indices d'attribut de sommet */
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(36 * 2 * sizeof *data));
  /* dé-lier le VAO et VBO */
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
   static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
   /* lier (mettre en avant ou "courante") la matrice modélisation-vue
   * créée dans init */
  gl4duBindMatrix("modelViewMatrix");
  gl4duPushMatrix();
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* Composer la matrice courante avec une translation (voir la
   * projection perspective dans le manuel pour comprendre pourquoi
   * nous devons éloigner de -3 en z les objets dessinés) */
  gl4duTranslatef(0.9, 0, -3);
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  /* on transmet la variable d'angle en l'intégrant dans la matrice de
   * modélisation-vue. Soit composer la matrice courante avec une
   * rotation "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 0, 0, 1);
  /* on incrémente angle d'un 1/60 de tour soit (360° x 1/60). Tester
   * l'application en activant/désactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que résultat soit toujours le même. */
  angle += (1.0f / 60.0f) * 360.0f;
  gl4duSendMatrices();  
  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme une bande de deux triangles avec 18 sommets commençant à 0 */
  for (int i = 0; i < 5 ; i++)
  {
    glDrawArrays(GL_TRIANGLES, i*3, 6);
  }
  
  gl4duPopMatrix();
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
   /* Composer la matrice courante avec une translation (voir la
   * projection perspective dans le manuel pour comprendre pourquoi
   * nous devons éloigner de -3 en z les objets dessinés) */
  gl4duTranslatef(-0.9, 0, -3);
   /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  /* on transmet la variable d'angle en l'intégrant dans la matrice de
   * modélisation-vue. Soit composer la matrice courante avec une
   * rotation "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 0, 1, 0);
  /* on incrémente angle d'un 1/60 de tour soit (360° x 1/60). Tester
   * l'application en activant/désactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que résultat soit toujours le même. */
  gl4duSendMatrices();  
  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
  glBindVertexArray(_vao);
   /* Dessiner le VAO comme une bande de deux triangles avec 18 sommets commençant à 0 */
   for (int i = 6; i < 11  ; i++)
  {
    glDrawArrays(GL_TRIANGLES, i*3, 6);
  } 

  /* dé-lier le VAO */
  glBindVertexArray(0);
  /* désactiver le programme shader */
  glUseProgram(0);
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
static void quit(void) {
  /* suppression du VAO _vao en GPU */
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  /* suppression du VBO _buffer en GPU */
  if(_buffer)
    glDeleteBuffers(1, &_buffer);
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
