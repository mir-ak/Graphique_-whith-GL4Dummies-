/*!\file window.c
 *
 * \brief GL4Dummies, exemple 3D simple avec dessin de rectangle texturé et filtre moyennant.
 * \author Farès BELHADJ, amsi@up8.edu
 * \date February 17 2021
 */

#include <GL4D/gl4dp.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <assert.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(SDL_Surface * s);
static void draw(void);
static void quit(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du (futur) Framebuffer Object */
static GLuint _fbo = 0;
/*!\brief identifiant du (futur) Vertex Array Object */
static GLuint _vao = 0;
/*!\brief identifiant du (futur) buffer de data - VBO ou Vertex Buffer Object */
static GLuint _buffer = 0;
/*!\brief identifiant du (futur) buffer d'indices - VBO ou Vertex Buffer Object */
static GLuint _ibuffer = 0;
/*!\brief identifiants des GLSL programs */
static GLuint _pId[6] = { 0 };
/*!\brief identifiant de la (future) texture originale */
static GLuint _texId = 0;
/*!\brief identifiant des textures liées au FBO */
static GLuint _fboTex[4] = {0,0,0,0};
/*!\brief pas en abscisse et en ordonnée entre deux texels partant de
 * la coordonnée de texture. Calculé dans init. */
static GLfloat _pas[2] = {0, 0};
static GLuint _rand;




/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  SDL_Surface * s = IMG_Load("images/Small_River.png");
  assert(s);
  _wW = s->w;
  _wH = s->h;
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init(s);
  /* effacer la texture du CPU */
  SDL_FreeSurface(s);
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

Uint32 color(int r, int g, int b) {
    return (((r << 8) + g) << 8) + b;
}

void setPixel(SDL_Surface* screen, int x, int y, Uint32 color) {
    SDL_Rect pixel;
    pixel.w = 1, pixel.h = 1;
    pixel.x = x, pixel.y = y;
    SDL_FillRect(screen, &pixel, color);
}
/*!\brief initialise les paramètres OpenGL et les données. 
 *
 * Exercice : texturer le vao.
 */
static void init(SDL_Surface * s) {
  int i;
  /* données envoyées par tranches (différent du mode interleaved
   * array) dans le VBO */
  GLfloat data[] = {
    /* 4 sommets */
    -1.0f, -1.0f, // 0
    0.0f, 0.0f, // coord de texture
    1.0f, 0.0f, 0.0f, // couleur
    1.0f, -1.0f, // 1
    1.0f, 0.0f, // coord de texture
    0.0f, 1.0f, 0.0f, // couleur
    -1.0f,  1.0f, // 2
    0.0f, 1.0f, // coord de texture
    0.0f, 0.0f, 1.0f, // couleur
    1.0f,  1.0f, // 3
    1.0f, 1.0f, // coord de texture
    1.0f, 1.0f, 0.0f // couleur

  };
  GLuint idata[] = {
    0, 1, 2,
    2, 1, 3
  };
  /* Création du 1er program shader (voir le dossier shader) */
  _pId[0] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/segment.fs", NULL);
  /* Création du 2nd program shader (voir le dossier shader) */
  _pId[1] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/sobel.fs", NULL);
  /* Création du 3 eme program shader (voir le dossier shader) */
  _pId[2] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/blur.fs", NULL);
    /* Création du triple program shader (voir le dossier shader) */
  _pId[3] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/mult.fs", NULL);
  /* Création du 4eme program shader (voir le dossier shader) */
  _pId[4] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/bruit.fs", NULL);
  /* Création du 5eme program shader (voir le dossier shader) */
  _pId[5] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/median.fs", NULL);
  
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* dans quelle partie de l'écran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
   SDL_Surface* random = NULL;
  random = SDL_CreateRGBSurface(0, s->w, s->h, 32, 0, 0, 0, 0);
  int Nbrand;
  for (int y = 0; y < s->h; ++y) {
      for (int x = 0; x < s->w; ++x) {
          Nbrand = rand() % 25;
          setPixel(random, x, y, color(0, 0, Nbrand));
      }
  }
  /* Génération d'un identifiant de VAO */
  glGenVertexArrays(1, &_vao);
  /* Lier le VAO-machine-GL à l'identifiant VAO généré */
  glBindVertexArray(_vao);
  /* Activation des 2 premiers indices d'attribut de sommet */
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  /* Génération d'un identifiant de VBO */
  glGenBuffers(1, &_buffer);
  /* Lier le VBO-machine-GL à l'identifiant VBO généré */
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  /* Transfert des données VBO */
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  /* Paramétrage 2 premiers indices d'attribut de sommet */
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (7 * sizeof *data), (const void *)0);  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (7 * sizeof *data), (const void *)(2 * sizeof *data));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (7 * sizeof *data), (const void *)(4 * sizeof *data));

  /* Génération d'un identifiant de VBO d'indices */
  glGenBuffers(1, &_ibuffer);
  /* Lier le VBO-machine-GL à l'identifiant VBO généré */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibuffer);
  /* Transfert des données VBO d'indices */
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof idata, idata, GL_STATIC_DRAW);

  /* dé-lier le VAO et VBO */
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);


  // s->pixels est le tableau brut de pixels (en CPU)
  glGenTextures(1, &_texId);
  glBindTexture(GL_TEXTURE_2D, _texId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  /* transfert CPU->GPU avec description */
  {
    /* d'abord conversion de l'image source, quel que soit son format, en RGBA 32 bits */
    /* s => d */
    SDL_Surface * d = SDL_CreateRGBSurface(0, s->w, s->h, 32, 0xFF/* R_MASK */, 0xFF << 8/* G_MASK */, 0xFF << 16/* B_MASK */, 0xFF << 24/* A_MASK */);
    SDL_BlitSurface(s, NULL, d, NULL);
    /* puis transfert depuis d vers le GPU (en GL) */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, d->w, d->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, d->pixels);
    /* et enfin libérer d dont nous n'avons plus besoin (s aussi, mais ça c'est fait dans le main) */
    SDL_FreeSurface(d);
  }
  _pas[0] = 1.0f / s->w;
  _pas[1] = 1.0f / s->h;
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &_fbo);

  glGenTextures(4, _fboTex);
  for(i = 0; i < 4; ++i) {
    /* paramétrage et description de la texture _fboTex[i] */
    glBindTexture(GL_TEXTURE_2D, _fboTex[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    /* description de la donnée texture */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _wW, _wH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  }
  /* paramétrage et description de la texture _rand */
  glGenTextures(1, &_rand);
  glBindTexture(GL_TEXTURE_2D, _rand);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  /* description de la donnée texture */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, random->w, random->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, random->pixels);

  glBindTexture(GL_TEXTURE_2D, 0);
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  int i;
  GLuint in;
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  /* traitement vers 1ere texture liée au FBO */
  in = _texId;
  i = 0;
  do {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[(i % 2)], 0);
    /* effacement du buffer de couleur */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* activation du programme _pId[0] (le blur) */
    glUseProgram(_pId[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, in);
    glUniform1i(glGetUniformLocation(_pId[0], "tex"), 0); // le 0 correspond à glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(_pId[0], "inv"), in == _texId ? 1 : 0); // inversion de la coordonnée y de la texture
    glUniform2fv(glGetUniformLocation(_pId[0], "pas"), 1, _pas);
    /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
    glBindVertexArray(_vao);
    /* Dessiner le VAO comme un rectangle composé de 2 triangles */
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid *)0);
    glBindTexture(GL_TEXTURE_2D, 0);
    /* dé-lier le VAO */
    glBindVertexArray(0);
    /* désactiver le programme shader */
    glUseProgram(0);
    in = _fboTex[(i % 2)];
    ++i;
  } while( i < 4 );
  
  /* traitement vers 2nd texture liée au FBO */
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[1], 0);
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme _pId[1] (le sobel) */
  glUseProgram(_pId[1]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _fboTex[0]);
  glUniform1i(glGetUniformLocation(_pId[1], "tex"), 0); // le 0 correspond à glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId[1], "inv"), 0); // non-inversion de la coordonnée y de la texture
  glUniform2fv(glGetUniformLocation(_pId[1], "pas"), 1, _pas); 
  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme un rectangle composé de 2 triangles */
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid *)0);
  glBindTexture(GL_TEXTURE_2D, 0);  
  /* dé-lier le VAO */
  glBindVertexArray(0);
  /* désactiver le programme shader */
  glUseProgram(0);
/***************************************************************/

/* traitement vers 3eme texture liée au FBO */

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[2], 0);
	  /* effacement du buffer de couleur */
	  glClear(GL_COLOR_BUFFER_BIT);
	  /* activation du programme _pId[2] (blur) */
	  glUseProgram(_pId[2]);
	  glActiveTexture(GL_TEXTURE0);
	  glBindTexture(GL_TEXTURE_2D, _fboTex[0]);
	  glUniform1i(glGetUniformLocation(_pId[2], "tex"), 0); // le 0 correspond à glActiveTexture(GL_TEXTURE0);
	  glUniform1i(glGetUniformLocation(_pId[2], "inv"), 0); // non-inversion de la coordonnée y de la texture
	  glUniform2fv(glGetUniformLocation(_pId[2], "pas"), 1, _pas); 
	  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
	  glBindVertexArray(_vao);
	  /* Dessiner le VAO comme un rectangle composé de 2 triangles */
	  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid *)0);
	  glBindTexture(GL_TEXTURE_2D, 0);  
	  /* dé-lier le VAO */
	  glBindVertexArray(0);
	  /* désactiver le programme shader */
	  glUseProgram(0);
	  
/*************************************************************/

  /* traitement vers 4ème texture liée au FBO */
  // triple de filtre sobel et blur et segment 
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[3], 0);
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(_pId[3]);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, _fboTex[2]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _fboTex[1]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _fboTex[0]);
  glUniform1i(glGetUniformLocation(_pId[3], "tex0"), 0); // le 0 correspond à glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId[3], "tex1"), 1); // le 1 correspond à glActiveTexture(GL_TEXTURE1);
  glUniform1i(glGetUniformLocation(_pId[3], "tex2"), 2); // le 1 correspond à glActiveTexture(GL_TEXTURE1);
  glUniform1i(glGetUniformLocation(_pId[3], "inv"), 0); // non-inversion de la coordonnée y de la texture
  glUniform2fv(glGetUniformLocation(_pId[3], "pas"), 1, _pas); 
  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme un rectangle composé de 2 triangles */
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid *)0);
  glBindTexture(GL_TEXTURE_2D, 0);  
  /* dé-lier le VAO */
  glBindVertexArray(0);
  /* désactiver le programme shader */
  glUseProgram(0);

/*******************************************************************/
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[1], 0);
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme _pId[1] (le random) */
  glUseProgram(_pId[4]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _fboTex[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _rand);
  glUniform1i(glGetUniformLocation(_pId[4], "tex"), 0); // le 0 correspond à glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId[4], "rand"), 1); // le 0 correspond à glActiveTexture(GL_TEXTURE1);
  glUniform1i(glGetUniformLocation(_pId[4], "inv"), 0); // non-inversion de la coordonnée y de la texture
  glUniform2fv(glGetUniformLocation(_pId[4], "pas"), 1, _pas); 

  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme un rectangle composé de 2 triangles */
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid *)0);
  glBindTexture(GL_TEXTURE_2D, 0);  
  /* dé-lier le VAO */
  glBindVertexArray(0);
  /* désactiver le programme shader */
  glUseProgram(0);

/*******************************************************************/
/* traitement vers 3ème texture liée au FBO */

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[2], 0);
  /* effacement du buffer de couleur */
  glClear(GL_COLOR_BUFFER_BIT);
  /* activation du programme _pId[1] (le medien) */
  glUseProgram(_pId[5]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _fboTex[0]);
  glUniform1i(glGetUniformLocation(_pId[5], "tex0"), 0); // le 0 correspond à glActiveTexture(GL_TEXTURE0);
  glUniform1i(glGetUniformLocation(_pId[5], "inv"), 0); // non-inversion de la coordonnée y de la texture
  glUniform2fv(glGetUniformLocation(_pId[5], "pas"), 1, _pas); 
  /* Lier le VAO-machine-GL à l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme un rectangle composé de 2 triangles */
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid *)0);
  glBindTexture(GL_TEXTURE_2D, 0);  
  /* dé-lier le VAO */
  glBindVertexArray(0);
  /* désactiver le programme shader */
  glUseProgram(0);



/*****************************************************************/


  /* Copie vers l'écran */
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); /* FBO écran est ouvert en écriture */
  glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo); /* FBO _fbo est ouvert en lecture (pas nécessaire) */
  /* copies depuis FBO en lecture vers FBO en écriture (l'écran) */
  /* l'image d'origine dans le coin inférieur-gauche */
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId, 0);
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW / 2, _wH / 2, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  /* l'image floutée dans le coin inférieur-droit */
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[1], 0);
  glBlitFramebuffer(0, 0, _wW, _wH, _wW / 2, 0, _wW, _wH / 2, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  /* l'image de contours dans le coin supérieur-gauche */
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[2], 0);
  glBlitFramebuffer(0, 0, _wW, _wH, 0, _wH / 2, _wW / 2, _wH, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  /* l'image du contour fois le flou dans le coin supérieur-droit */
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[3], 0);
  glBlitFramebuffer(0, 0, _wW, _wH, _wW / 2, _wH / 2, _wW, _wH, GL_COLOR_BUFFER_BIT, GL_NEAREST);
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
  if(_ibuffer)
    glDeleteBuffers(1, &_ibuffer);
  /* suppression de la texture de l'image d'origine */
  if(_texId)
    glDeleteTextures(1, &_texId);
  /* suppression des textures liées au FBO */
  if(_fboTex[0])
    glDeleteTextures(4, _fboTex);
  if(_fbo)
      glDeleteFramebuffers(1, &_fbo);
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
