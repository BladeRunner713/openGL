/*
  CSCI 420 Computer Graphics, USC
  Assignment 1: Height Fields
  C++ starter code

  Student username: lingxiag
*/

#include <iostream>
#include <cstring>
#include "openGLHeader.h"
#include "glutHeader.h"

#include "imageIO.h"
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"

#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib, "glew32d.lib")
  #else
    #pragma comment(lib, "glew32.lib")
  #endif
#endif

#ifdef WIN32
  char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
  char shaderBasePath[1024] = "../openGLHelper-starterCode";
#endif

using namespace std;

int mousePos[2]; // x,y coordinate of the mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not 
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;
typedef enum { SOLID, LINE, POINT } RENDER_TYPE;
RENDER_TYPE renderType = SOLID;

// state of the world
float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { 0.0f, 0.0f, 0.0f };
float landScale[3] = { 1.0f, 1.0f, 1.0f };

int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework I";
float Vvector[3] = { 1.0f, 1.0f, 1.0f };

OpenGLMatrix *openGLMatrix;
BasicPipelineProgram *pipelineProgram;
ImageIO * heightmapImage;
int posLength;
int colLength;
GLuint solid_vao;
GLuint solid_vbo;
GLuint line_vao;
GLuint line_vbo;
GLuint point_vao;
GLuint point_vbo;
GLuint program;
float *positions;
float *colors;
int one = 0;
int ten = 0;
int hundred = 0;
void calculate();
void initVBO();
void initVAO();

struct Point 
{
  double x;
  double y;
  double z;
};



// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
  unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
  glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);

  if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
    cout << "File " << filename << " saved successfully." << endl;
  else cout << "Failed to save file " << filename << '.' << endl;

  delete [] screenshotData;
}

void displayFunc()
{
  // render some stuff...
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    pipelineProgram->Bind();
    GLint h_projectionMatrix = glGetUniformLocation(program, "projectionMatrix");

    float p[16]; 
    openGLMatrix->SetMatrixMode(OpenGLMatrix::Projection);
    openGLMatrix->GetMatrix(p);
    GLboolean isRowMajor = GL_FALSE;
    glUniformMatrix4fv(h_projectionMatrix, 1, isRowMajor, p);
    openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);
    openGLMatrix->LoadIdentity();
    
    openGLMatrix->LookAt(200, 200, 200, 0, 0, -1, 0, 1, 0);
    openGLMatrix->Rotate(landRotate[0], 1.0, 0.0, 0.0);
    openGLMatrix->Rotate(landRotate[1], 0.0, 1.0, 0.0);
    openGLMatrix->Rotate(landRotate[2], 0.0, 0.0, 1.0);
    openGLMatrix->Translate(landTranslate[0], landTranslate[1], landTranslate[2]);
    openGLMatrix->Scale(landScale[0], landScale[1], landScale[2]);
    
    float m[16]; 
    openGLMatrix->GetMatrix(m);
    GLint h_modelViewMatrix = glGetUniformLocation(program, "modelViewMatrix");
    glUniformMatrix4fv(h_modelViewMatrix, 1, isRowMajor, m);
    
    if (renderType == SOLID){
        glBindVertexArray(solid_vao); // bind the VAO
        GLint first = 0;
        GLsizei count = posLength * sizeof(float);
        glDrawArrays(GL_TRIANGLES, first, count);
        glBindVertexArray(0); // unbind the VAO
    }
    else if(renderType == LINE){
        glBindVertexArray(line_vao); // bind the VAO
        GLint first = 0;
        GLsizei count = posLength * sizeof(float);
        glDrawArrays(GL_LINES, first, count);
        glBindVertexArray(0); // unbind the VAO
    }
    else if(renderType == POINT){
        glBindVertexArray(point_vao); // bind the VAO
        GLint first = 0;
        GLsizei count = posLength * sizeof(float);
        glDrawArrays(GL_POINTS, first, count);
        glBindVertexArray(0); // unbind the VAO
    }
    
    glutSwapBuffers (); //force display
}

void idleFunc()
{
   if (hundred < 3) {
        char filename [10];
        filename[0] = '0' + hundred;
        filename[1] = '0' + ten;
        filename[2] = '0' + one;
        filename[3] = '.';
        filename[4] = 'j';
        filename[5] = 'p';
        filename[6] = 'g';
        filename[7] = '\0';
        saveScreenshot(filename);
        ++one;
        if (one == 10) {
            one = 0;
            ten++;
        }
        if (ten == 10) {
            ten = 0;
            hundred++;
        }
    }
  glutPostRedisplay();  
}

void keyboardFunc(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: // ESC key
      exit(0); // exit the program
    break;

    case ' ':
      cout << "You pressed the spacebar." << endl;
    break;

    case 'x':
      // take a screenshot
      saveScreenshot("screenshot.jpg");
    break;
      
    case 's':
      // switch to solid render
      renderType = SOLID;
      initVBO();
      initVAO();
      cout << "Switch to solid render." << endl;
    break;
    
    case 'l':
      // switch to line render
      renderType = LINE;
      initVBO();
      initVAO();
      cout << "Switch to line render." << endl;
    break;
      
    case 'p':
      // switch to point render
      renderType = POINT;
      initVBO();
      initVAO();
      cout << "Switch to point render." << endl;
    break;
  }
}

void reshapeFunc(int w, int h)
{
    glViewport(0, 0, w, h);
  // setup perspective matrix...
    openGLMatrix->SetMatrixMode(OpenGLMatrix::Projection);
    openGLMatrix->LoadIdentity();
    openGLMatrix->Perspective(60, 1.0 * w / h, 0.01, 1000.0);
    openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);
}

void mouseMotionDragFunc(int x, int y)
{
  // mouse has moved and one of the mouse buttons is pressed (dragging)

  // the change in mouse position since the last invocation of this function
  int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };

  switch (controlState)
  {
    // translate the landscape
    case TRANSLATE:
      if (leftMouseButton)
      {
        // control x,y translation via the left mouse button
        landTranslate[0] += mousePosDelta[0] * 0.01f;
        landTranslate[1] -= mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z translation via the middle mouse button
        landTranslate[2] += mousePosDelta[1] * 0.01f;
      }
      break;

    // rotate the landscape
    case ROTATE:
      if (leftMouseButton)
      {
        // control x,y rotation via the left mouse button
        landRotate[0] += mousePosDelta[1];
        landRotate[1] += mousePosDelta[0];
      }
      if (middleMouseButton)
      {
        // control z rotation via the middle mouse button
        landRotate[2] += mousePosDelta[1];
      }
      break;

    // scale the landscape
    case SCALE:
      if (leftMouseButton)
      {
        // control x,y scaling via the left mouse button
        landScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
        landScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      if (middleMouseButton)
      {
        // control z scaling via the middle mouse button
        landScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
      }
      break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
  // mouse has moved
  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
  // a mouse button has has been pressed or depressed

  // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
  switch (button)
  {
    case GLUT_LEFT_BUTTON:
      leftMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_MIDDLE_BUTTON:
      middleMouseButton = (state == GLUT_DOWN);
    break;

    case GLUT_RIGHT_BUTTON:
      rightMouseButton = (state == GLUT_DOWN);
    break;
  }

  // keep track of whether CTRL and SHIFT keys are pressed
  switch (glutGetModifiers())
  {
    case GLUT_ACTIVE_CTRL:
      controlState = TRANSLATE;
    break;

    case GLUT_ACTIVE_SHIFT:
      controlState = SCALE;
    break;

    // if CTRL and SHIFT are not pressed, we are in rotate mode
    default:
      controlState = ROTATE;
    break;
  }

  // store the new mouse position
  mousePos[0] = x;
  mousePos[1] = y;
}

void initVBO(){
    if (renderType == SOLID) {
        //VBO code
        glGenBuffers(1, &solid_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, solid_vbo);
        glBufferData(GL_ARRAY_BUFFER, posLength * sizeof(float) + colLength * sizeof(float), NULL, GL_STATIC_DRAW); // init buffer’s size, but don’t assign any data to it
        // upload position data
        glBufferSubData(GL_ARRAY_BUFFER, 0, posLength * sizeof(float), positions);
        // upload color data
        glBufferSubData(GL_ARRAY_BUFFER, posLength * sizeof(float), colLength * sizeof(float), colors);
    }
    else if(renderType == LINE){
        //VBO code
        glGenBuffers(1, &line_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
        glBufferData(GL_ARRAY_BUFFER, posLength * sizeof(float) + colLength * sizeof(float), NULL, GL_STATIC_DRAW); // init buffer’s size, but don’t assign any data to it
        // upload position data
        glBufferSubData(GL_ARRAY_BUFFER, 0, posLength * sizeof(float), positions);
        // upload color data
        glBufferSubData(GL_ARRAY_BUFFER, posLength * sizeof(float), colLength * sizeof(float), colors);
    }
    else if(renderType == POINT){
        //VBO code
        glGenBuffers(1, &point_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
        glBufferData(GL_ARRAY_BUFFER, posLength * sizeof(float) + colLength * sizeof(float), NULL, GL_STATIC_DRAW); // init buffer’s size, but don’t assign any data to it
        // upload position data
        glBufferSubData(GL_ARRAY_BUFFER, 0, posLength * sizeof(float), positions);
        // upload color data
        glBufferSubData(GL_ARRAY_BUFFER, posLength * sizeof(float), colLength * sizeof(float), colors);
    }
}

void initVAO(){
    if (renderType == SOLID) {
        //VAO code
        glGenVertexArrays(1, &solid_vao);
        glBindVertexArray(solid_vao); // bind the VAO
        
        // bind the VBO “buffer” (must be previously created)
        glBindBuffer(GL_ARRAY_BUFFER, solid_vbo);
        // get location index of the “position” shader variable
        GLuint loc = glGetAttribLocation(program, "position");
        glEnableVertexAttribArray(loc); // enable the “position” attribute
        const void * offset = (const void *)0;
        GLsizei stride = 0;
        GLboolean normalized = GL_FALSE;
        // set the layout of the “position” attribute data
        glVertexAttribPointer(loc, 3, GL_FLOAT, normalized, stride, offset);
        
        
        // get the location index of the “color” shader variable
        loc = glGetAttribLocation(program, "color");
        glEnableVertexAttribArray(loc); // enable the “color” attribute
        offset = (const void *)(posLength * sizeof(float));
        stride = 0;
        normalized = GL_FALSE;
        // set the layout of the “color” attribute data
        glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);
        
        glBindVertexArray(0); // unbind the VAO
    }
    else if (renderType == LINE){
        //VAO code
        glGenVertexArrays(1, &line_vao);
        glBindVertexArray(line_vao); // bind the VAO
        
        // bind the VBO “buffer” (must be previously created)
        glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
        // get location index of the “position” shader variable
        GLuint loc = glGetAttribLocation(program, "position");
        glEnableVertexAttribArray(loc); // enable the “position” attribute
        const void * offset = (const void *)0;
        GLsizei stride = 0;
        GLboolean normalized = GL_FALSE;
        // set the layout of the “position” attribute data
        glVertexAttribPointer(loc, 3, GL_FLOAT, normalized, stride, offset);
        
        
        // get the location index of the “color” shader variable
        loc = glGetAttribLocation(program, "color");
        glEnableVertexAttribArray(loc); // enable the “color” attribute
        offset = (const void *)(posLength * sizeof(float));
        stride = 0;
        normalized = GL_FALSE;
        // set the layout of the “color” attribute data
        glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);
        
        glBindVertexArray(0); // unbind the VAO
    }
    else if(renderType == POINT){
        //VAO code
        glGenVertexArrays(1, &point_vao);
        glBindVertexArray(point_vao); // bind the VAO
        
        // bind the VBO “buffer” (must be previously created)
        glBindBuffer(GL_ARRAY_BUFFER, point_vbo);
        // get location index of the “position” shader variable
        GLuint loc = glGetAttribLocation(program, "position");
        glEnableVertexAttribArray(loc); // enable the “position” attribute
        const void * offset = (const void *)0;
        GLsizei stride = 0;
        GLboolean normalized = GL_FALSE;
        // set the layout of the “position” attribute data
        glVertexAttribPointer(loc, 3, GL_FLOAT, normalized, stride, offset);
        
        
        // get the location index of the “color” shader variable
        loc = glGetAttribLocation(program, "color");
        glEnableVertexAttribArray(loc); // enable the “color” attribute
        offset = (const void *)(posLength * sizeof(float));
        stride = 0;
        normalized = GL_FALSE;
        // set the layout of the “color” attribute data
        glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);
        
        glBindVertexArray(0); // unbind the VAO
    }
}

void calculate(){
    int	w = heightmapImage->getWidth();
    int h = heightmapImage->getHeight();
    posLength = (w - 1) * (h - 1) * 2 * 3 * 3;
    colLength = (w - 1) * (h - 1) * 2 * 3 * 4;
    
    if (renderType == SOLID) {
        positions = new float[posLength];
        colors = new float[colLength];
        
        int k = 0;
        for(int j = 0; j < h; j++){
            for (int i = 0; i < w; i++) {
                if (i+1 < w && j+1 < h) {
                    //vertice 1 (i,j)
                    positions[k] = (float)i - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i, j, 0));
                    ++k;
                    positions[k] = -(float)j + (float)h/2.0;
                    ++k;
                    //vertice 2 (i,j+1)
                    positions[k] = (float)i - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i, j+1, 0));
                    ++k;
                    positions[k] = -(float)(j+1) + (float)h/2.0;
                    ++k;
                    //vertice 3 (i+1, j)
                    positions[k] = (float)(i+1) - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i+1, j, 0));
                    ++k;
                    positions[k] = -(float)j + (float)h/2.0;
                    ++k;
                    //vertice 4 (i,j+1)
                    positions[k] = (float)i - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i, j+1, 0));
                    ++k;
                    positions[k] = -(float)(j+1) + (float)h/2.0;
                    ++k;
                    //vertice 5 (i+1,j)
                    positions[k] = (float)(i+1) - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i+1, j, 0));
                    ++k;
                    positions[k] = -(float)j + (float)h/2.0;
                    ++k;
                    //vertice 6 (i+1,j+1)
                    positions[k] = (float)(i+1) - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i+1, j+1, 0));
                    ++k;
                    positions[k] = -(float)(j+1) + (float)h/2.0;
                    ++k;
                }
            }
        }
        k = 0;
        for(int j = 0; j < h; j++){
            for (int i = 0; i < w; i++) {
                if (i+1 < w && j+1 < h) {
                    //vertex 1 (i,j)
                    colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 2 (i,j+1)
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 3 (i+1,j)
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 4 (i,j+1)
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 5 (i+1,j)
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 6 (i+1,j+1)
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j+1, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                }
            }
        }
    }
    else if (renderType == LINE){
        
        positions = new float[posLength];
        colors = new float[colLength];
        
        int k = 0;
        for(int j = 0; j < h; j++){
            for (int i = 0; i < w; i++) {
                if (i+1 < w && j+1 < h) {
                    //vertice 1 (i,j)
                    positions[k] = (float)i - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i, j, 0));
                    ++k;
                    positions[k] = -(float)j + (float)h/2.0;
                    ++k;
                    //vertice 2 (i,j+1)
                    positions[k] = (float)i - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i, j+1, 0));
                    ++k;
                    positions[k] = -(float)(j+1) + (float)h/2.0;
                    ++k;
                    //vertice 3 (i+1, j)
                    positions[k] = (float)(i+1) - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i+1, j, 0));
                    ++k;
                    positions[k] = -(float)j + (float)h/2.0;
                    ++k;
                    //vertice 4 (i,j+1)
                    positions[k] = (float)i - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i, j+1, 0));
                    ++k;
                    positions[k] = -(float)(j+1) + (float)h/2.0;
                    ++k;
                    //vertice 5 (i+1,j)
                    positions[k] = (float)(i+1) - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i+1, j, 0));
                    ++k;
                    positions[k] = -(float)j + (float)h/2.0;
                    ++k;
                    //vertice 6 (i+1,j+1)
                    positions[k] = (float)(i+1) - (float)w/2.0;
                    ++k;
                    positions[k] = (float)(0.4 * heightmapImage->getPixel(i+1, j+1, 0));
                    ++k;
                    positions[k] = -(float)(j+1) + (float)h/2.0;
                    ++k;
                }
            }
        }
        k = 0;
        for(int j = 0; j < h; j++){
            for (int i = 0; i < w; i++) {
                if (i+1 < w && j+1 < h) {
                    //vertex 1 (i,j)
                    colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 2 (i,j+1)
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 3 (i+1,j)
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 4 (i,j+1)
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i, j+1, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 5 (i+1,j)
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                    //vertex 6 (i+1,j+1)
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j+1, 0))/255.0;
                    ++k;
                    colors[k] = (float)(heightmapImage->getPixel(i+1, j+1, 0))/255.0;
                    ++k;
                    colors[k] = 1.0;
                    ++k;
                }
            }
        }

    }
    else if (renderType == POINT) {
        posLength = 3 * w * h;
        colLength = 4 * w * h;
    
        positions = new float[posLength];
        colors = new float[colLength];

        int k = 0;
        for(int j = 0; j < h; j++){
            for (int i = 0; i < w; i++) {
                float height = 0.4 * heightmapImage->getPixel(i, j, 0);
                positions[k] = (float)i - (float)w/2.0;
                ++k;
                positions[k] = height;
                ++k;
                positions[k] = -(float)j + (float)h/2.0;
                ++k;
            }
        }
        k = 0;
        for(int j = 0; j < h; j++){
            for (int i = 0; i < w; i++) {
                colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                ++k;
                colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                ++k;
                colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                ++k;
                colors[k] = (float)(heightmapImage->getPixel(i, j, 0))/255.0;
                ++k;
            }
        }
    }
}


void initScene(int argc, char *argv[])
{
  // load the image from a jpeg disk file to main memory
  heightmapImage = new ImageIO();
  if (heightmapImage->loadJPEG(argv[1]) != ImageIO::OK)
  {
    cout << "Error reading image " << argv[1] << "." << endl;
    exit(EXIT_FAILURE);
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // do additional initialization here...
    openGLMatrix = new OpenGLMatrix();
    pipelineProgram = new BasicPipelineProgram();   
    pipelineProgram->Init("../openGLHelper-starterCode");
    pipelineProgram->Bind();
    program = pipelineProgram->GetProgramHandle();   
    calculate();
    initVBO();
    initVAO();
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cout << "The arguments are incorrect." << endl;
    cout << "usage: ./hw1 <heightmap file>" << endl;
    exit(EXIT_FAILURE);
  }

  cout << "Initializing GLUT..." << endl;
  glutInit(&argc,argv);

  cout << "Initializing OpenGL..." << endl;

  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif

  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);  
  glutCreateWindow(windowTitle);

  cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
  cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

  // tells glut to use a particular display function to redraw 
  glutDisplayFunc(displayFunc);
  // perform animation inside idleFunc
  glutIdleFunc(idleFunc);
  // callback for mouse drags
  glutMotionFunc(mouseMotionDragFunc);
  // callback for idle mouse movement
  glutPassiveMotionFunc(mouseMotionFunc);
  // callback for mouse button changes
  glutMouseFunc(mouseButtonFunc);
  // callback for resizing the window
  glutReshapeFunc(reshapeFunc);
  // callback for pressing the keys on the keyboard
  glutKeyboardFunc(keyboardFunc);

  // init glew
  #ifdef __APPLE__
    // nothing is needed on Apple
  #else
    // Windows, Linux
    GLint result = glewInit();
    if (result != GLEW_OK)
    {
      cout << "error: " << glewGetErrorString(result) << endl;
      exit(EXIT_FAILURE);
    }
  #endif

  // do initialization
  initScene(argc, argv);

  // sink forever into the glut loop
  glutMainLoop();
}


