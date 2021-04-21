#include <GL4D/gl4duw_SDL2.h>
#include <stdio.h>

static void init(void);
static void initData(void);
static void resize(int width, int height);
static void draw(void);
static void sortie(void);

static GLuint _wW = 900, _wH = 700;
static GLuint _quadId = 0;
 // \brief identifiant du (futur) Framebuffer Object 
static GLuint _fbo = 0;
/*
//\brief identifiant du (futur) Vertex Array Object 
static GLuint _vao = 0;
//\brief identifiant de la (future) texture originale 
//\brief identifiant du (futur) buffer de data - VBO ou Vertex Buffer Object 
static GLuint _buffer = 0;
//\brief identifiant du (futur) buffer d'indices - VBO ou Vertex Buffer Object 
static GLuint _ibuffer = 0;
static GLuint _texId = 0;
//la coordonnée de texture. Calculé dans init. 
static GLfloat _pas[2] = {0, 0};

static GLuint _fboTex[3] = {0, 0, 0}; */

static GLuint _pId[4]= {0} , _smPID = 0 ;
/*!\brief Texture recevant la shadow map */
static GLuint _smTex = 0;
static GLuint _cube = 0;
#define SHADOW_MAP_SIDE 512
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Eclairage", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init();
  initData();
  atexit(sortie);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  _quadId = gl4dgGenQuadf();
  _pId[0] = gl4duCreateProgram("<vs>shaders/lights.vs", "<fs>shaders/lights.fs", NULL);
  _pId[1] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/sobel_1.fs", NULL);
  _pId[2] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/blur.fs", NULL);
  _pId[3] = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/mult.fs", NULL);
  _smPID = gl4duCreateProgram("<vs>shaders/shadowMap.vs", "<fs>shaders/shadowMap.fs", NULL);
  glEnable(GL_DEPTH_TEST);
  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duGenMatrix(GL_FLOAT, "mod");
  gl4duGenMatrix(GL_FLOAT, "view");
  gl4duGenMatrix(GL_FLOAT, "cameraViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "cameraProjectionMatrix");
  resize(_wW, _wH);
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("lightProjectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1, 1, -1, 1, 1.5, 50.0);
  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.7, 0.7, -0.7 * _wH / _wW, 0.5 * _wH / _wW, 1.0, 50.0);
  gl4duBindMatrix("mod");
  glGenTextures(1, &_smTex);
  glBindTexture(GL_TEXTURE_2D, _smTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_SIDE, SHADOW_MAP_SIDE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  glGenFramebuffers(1, &_fbo);

}
static void initData(void) {
  _cube = gl4dgGenCubef();
} 
static void resize(int width, int height) {
  GLfloat ratio;
  _wW = width;
  _wH = height;
  glViewport(0, 0, _wW, _wH);
  ratio = _wW / ((GLfloat)_wH);
  gl4duBindMatrix("proj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1 * ratio, 1 * ratio, -1 /* / ratio */, 1 /* / ratio */, 2, 100);
}
void scene(GLfloat dt) {
  static GLfloat x = 0.0f;
  glEnable(GL_CULL_FACE);
  GLfloat rouge[] = {1.0f, 0, 0, 1.0f}, blanc[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat jaune[] = {1.0f,1.0f,0.0f,1.0f};
  GLfloat lumpos0[] = { 0.0f, 0.0f, 1.1f, 1.0f }, lumpos[4], * mat;
  lumpos0[0] = 4.0f * sin(x);
  x += dt;
  gl4duBindMatrix("view");
  mat = (GLfloat *)gl4duGetMatrixData();
  MMAT4XVEC4(lumpos, mat, lumpos0);
  gl4duBindMatrix("mod");
  gl4duLoadIdentityf();
    glUseProgram(_pId[0]);
  gl4duSendMatrices();
  gl4duTranslatef(0, 0, -10.0);
  gl4duPushMatrix(); {
    gl4duTranslatef(0, -3.0, 0.0);
    gl4duRotatef(-90, 1, 0, 0);
    gl4duScalef(17, 15, 17);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId[0], "scolor"), 1, rouge);
  glUniform4fv(glGetUniformLocation(_pId[0], "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId[0], "lumpos"), 1, lumpos);
  gl4dgDraw(_quadId);
/********************************* A *********************************/
  gl4duSendMatrices();
  gl4duPushMatrix(); {
    gl4duTranslatef(3, -0.1, 6);//position
    gl4duRotatef(-14, 0, 0, 1);
    gl4duScalef(0.6, 4, 0.6);//taille
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId[0], "scolor"), 1, jaune);
  glUniform4fv(glGetUniformLocation(_pId[0], "lcolor"), 1, blanc);
  glUniform4fv(glGetUniformLocation(_pId[0], "lumpos"), 1, lumpos);
  gl4dgDraw(_cube);

  gl4duSendMatrices();
  gl4duPushMatrix(); {
    gl4duTranslatef(5, -0.1, 6);//position
    gl4duRotatef(14, 0, 0, 1);
    gl4duScalef(0.6, 4, 0.6);//taille
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId[0], "scolor"), 1, jaune);
  gl4dgDraw(_cube);

gl4duSendMatrices();
  gl4duPushMatrix(); {
    gl4duTranslatef(4, 3.72, 6);
    gl4duScalef(0.6, 0.2, 0.6);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId[0], "scolor"), 1, jaune);
  gl4dgDraw(_cube);

gl4duSendMatrices();
  gl4duPushMatrix(); {
    gl4duTranslatef(4, -0.3, 6);
    gl4duScalef(0.6, 0.4, 0.6);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId[0], "scolor"), 1, jaune);
  gl4dgDraw(_cube); 

/********************************* K *********************************/
  gl4duSendMatrices();
  gl4duPushMatrix(); {
    gl4duTranslatef(-6, 0.0, 6);//position
    gl4duScalef(0.6, 3.92, 0.6);//taille
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId[0], "scolor"), 1, jaune);
  gl4dgDraw(_cube);
  
  gl4duSendMatrices();
  gl4duPushMatrix(); {
    gl4duTranslatef(-4, 1.2, 6);//position
    gl4duRotatef(-38, 0, 0, 1);
    gl4duScalef(0.6, 3, 0.6);//taille
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId[0], "scolor"), 1, jaune);
  gl4dgDraw(_cube);

gl4duSendMatrices();
  gl4duPushMatrix(); {
    gl4duTranslatef(-3.4, -1.5, 6);//position
    gl4duRotatef(33, 0, 0, 1);
    gl4duScalef(0.6, 2.4, 0.6);//taille
    gl4duSendMatrices();
  } gl4duPopMatrix();
  glUniform4fv(glGetUniformLocation(_pId[0], "scolor"), 1, jaune);
  gl4dgDraw(_cube);
  
  glUseProgram(0);
}
void draw(void) {

 
  static double t0 = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = t - t0;
  t0 = t;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* vue 1 */
  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0, 0, 15.0f, 0, 0, 0, 0.0f, 1.0f, 0);
  scene(dt);
   
  
}

void sortie(void) {
  gl4duClean(GL4DU_ALL);
}
