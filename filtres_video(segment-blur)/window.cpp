/*!\file window.c
 *
 * \brief Utilisation de la SDL2/GL4Dummies et d'OpenGL 3+
 *
 * \author Far�s BELHADJ, amsi@ai.univ-paris8.fr
 * \date April 24 2014
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <GL4D/gl4du.h>


using namespace cv;
using namespace std;
/*
 * Prototypes des fonctions statiques contenues dans ce fichier C
 */
static SDL_Window * initWindow(int w, int h, SDL_GLContext * poglContext);
static void initGL(SDL_Window * win);
static void initData(void);
static void resizeGL(SDL_Window * win);
static void loop(SDL_Window * win);
static void manageEvents(SDL_Window * win);
static void draw(void);
static void quit(void);

/*!\brief dimensions de la fenêtre */
static GLfloat _dim[] = {640, 480};
/*!\brief pointeur vers la (future) fenêtre SDL */
static SDL_Window * _win = NULL;
/*!\brief pointeur vers le (futur) contexte OpenGL */
static SDL_GLContext _oglContext = NULL;
/*!\brief identifiant du (futur) vertex array object */
static GLuint _vao = 0;
/*!\brief identifiant du (futur) buffer de data */
static GLuint _buffer = 0;
/*!\brief identifiants des (futurs) GLSL programs */
static GLuint _pId[2] = { 0 };
/*!\brief identifiant de la texture charg�e */
static GLuint _tId = 0;
/*!\brief identifiant des textures liées au FBO */
static GLuint _fboTex[2] = {0, 0};
/*!\brief device de capture vid�o */
static VideoCapture * _cap = NULL;

/*!\brief La fonction principale initialise la biblioth�que SDL2,
 * demande la cr�ation de la fenêtre SDL et du contexte OpenGL par
 * l'appel � \ref initWindow, initialise OpenGL avec \ref initGL et
 * lance la boucle (infinie) principale.
 */
int main(int argc, char ** argv) {
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Erreur lors de l'initialisation de SDL :  %s", SDL_GetError());
    return -1;
  }
  atexit(SDL_Quit);
  if((_win = initWindow(_dim[0], _dim[1], &_oglContext))) {
    atexit(quit);
    gl4duInit(argc, argv);
    initGL(_win);
    _pId[0] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
    _pId[1] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/segment.fs", NULL);
    
    initData();
    loop(_win);
  }
  return 0;
}

/*!\brief Cette fonction cr�� la fenêtre SDL de largeur \a w et de
 *  hauteur \a h, le contexte OpenGL \ref et stocke le pointeur dans
 *  poglContext. Elle retourne le pointeur vers la fenêtre SDL.
 *
 * Le contexte OpenGL cr�� est en version 3 pour
 * SDL_GL_CONTEXT_MAJOR_VERSION, en version 2 pour
 * SDL_GL_CONTEXT_MINOR_VERSION et en SDL_GL_CONTEXT_PROFILE_CORE
 * concernant le profile. Le double buffer est activ� et le buffer de
 * profondeur est en 24 bits.
 *
 * \param w la largeur de la fenêtre � cr�er.
 * \param h la hauteur de la fenêtre � cr�er.
 * \param poglContext le pointeur vers la case o� sera r�f�renc� le
 * contexte OpenGL cr��.
 * \return le pointeur vers la fenêtre SDL si tout se passe comme
 * pr�vu, NULL sinon.
 */
static SDL_Window * initWindow(int w, int h, SDL_GLContext * poglContext) {
  SDL_Window * win = NULL;
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  if( (win = SDL_CreateWindow("GLSLExample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			      w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | 
			      SDL_WINDOW_SHOWN)) == NULL )
    return NULL;
  if( (*poglContext = SDL_GL_CreateContext(win)) == NULL ) {
    SDL_DestroyWindow(win);
    return NULL;
  }
  fprintf(stderr, "Version d'OpenGL : %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "Version de shaders supportes : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));  
  return win;
}

/*!\brief Cette fonction initialise les param�tres OpenGL.
 *
 * \param win le pointeur vers la fenêtre SDL pour laquelle nous avons
 * attach� le contexte OpenGL.
 */
static void initGL(SDL_Window * win) {
  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelviewMatrix");
  gl4duBindMatrix("modelviewMatrix");
  gl4duLoadIdentityf();
  /* placer les objets en -10, soit bien apr�s le plan near (qui est � -2 voir resizeGL) */
  gl4duTranslatef(0, 0, -10);
  resizeGL(win);
}

/*!\brief Cette fonction param�trela vue (viewPort) OpenGL en fonction
 * des dimensions de la fenêtre SDL point�e par \a win.
 *
 * \param win le pointeur vers la fenêtre SDL pour laquelle nous avons
 * attach� le contexte OpenGL.
 */
static void resizeGL(SDL_Window * win) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);
  glViewport(0, 0, w, h);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1.0f, 1.0f, -h / (GLfloat)w, h / (GLfloat)w, 2.0f, 1000.0f);
}
static void initData(void) {
  int i;
  GLfloat data[] = {
    /* 4 coordonn�es de sommets */
    -1.f, -1.f, 0.f, 1.f, -1.f, 0.f,
    -1.f,  1.f, 0.f, 1.f,  1.f, 0.f,
    /* 2 coordonn�es de texture par sommet */
    1.0f, 1.0f, 0.0f, 1.0f, 
    1.0f, 0.0f, 0.0f, 0.0f
  };
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glGenBuffers(1, &_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1); 
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const void *)((4 * 3) * sizeof *data));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenTextures(1, &_tId);
  glBindTexture(GL_TEXTURE_2D, _tId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  _cap = new VideoCapture("video/MyVideo.mp4");
  if(!_cap || !_cap->isOpened()) {
    delete _cap;
    _cap = new VideoCapture(CAP_ANY);
  }
  _cap->set(CAP_PROP_FRAME_WIDTH,  (int)_dim[0]);
  _cap->set(CAP_PROP_FRAME_HEIGHT, (int)_dim[1]);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &_vao);

  glGenTextures(2, _fboTex);
  for(i = 0; i < 2; ++i) {
    /* paramétrage et description de la texture _fboTex[i] */
    glBindTexture(GL_TEXTURE_2D, _fboTex[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    /* description de la donnée texture */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _dim[0], _dim[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  }
  glBindTexture(GL_TEXTURE_2D, 0); 
}

/*!\brief Boucle infinie principale : g�re les �v�nements, dessine,
 * imprime le FPS et swap les buffers.
 *
 * \param win le pointeur vers la fenêtre SDL pour laquelle nous avons
 * attach� le contexte OpenGL.
 */
static void loop(SDL_Window * win) {
  SDL_GL_SetSwapInterval(1);
  for(;;) {
    manageEvents(win);
    draw();
    gl4duPrintFPS(stderr);
    SDL_GL_SwapWindow(win);
    gl4duUpdateShaders();
  }
}
/*!\brief Cette fonction permet de g�rer les �v�nements clavier et
 * souris via la biblioth�que SDL et pour la fenêtre point�e par \a
 * win.
 *
 * \param win le pointeur vers la fenêtre SDL pour laquelle nous avons
 * attach� le contexte OpenGL.
 */
static void manageEvents(SDL_Window * win) {
  SDL_Event event;
  while(SDL_PollEvent(&event)) 
    switch (event.type) {
    case SDL_KEYDOWN:
      switch(event.key.keysym.sym) {
      case SDLK_ESCAPE:
      case 'q':
	exit(0);
      default:
	fprintf(stderr, "La touche %s a ete pressee\n",
		SDL_GetKeyName(event.key.keysym.sym));
	break;
      }
      break;
    case SDL_KEYUP:
      break;
    case SDL_WINDOWEVENT:
      if(event.window.windowID == SDL_GetWindowID(win)) {
	switch (event.window.event)  {
	case SDL_WINDOWEVENT_RESIZED:
	  resizeGL(win);
	  break;
	case SDL_WINDOWEVENT_CLOSE:
	  event.type = SDL_QUIT;
	  SDL_PushEvent(&event);
	  break;
	}
      }
      break;
    case SDL_QUIT:
      exit(0);
    }
}

static void draw(void) {
  Mat ci;
  const GLfloat blanc[] = {1.0f, 1.0f, 1.0f, 1.0f};
 // const GLfloat bleu[]  = {0.5f, 0.5f, 1.0f, 1.0f};
  glBindTexture(GL_TEXTURE_2D, _fboTex[0]);
  *_cap >> ci;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ci.cols, ci.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, ci.data);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[1], 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   /* activation du programme _pId[0] (segment) */
  glUseProgram(_pId[1]);
  glEnable(GL_DEPTH_TEST);
  glUniform1i(glGetUniformLocation(_pId[1], "myTexture"), 0);
  glUniform1f(glGetUniformLocation(_pId[1], "width"), _dim[0]);
  glUniform1f(glGetUniformLocation(_pId[1], "height"), _dim[1]);
  /* streaming au fond */
  gl4duBindMatrix("modelviewMatrix");
  gl4duPushMatrix(); /* sauver modelview */
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, 0.9999f);
  gl4duBindMatrix("projectionMatrix");
  gl4duPushMatrix(); /* sauver projection */
  gl4duLoadIdentityf();
  gl4duSendMatrices(); /* envoyer les matrices */
  glUniform4fv(glGetUniformLocation(_pId[0], "couleur"), 1, blanc); /* envoyer une couleur */
  glBindVertexArray(_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); /* dessiner le streaming (ortho et au fond) */
  gl4duPopMatrix(); /* restaurer projection */
  gl4duBindMatrix("modelviewMatrix");
  /*------------------------------------------------------------------------------------------*/
  // pour qu'on affiche l'origine sur la petite en milieu 
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTex[0], 0);
  glUseProgram(_pId[0]);
  glEnable(GL_DEPTH_TEST);
  /*----------------------------------------------------------------------------------------------*/
  gl4duPopMatrix(); /* restaurer modelview */
  /* streaming tournant */
  gl4duRotatef(2, 0, 1, 0); /* ajouter 5 degres selon l'axe y � la matrice modelview en cours */
  gl4duSendMatrices(); /* envoyer les matrices */
  glUniform4fv(glGetUniformLocation(_pId[0], "couleur"), 1, blanc); /* envoyer une couleur */
  glBindVertexArray(_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); /* dessiner le streaming sur le lointain (ici perspective li�e � projection) plan tournant */

  gl4duPopMatrix(); /* restaurer modelview */
  /* streaming tournant */
  gl4duRotatef(2, 0, 1, 0); /* ajouter 5 degres selon l'axe y � la matrice modelview en cours */
  gl4duSendMatrices(); /* envoyer les matrices */
  glUniform4fv(glGetUniformLocation(_pId[0], "couleur"), 1, blanc); /* envoyer une couleur */
  glBindVertexArray(_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); /* dessiner le streaming sur le lointain (ici perspective li�e � projection) plan tournant */
}

/*!\brief Cette fonction est appel�e au moment de sortir du programme
 *  (atexit), elle lib�re la fenêtre SDL \ref _win et le contexte
 *  OpenGL \ref _oglContext.
 */
static void quit(void) {
  if(_cap) {
    delete _cap;
    _cap = NULL;
  }
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  if(_buffer)
    glDeleteBuffers(1, &_buffer);
  if(_tId)
    glDeleteTextures(1, &_tId);
  if(_oglContext)
    SDL_GL_DeleteContext(_oglContext);
  if(_win)
    SDL_DestroyWindow(_win);
  gl4duClean(GL4DU_ALL);
}
