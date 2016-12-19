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
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#include "imageIO.h"
#include <glm/glm.hpp>
#include <vector>
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"
#include <math.h>


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

int leftMouseButton = 0;   // 1 if pressed, 0 if not
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0;  // 1 if pressed, 0 if not

typedef enum { ROTATE,
    TRANSLATE,
    SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

// state of the world
float landRotate[3] = {0.0f, 0.0f, 0.0f};
float landTranslate[3] = {0.0f, 0.0f, 0.0f};
float landScale[3] = {1.0f, 1.0f, 1.0f};
// matrix for ground
float vertices[] = {
    -100.0f, 100.0f, -100.0f,
    -100.0f,  -100.0f, -100.0f,
    100.0f, -100.0f, -100.0f,
    -100.0f,  100.0f, -100.0f,
    100.0f, -100.0f, -100.0f,
    100.0f, 100.0f, -100.0f
};
//texture matrix for ground
float textCoord[] = {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 1.0f
};
//matirx for sky
float vertices1[] = {
    -100.0f, -100.0f, 100.0f,
    100.0f, -100.0f, 100.0f,
    100.0f, 100.0f, 100.0f,
    100.0f, 100.0f, 100.0f,
    -100.0f, 100.0f, 100.0f,
    -100.0f, -100.0f, 100.0f,
    
    
    -100.0f,  100.0f,  100.0f,
    -100.0f,  100.0f, -100.0f,
    -100.0f, -100.0f, -100.0f,
    -100.0f, -100.0f, -100.0f,
    -100.0f, -100.0f,  100.0f,
    -100.0f,  100.0f,  100.0f,
    
    100.0f,  100.0f,  100.0f,
    100.0f,  100.0f, -100.0f,
    100.0f, -100.0f, -100.0f,
    100.0f, -100.0f, -100.0f,
    100.0f, -100.0f,  100.0f,
    100.0f,  100.0f,  100.0f,
    
    -100.0f, -100.0f, -100.0f,
    100.0f, -100.0f, -100.0f,
    100.0f, -100.0f,  100.0f,
    100.0f, -100.0f,  100.0f,
    -100.0f, -100.0f,  100.0f,
    -100.0f, -100.0f, -100.0f,
    
    -100.0f,  100.0f, -100.0f,
    100.0f,  100.0f, -100.0f,
    100.0f,  100.0f,  100.0f,
    100.0f,  100.0f,  100.0f,
    -100.0f,  100.0f,  100.0f,
    -100.0f,  100.0f, -100.0f
};
//texture matrix for sky
float textCoord1[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    
    0.0f, 1.0f,
    0.0, 0.0f,
    1.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f
    
    
};

int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework I";

OpenGLMatrix *openGLMatrix;
BasicPipelineProgram *pipelineProgram;
BasicPipelineProgram *groundProgram;

ImageIO *heightmapImage;
int posLength;
int posLength1;
int colLength;
GLuint vao;
GLuint vbo;
GLuint secondvao;
GLuint secondvbo;
GLuint thirdvao;
GLuint thirdvbo;
GLuint groundVao;
GLuint groundVbo;
GLuint skyVao;
GLuint skyVbo;
GLuint program;
GLuint program1;
GLuint texHandle;
GLuint SkytexHandle;
GLuint WoodHandle;
GLuint RailtexHandle;
float *positions;
float *railPositions;
float *secondRailPositions;
float *temp;
float *railTextCoord;
float *CrossSectionPositions;
float * woodTextCoord;
//float *colors;
int one = 0;
int ten = 0;
vector<float>res;
vector<float> tangent;
vector<float> vvector;
vector<float> normalVector;
vector<float> bvector;
vector<float> railVector;
vector<float> secondRailVector;
vector<float> railTextVector;
vector<float> second;
vector<float> crossSectionVector;
vector<float>crossTextVector;

int hundred = 0;
int index1 = 0;



// represents one control point along the spline
struct Point
{
    double x;
    double y;
    double z;
};

// spline struct
// contains how many control points the spline has, and an array of control points
struct Spline
{
    int numControlPoints;
    Point *points;
};

// the spline array
Spline *splines;
// total number of splines
int numSplines;

int loadSplines(char *argv)
{
    char *cName = (char *)malloc(128 * sizeof(char));
    FILE *fileList;
    FILE *fileSpline;
    int iType, i = 0, j, iLength;
    
    // load the track file
    fileList = fopen(argv, "r");
    if (fileList == NULL)
    {
        printf("can't open file\n");
        exit(1);
    }
    
    // stores the number of splines in a global variable
    fscanf(fileList, "%d", &numSplines);
    
    splines = (Spline *)malloc(numSplines * sizeof(Spline));
    
    // reads through the spline files
    for (j = 0; j < numSplines; j++)
    {
        i = 0;
        fscanf(fileList, "%s", cName);
        fileSpline = fopen(cName, "r");
        
        if (fileSpline == NULL)
        {
            printf("can't open file\n");
            exit(1);
        }
        
        // gets length for spline file
        fscanf(fileSpline, "%d %d", &iLength, &iType);
        
        // allocate memory for all the points
        splines[j].points = (Point *)malloc(iLength * sizeof(Point));
        splines[j].numControlPoints = iLength;
        
        // saves the data to the struct
        while (fscanf(fileSpline, "%lf %lf %lf",
                      &splines[j].points[i].x,
                      &splines[j].points[i].y,
                      &splines[j].points[i].z) != EOF)
        {
            i++;
        }
    }
    
    free(cName);
    
    return 0;
}

int initTexture(const char *imageFilename, GLuint textureHandle)
{
    // read the texture image
    ImageIO img;
    ImageIO::fileFormatType imgFormat;
    ImageIO::errorType err = img.load(imageFilename, &imgFormat);
    
    if (err != ImageIO::OK)
    {
        printf("Loading texture from %s failed.\n", imageFilename);
        return -1;
    }
    
    // check that the number of bytes is a multiple of 4
    if (img.getWidth() * img.getBytesPerPixel() % 4)
    {
        printf("Error (%s): The width*numChannels in the loaded image must be a multiple of 4.\n", imageFilename);
        return -1;
    }
    
    // allocate space for an array of pixels
    int width = img.getWidth();
    int height = img.getHeight();
    unsigned char *pixelsRGBA = new unsigned char[4 * width * height]; // we will use 4 bytes per pixel, i.e., RGBA
    
    // fill the pixelsRGBA array with the image pixels
    memset(pixelsRGBA, 0, 4 * width * height); // set all bytes to 0
    for (int h = 0; h < height; h++)
        for (int w = 0; w < width; w++)
        {
            // assign some default byte values (for the case where img.getBytesPerPixel() < 4)
            pixelsRGBA[4 * (h * width + w) + 0] = 0;   // red
            pixelsRGBA[4 * (h * width + w) + 1] = 0;   // green
            pixelsRGBA[4 * (h * width + w) + 2] = 0;   // blue
            pixelsRGBA[4 * (h * width + w) + 3] = 255; // alpha channel; fully opaque
            
            // set the RGBA channels, based on the loaded image
            int numChannels = img.getBytesPerPixel();
            for (int c = 0; c < numChannels; c++) // only set as many channels as are available in the loaded image; the rest get the default value
                pixelsRGBA[4 * (h * width + w) + c] = img.getPixel(w, h, c);
        }
    
    // bind the texture
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    
    // initialize the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsRGBA);
    
    // generate the mipmaps for this texture
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // query support for anisotropic texture filtering
    GLfloat fLargest;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
    printf("Max available anisotropic samples: %f\n", fLargest);
    // set anisotropic texture filtering
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.5f * fLargest);
    
    // query for any errors
    GLenum errCode = glGetError();
    if (errCode != 0)
    {
        printf("Texture initialization error. Error code: %d.\n", errCode);
        return -1;
    }
    
    // de-allocate the pixel array -- it is no longer needed
    delete[] pixelsRGBA;
    
    return 0;
}

// Note: You should combine this file
// with the solution of homework 1.

// Note for Windows/MS Visual Studio:
// You should set argv[1] to track.txt.
// To do this, on the "Solution Explorer",
// right click your project, choose "Properties",
// go to "Configuration Properties", click "Debug",
// then type your track file name for the "Command Arguments".
// You can also repeat this process for the "Release" configuration.

// write a screenshot to the specified filename
void saveScreenshot(const char *filename)
{
    unsigned char *screenshotData = new unsigned char[windowWidth * windowHeight * 3];
    glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);
    
    ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);
    
    if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
        cout << "File " << filename << " saved successfully." << endl;
    else
        cout << "Failed to save file " << filename << '.' << endl;
    
    delete[] screenshotData;
}

void setTextureUnit(GLint unit)
{
    glActiveTexture(unit);
    GLint h_textureImage = glGetUniformLocation(program, "textureImage");
    glUniform1i(h_textureImage, unit - GL_TEXTURE0);
}

void displayFunc()
{
    // render some stuff...
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor(red, green, blue, alpha);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    pipelineProgram->Bind();
    GLint h_projectionMatrix = glGetUniformLocation(program, "projectionMatrix");
    
    float p[16];
    openGLMatrix->SetMatrixMode(OpenGLMatrix::Projection);
    openGLMatrix->GetMatrix(p);
    GLboolean isRowMajor = GL_FALSE;
    glUniformMatrix4fv(h_projectionMatrix, 1, isRowMajor, p);
    openGLMatrix->SetMatrixMode(OpenGLMatrix::ModelView);
    openGLMatrix->LoadIdentity();
    //openGLMatrix->LookAt(1, 1, 1, tangent[0], tangent[1], tangent[2], 0, 0, 1);
    if(index1 <  posLength - 1) {
        openGLMatrix->LookAt(positions[index1] + normalVector[index1], positions[index1 + 1] + normalVector[index1 + 1], positions[index1 + 2] + normalVector[index1 + 2], tangent[index1] + positions[index1] + normalVector[index1], tangent[index1 + 1] + positions[index1 + 1] + normalVector[index1 + 1], tangent[index1 + 2] + positions[index1 + 2] + normalVector[index1 + 2], normalVector[index1], normalVector[index1 + 1], normalVector[index1 + 2]);
        index1 += 3;
    }
    else {
        index1 = 0;
        openGLMatrix->LookAt(positions[index1] + normalVector[index1], positions[index1 + 1] + normalVector[index1 + 1], positions[index1 + 2] + normalVector[index1 + 2], tangent[index1] + positions[index1] + normalVector[index1], tangent[index1 + 1] + positions[index1 + 1] + normalVector[index1 + 1], tangent[index1 + 2] + positions[index1 + 2] + normalVector[index1 + 2], normalVector[index1], normalVector[index1 + 1], normalVector[index1 + 2]);
        index1 += 3;
    }
    openGLMatrix->Rotate(landRotate[0], 1.0, 0.0, 0.0);
    openGLMatrix->Rotate(landRotate[1], 0.0, 1.0, 0.0);
    openGLMatrix->Rotate(landRotate[2], 0.0, 0.0, 1.0);
    openGLMatrix->Translate(landTranslate[0], landTranslate[1], landTranslate[2]);
    //openGLMatrix->Scale(landScale[0], landScale[1], landScale[2]);
    
    float m[16];
    openGLMatrix->GetMatrix(m);
    GLint h_modelViewMatrix = glGetUniformLocation(program, "modelViewMatrix");
    glUniformMatrix4fv(h_modelViewMatrix, 1, isRowMajor, m);
    glEnable(GL_LINE_SMOOTH);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    setTextureUnit(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, WoodHandle);
    
    glBindVertexArray(thirdvao);
    glDrawArrays(GL_TRIANGLES, 0, crossSectionVector.size() / 3);
    glBindVertexArray(0); // unbind the VAO*/
    
    setTextureUnit(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RailtexHandle);
    
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, railVector.size() / 3);
    glBindVertexArray(0);
    
    
    //draw second rail
    glBindVertexArray(secondvao);
    glDrawArrays(GL_TRIANGLES, 0, secondRailVector.size() / 3);
    glBindVertexArray(0); // unbind the VAO*/
    
    setTextureUnit(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    
    glBindVertexArray(groundVao); // bind the VAO
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0); // unbind the VAO
    
    setTextureUnit(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, SkytexHandle);
    glBindVertexArray(skyVao); // bind the VAO
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 30);
    glBindVertexArray(0); // unbind the VAO
    
    glutSwapBuffers();
    
}


void idleFunc()
{
    if (hundred < 9)
    {
        char filename[10];
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
        if (one == 10)
        {
            one = 0;
            ten++;
        }
        if (ten == 10)
        {
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
        case 27:   // ESC key
            exit(0); // exit the program
            break;
            
        case ' ':
            cout << "You pressed the spacebar." << endl;
            break;
            
        case 'x':
            // take a screenshot
            saveScreenshot("screenshot.jpg");
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
    int mousePosDelta[2] = {x - mousePos[0], y - mousePos[1]};
    
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

//vbo for sky
void initSkyVBO()
{
    glGenBuffers(1, &skyVbo);
    glBindBuffer(GL_ARRAY_BUFFER, skyVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1) + sizeof(textCoord1), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices1), vertices1);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices1), sizeof(textCoord1), textCoord1);
}

//vao for sky
void initSkyVAO()
{
    glGenVertexArrays(1, &skyVao);
    glBindVertexArray(skyVao);
    glBindBuffer(GL_ARRAY_BUFFER, skyVao);
    GLuint loc = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    GLuint texture = glGetAttribLocation(program, "texCoord");
    glEnableVertexAttribArray(texture);
    // enable the texture attribute
    const void *offset = (const void *)sizeof(vertices1);
    // set the layout of the texture attribute data
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, offset);
    glBindVertexArray(0);
}


//vbo for ground 
void initGroundVBO()
{
    glGenBuffers(1, &groundVbo);
    glBindBuffer(GL_ARRAY_BUFFER, groundVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(textCoord), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(textCoord), textCoord);
    // upload position data
}
//vao for ground
void initGroundVAO()
{
    glGenVertexArrays(1, &groundVao);
    glBindVertexArray(groundVao);
    glBindBuffer(GL_ARRAY_BUFFER, groundVbo);
    GLuint loc = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    GLuint texture = glGetAttribLocation(program, "texCoord");
    glEnableVertexAttribArray(texture);
    // enable the texture attribute
    const void *offset = (const void *)sizeof(vertices);
    // set the layout of the texture attribute data
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, offset);
    glBindVertexArray(0);
}


//vao for rail
void initVAO()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); // bind the VAO
    // bind the VBO “buffer” (must be previously created)
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // get location index of the “position” shader variable
    GLuint loc = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc); // enable the “position” attribute
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    GLuint texture = glGetAttribLocation(program, "texCoord");
    glEnableVertexAttribArray(texture);
    // enable the texture attribute
    const void *offset = (const void *)sizeof(railPositions);
    // set the layout of the texture attribute data
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, offset);
    glBindVertexArray(0); // unbind the VAO
}


//vao for cross section
void initThirdVAO()
{
    glGenVertexArrays(1, &thirdvao);
    glBindVertexArray(thirdvao); // bind the VAO
    // bind the VBO “buffer” (must be previously created)
    glBindBuffer(GL_ARRAY_BUFFER, thirdvbo);
    // get location index of the “position” shader variable
    GLuint loc = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc); // enable the “position” attribute
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    GLuint texture = glGetAttribLocation(program, "texCoord");
    glEnableVertexAttribArray(texture);
    // enable the texture attribute
    const void *offset = (const void *)sizeof(CrossSectionPositions);
    // set the layout of the texture attribute data
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, offset);
    glBindVertexArray(0); // unbind the VAO
}

//vbo for cross section
void initThirdVBO()
{
    //VBO code
    glGenBuffers(1, &thirdvbo);
    glBindBuffer(GL_ARRAY_BUFFER, thirdvbo);
    glBufferData(GL_ARRAY_BUFFER, crossSectionVector.size() * 4 + sizeof(woodTextCoord), NULL, GL_STATIC_DRAW); // init buffer’s size, but don’t assign any data to it
    // upload position data
    glBufferSubData(GL_ARRAY_BUFFER, 0, crossSectionVector.size() * 4 , CrossSectionPositions);
    glBufferSubData(GL_ARRAY_BUFFER, crossSectionVector.size() * 4, sizeof(woodTextCoord), woodTextCoord);
    
}

//vao for second rail
void initSecondVAO()
{
    glGenVertexArrays(1, &secondvao);
    glBindVertexArray(secondvao); // bind the VAO
    // bind the VBO “buffer” (must be previously created)
    glBindBuffer(GL_ARRAY_BUFFER, secondvbo);
    // get location index of the “position” shader variable
    GLuint loc = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc); // enable the “position” attribute
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    GLuint texture = glGetAttribLocation(program, "texCoord");
    glEnableVertexAttribArray(texture);
    // enable the texture attribute
    const void *offset = (const void *)sizeof(secondRailPositions);
    // set the layout of the texture attribute data
    glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 0, offset);
    glBindVertexArray(0); // unbind the VAO
}

//vbo for second rail
void initSecondVBO()
{
    //VBO code
    glGenBuffers(1, &secondvbo);
    glBindBuffer(GL_ARRAY_BUFFER, secondvbo);
    glBufferData(GL_ARRAY_BUFFER, railVector.size() * 4 + sizeof(railTextCoord), NULL, GL_STATIC_DRAW); // init buffer’s size, but don’t assign any data to it
    // upload position data
    glBufferSubData(GL_ARRAY_BUFFER, 0, railVector.size() * 4 , secondRailPositions);
    glBufferSubData(GL_ARRAY_BUFFER, railVector.size() * 4, sizeof(railTextCoord), railTextCoord);
    
}

//vao for rail
void initVBO()
{
    //VBO code
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, railVector.size() * 4 + sizeof(railTextCoord), NULL, GL_STATIC_DRAW); // init buffer’s size, but don’t assign any data to it
    // upload position data
    glBufferSubData(GL_ARRAY_BUFFER, 0, railVector.size() * 4 , railPositions);
    glBufferSubData(GL_ARRAY_BUFFER, railVector.size() * 4, sizeof(railTextCoord), railTextCoord);
    
}


// all of the calculation
void calculatePoint()
{  //posLength = 100 * 3 * (splines[0].numControlPoints - 4);
    //positions = new float[posLength];
    int count = 0;
    vvector.push_back(2.0f);
    vvector.push_back(2.0f);
    vvector.push_back(2.0f);
    
    float basicMatrix[4][4] = {
        {-0.5, 1.5, -1.5, 0.5},
        {1, -2.5, 2, -0.5},
        {-0.5, 0, 0.5, 0},
        {0, 1, 0, 0}};
    for (int i = 1; i < splines[0].numControlPoints - 2; i++)
    {
        for (float u = 0.0; u <= 0.99; u += 0.01)
        {
            float temp[4] = {0, 0, 0, 0};
            float temp1[4] = {0, 0, 0, 0};
            float uMatrix[4] = {u * u * u, u * u, u, 1.0};
            float upMatrix[4] = {3 * u * u, 2 * u, 1.0, 0.0};
            for (int j = 0; j < 4; ++j)
            {
                for (int k = 0; k < 4; ++k)
                {
                    temp[j] += uMatrix[k] * basicMatrix[k][j];
                    temp1[j] += upMatrix[k] * basicMatrix[k][j];
                }
            }
            res.push_back(temp[0] * splines[0].points[i - 1].x + temp[1] * splines[0].points[i].x + temp[2] * splines[0].points[i + 1].x + temp[3] * splines[0].points[i + 2].x);
            res.push_back(temp[0] * splines[0].points[i - 1].y + temp[1] * splines[0].points[i].y + temp[2] * splines[0].points[i + 1].y + temp[3] * splines[0].points[i + 2].y);
            res.push_back(temp[0] * splines[0].points[i - 1].z + temp[1] * splines[0].points[i].z + temp[2] * splines[0].points[i + 1].z + temp[3] * splines[0].points[i + 2].z);
            second.push_back(temp[0] * splines[0].points[i - 1].x + temp[1] * splines[0].points[i].x + temp[2] * splines[0].points[i + 1].x + temp[3] * splines[0].points[i + 2].x - 1);
            second.push_back(temp[0] * splines[0].points[i - 1].y + temp[1] * splines[0].points[i].y + temp[2] * splines[0].points[i + 1].y + temp[3] * splines[0].points[i + 2].y);
            second.push_back(temp[0] * splines[0].points[i - 1].z + temp[1] * splines[0].points[i].z + temp[2] * splines[0].points[i + 1].z + temp[3] * splines[0].points[i + 2].z);
            float x = temp1[0] * splines[0].points[i - 1].x + temp1[1] * splines[0].points[i].x + temp1[2] * splines[0].points[i + 1].x + temp1[3] * splines[0].points[i + 2].x;
            float y = temp1[0] * splines[0].points[i - 1].y + temp1[1] * splines[0].points[i].y + temp1[2] * splines[0].points[i + 1].y + temp1[3] * splines[0].points[i + 2].y;
            float z = temp1[0] * splines[0].points[i - 1].z + temp1[1] * splines[0].points[i].z + temp1[2] * splines[0].points[i + 1].z + temp1[3] * splines[0].points[i + 2].z;
            float l = sqrt(x * x + y * y + z * z);
            tangent.push_back(x/l);
            tangent.push_back(y/l);
            tangent.push_back(z/l);
        }
    }
    posLength = res.size();
    positions = new float[posLength];
    for (int i = 0; i < posLength; i++)
    {
        positions[i] = res[i];
    }
    for (int i = 0; i < posLength; i += 3)
    {
        if (i == 0) {
            float x = tangent[1] * vvector[2] - tangent[2] * vvector[1];
            float y = tangent[2] * vvector[0] - tangent[0] * vvector[2];
            float z = tangent[0] * vvector[1] - tangent[1] * vvector[0];
            float unit = sqrt(x * x + y * y + z * z);
            normalVector.push_back(x/unit);
            normalVector.push_back(y/unit);
            normalVector.push_back(z/unit);
            x = tangent[1] * normalVector[2] - tangent[2] * normalVector[1];
            y = tangent[2] * normalVector[0] - tangent[0] * normalVector[2];
            z = tangent[0] * normalVector[1] - tangent[1] * normalVector[0];
            unit = sqrt(x * x + y * y + z * z);
            bvector.push_back(x/unit);
            bvector.push_back(y/unit);
            bvector.push_back(z/unit);
            continue;
        }
        float x = bvector[i - 2] * tangent[i + 2] - bvector[i - 1] * tangent[i + 1];
        float y = bvector[i - 1] * tangent[i] - bvector[i - 3] * tangent[i + 2];
        float z = bvector[i - 3] * tangent[i + 1] - bvector[i - 2] * tangent[i];
        float unit = sqrt(x * x + y * y + z * z);
        normalVector.push_back(x/unit);
        normalVector.push_back(y/unit);
        normalVector.push_back(z/unit);
        x = tangent[i + 1] * normalVector[i + 2] - tangent[i + 2] * normalVector[i + 1];
        y = tangent[i + 2] * normalVector[i] - tangent[i] * normalVector[i + 2];
        z = tangent[i] * normalVector[i + 1] - tangent[i + 1] * normalVector[i];
        unit = sqrt(x * x + y * y + z * z);
        bvector.push_back(x/unit);
        bvector.push_back(y/unit);
        bvector.push_back(z/unit);
    }
    for (int i = 0; i < posLength - 3; i += 3)
    {
        float x1 = positions[i] + 0.05 * (-1 * normalVector[i] + bvector[i]);
        float y1 = positions[i + 1] + 0.05 * (-1 * normalVector[i + 1] + bvector[i + 1]);
        float z1 = positions[i + 2] + 0.05 * (-1 * normalVector[i + 2] + bvector[i + 2]);
        
        float x2 = positions[i] + 0.05 * (normalVector[i] + bvector[i]);
        float y2 = positions[i + 1] + 0.05 * (normalVector[i + 1] + bvector[i + 1]);
        float z2 = positions[i + 2] + 0.05 * (normalVector[i + 2] + bvector[i + 2]);
        
        float x3 = positions[i] + 0.05 * (normalVector[i] - bvector[i]);
        float y3 = positions[i + 1] + 0.05 * (normalVector[i + 1] - bvector[i + 1]);
        float z3 = positions[i + 2] + 0.05 * (normalVector[i + 2] - bvector[i + 2]);
        
        float x4 = positions[i] + 0.05 * (-1 * normalVector[i] - bvector[i]);
        float y4 = positions[i + 1] + 0.05 * (-1 * normalVector[i + 1] - bvector[i + 1]);
        float z4 = positions[i + 2] + 0.05 * (-1 * normalVector[i + 2] - bvector[i + 2]);
        
        float x5 = positions[i + 3] + 0.05 * (-1 * normalVector[i + 3] + bvector[i + 3]);
        float y5 = positions[i + 4] + 0.05 * (-1 * normalVector[i + 4] + bvector[i + 4]);
        float z5 = positions[i + 5] + 0.05 * (-1 * normalVector[i + 5] + bvector[i + 5]);
        
        float x6 = positions[i + 3] + 0.05 * (normalVector[i + 3] + bvector[i + 3]);
        float y6 = positions[i + 4] + 0.05 * (normalVector[i + 4] + bvector[i + 4]);
        float z6 = positions[i + 5] + 0.05 * (normalVector[i + 5] + bvector[i + 5]);
        
        float x7 = positions[i + 3] + 0.05 * (normalVector[i + 3] - bvector[i + 3]);
        float y7 = positions[i + 4] + 0.05 * (normalVector[i + 4] - bvector[i + 4]);
        float z7 = positions[i + 5] + 0.05 * (normalVector[i + 5] - bvector[i + 5]);
        
        float x8 = positions[i + 3] + 0.05 * (-1 * normalVector[i + 3] - bvector[i + 3]);
        float y8 = positions[i + 4] + 0.05 * (-1 * normalVector[i + 4] - bvector[i + 4]);
        float z8 = positions[i + 5] + 0.05 * (-1 * normalVector[i + 5] - bvector[i + 5]);
        if(i % 50 == 0)
        {
            float a1 = x1 + 0.5;
            float b1 = y1 - 0.1;
            float c1 = z1;
            
            float a2 = x1 + 0.5;
            float b2 = y1;
            float c2 = z1;
            
            float a3 = x3;
            float b3 = y3;
            float c3 = z3;
            
            float a4 = x3;
            float b4 = y3 - 0.1;
            float c4 = z3;
            
            float a5 = x4 + 0.5;
            float b5 = y4 - 0.1;
            float c5 = z5;
            
            float a6 = x4 + 0.5;
            float b6 = y4;
            float c6 = z4;
            
            float a7 = x7;
            float b7 = y7;
            float c7 = z7;
            
            float a8 = x7;
            float b8 = y7 - 0.1;
            float c8 = z7;
            
            crossSectionVector.push_back(a1);
            crossSectionVector.push_back(b1);
            crossSectionVector.push_back(c1);
            crossSectionVector.push_back(a2);
            crossSectionVector.push_back(b2);
            crossSectionVector.push_back(c2);
            crossSectionVector.push_back(a3);
            crossSectionVector.push_back(b3);
            crossSectionVector.push_back(c3);
            
            crossSectionVector.push_back(a1);
            crossSectionVector.push_back(b1);
            crossSectionVector.push_back(c1);
            crossSectionVector.push_back(a3);
            crossSectionVector.push_back(b3);
            crossSectionVector.push_back(c3);
            crossSectionVector.push_back(a4);
            crossSectionVector.push_back(b4);
            crossSectionVector.push_back(c4);
            
            crossSectionVector.push_back(a1);
            crossSectionVector.push_back(b1);
            crossSectionVector.push_back(c1);
            crossSectionVector.push_back(a2);
            crossSectionVector.push_back(b2);
            crossSectionVector.push_back(c2);
            crossSectionVector.push_back(a5);
            crossSectionVector.push_back(b5);
            crossSectionVector.push_back(c5);
            
            crossSectionVector.push_back(a2);
            crossSectionVector.push_back(b2);
            crossSectionVector.push_back(c2);
            crossSectionVector.push_back(a5);
            crossSectionVector.push_back(b5);
            crossSectionVector.push_back(c5);
            crossSectionVector.push_back(a6);
            crossSectionVector.push_back(b6);
            crossSectionVector.push_back(c6);
            
            crossSectionVector.push_back(a2);
            crossSectionVector.push_back(b2);
            crossSectionVector.push_back(c2);
            crossSectionVector.push_back(a3);
            crossSectionVector.push_back(b3);
            crossSectionVector.push_back(c3);
            crossSectionVector.push_back(a6);
            crossSectionVector.push_back(b6);
            crossSectionVector.push_back(c6);
            
            crossSectionVector.push_back(a3);
            crossSectionVector.push_back(b3);
            crossSectionVector.push_back(c3);
            crossSectionVector.push_back(a6);
            crossSectionVector.push_back(b6);
            crossSectionVector.push_back(c6);
            crossSectionVector.push_back(a7);
            crossSectionVector.push_back(b7);
            crossSectionVector.push_back(c7);
            
            crossSectionVector.push_back(a3);
            crossSectionVector.push_back(b3);
            crossSectionVector.push_back(c3);
            crossSectionVector.push_back(a4);
            crossSectionVector.push_back(b4);
            crossSectionVector.push_back(c4);
            crossSectionVector.push_back(a8);
            crossSectionVector.push_back(b8);
            crossSectionVector.push_back(c8);
            
            crossSectionVector.push_back(a3);
            crossSectionVector.push_back(b3);
            crossSectionVector.push_back(c3);
            crossSectionVector.push_back(a8);
            crossSectionVector.push_back(b8);
            crossSectionVector.push_back(c8);
            crossSectionVector.push_back(a7);
            crossSectionVector.push_back(b7);
            crossSectionVector.push_back(c7);
            
            crossSectionVector.push_back(a1);
            crossSectionVector.push_back(b1);
            crossSectionVector.push_back(c1);
            crossSectionVector.push_back(a4);
            crossSectionVector.push_back(b4);
            crossSectionVector.push_back(c4);
            crossSectionVector.push_back(a5);
            crossSectionVector.push_back(b5);
            crossSectionVector.push_back(c5);
            
            crossSectionVector.push_back(a4);
            crossSectionVector.push_back(b4);
            crossSectionVector.push_back(c4);
            crossSectionVector.push_back(a5);
            crossSectionVector.push_back(b5);
            crossSectionVector.push_back(c5);
            crossSectionVector.push_back(a8);
            crossSectionVector.push_back(b8);
            crossSectionVector.push_back(c8);
            
            crossSectionVector.push_back(a5);
            crossSectionVector.push_back(b5);
            crossSectionVector.push_back(c5);
            crossSectionVector.push_back(a6);
            crossSectionVector.push_back(b6);
            crossSectionVector.push_back(c6);
            crossSectionVector.push_back(a7);
            crossSectionVector.push_back(b7);
            crossSectionVector.push_back(c7);
            
            crossSectionVector.push_back(a6);
            crossSectionVector.push_back(b6);
            crossSectionVector.push_back(c6);
            crossSectionVector.push_back(a7);
            crossSectionVector.push_back(b7);
            crossSectionVector.push_back(c7);
            crossSectionVector.push_back(a8);
            crossSectionVector.push_back(b8);
            crossSectionVector.push_back(c8);
        }
        //
        railVector.push_back(x1);
        railVector.push_back(y1);
        railVector.push_back(z1);
        railVector.push_back(x2);
        railVector.push_back(y2);
        railVector.push_back(z2);
        railVector.push_back(x3);
        railVector.push_back(y3);
        railVector.push_back(z3);
        
        railVector.push_back(x1);
        railVector.push_back(y1);
        railVector.push_back(z1);
        railVector.push_back(x3);
        railVector.push_back(y3);
        railVector.push_back(z3);
        railVector.push_back(x4);
        railVector.push_back(y4);
        railVector.push_back(z4);
        
        railVector.push_back(x1);
        railVector.push_back(y1);
        railVector.push_back(z1);
        railVector.push_back(x2);
        railVector.push_back(y2);
        railVector.push_back(z2);
        railVector.push_back(x5);
        railVector.push_back(y5);
        railVector.push_back(z5);
        
        railVector.push_back(x2);
        railVector.push_back(y2);
        railVector.push_back(z2);
        railVector.push_back(x5);
        railVector.push_back(y5);
        railVector.push_back(z5);
        railVector.push_back(x6);
        railVector.push_back(y6);
        railVector.push_back(z6);
        
        railVector.push_back(x2);
        railVector.push_back(y2);
        railVector.push_back(z2);
        railVector.push_back(x3);
        railVector.push_back(y3);
        railVector.push_back(z3);
        railVector.push_back(x6);
        railVector.push_back(y6);
        railVector.push_back(z6);
        
        railVector.push_back(x3);
        railVector.push_back(y3);
        railVector.push_back(z3);
        railVector.push_back(x6);
        railVector.push_back(y6);
        railVector.push_back(z6);
        railVector.push_back(x7);
        railVector.push_back(y7);
        railVector.push_back(z7);
        
        railVector.push_back(x3);
        railVector.push_back(y3);
        railVector.push_back(z3);
        railVector.push_back(x4);
        railVector.push_back(y4);
        railVector.push_back(z4);
        railVector.push_back(x8);
        railVector.push_back(y8);
        railVector.push_back(z8);
        
        railVector.push_back(x3);
        railVector.push_back(y3);
        railVector.push_back(z3);
        railVector.push_back(x8);
        railVector.push_back(y8);
        railVector.push_back(z8);
        railVector.push_back(x7);
        railVector.push_back(y7);
        railVector.push_back(z7);
        
        railVector.push_back(x1);
        railVector.push_back(y1);
        railVector.push_back(z1);
        railVector.push_back(x4);
        railVector.push_back(y4);
        railVector.push_back(z4);
        railVector.push_back(x5);
        railVector.push_back(y5);
        railVector.push_back(z5);
        
        railVector.push_back(x4);
        railVector.push_back(y4);
        railVector.push_back(z4);
        railVector.push_back(x5);
        railVector.push_back(y5);
        railVector.push_back(z5);
        railVector.push_back(x8);
        railVector.push_back(y8);
        railVector.push_back(z8);
        
        railVector.push_back(x5);
        railVector.push_back(y5);
        railVector.push_back(z5);
        railVector.push_back(x6);
        railVector.push_back(y6);
        railVector.push_back(z6);
        railVector.push_back(x7);
        railVector.push_back(y7);
        railVector.push_back(z7);
        
        railVector.push_back(x6);
        railVector.push_back(y6);
        railVector.push_back(z6);
        railVector.push_back(x7);
        railVector.push_back(y7);
        railVector.push_back(z7);
        railVector.push_back(x8);
        railVector.push_back(y8);
        railVector.push_back(z8);
        
        
        ///
        secondRailVector.push_back(x1 + 0.5);
        secondRailVector.push_back(y1);
        secondRailVector.push_back(z1);
        secondRailVector.push_back(x2 + 0.5);
        secondRailVector.push_back(y2);
        secondRailVector.push_back(z2);
        secondRailVector.push_back(x3 + 0.5);
        secondRailVector.push_back(y3);
        secondRailVector.push_back(z3);
        
        secondRailVector.push_back(x1 + 0.5);
        secondRailVector.push_back(y1);
        secondRailVector.push_back(z1);
        secondRailVector.push_back(x3 + 0.5);
        secondRailVector.push_back(y3);
        secondRailVector.push_back(z3);
        secondRailVector.push_back(x4 + 0.5);
        secondRailVector.push_back(y4);
        secondRailVector.push_back(z4);
        
        secondRailVector.push_back(x1 + 0.5);
        secondRailVector.push_back(y1);
        secondRailVector.push_back(z1);
        secondRailVector.push_back(x2 + 0.5);
        secondRailVector.push_back(y2);
        secondRailVector.push_back(z2);
        secondRailVector.push_back(x5 + 0.5);
        secondRailVector.push_back(y5);
        secondRailVector.push_back(z5);
        
        secondRailVector.push_back(x2 + 0.5);
        secondRailVector.push_back(y2);
        secondRailVector.push_back(z2);
        secondRailVector.push_back(x5 + 0.5);
        secondRailVector.push_back(y5);
        secondRailVector.push_back(z5);
        secondRailVector.push_back(x6 + 0.5);
        secondRailVector.push_back(y6);
        secondRailVector.push_back(z6);
        
        secondRailVector.push_back(x2 + 0.5);
        secondRailVector.push_back(y2);
        secondRailVector.push_back(z2);
        secondRailVector.push_back(x3 + 0.5);
        secondRailVector.push_back(y3);
        secondRailVector.push_back(z3);
        secondRailVector.push_back(x6 + 0.5);
        secondRailVector.push_back(y6);
        secondRailVector.push_back(z6);
        
        secondRailVector.push_back(x3 + 0.5);
        secondRailVector.push_back(y3);
        secondRailVector.push_back(z3);
        secondRailVector.push_back(x6 + 0.5);
        secondRailVector.push_back(y6);
        secondRailVector.push_back(z6);
        secondRailVector.push_back(x7 + 0.5);
        secondRailVector.push_back(y7);
        secondRailVector.push_back(z7);
        
        secondRailVector.push_back(x3 + 0.5);
        secondRailVector.push_back(y3);
        secondRailVector.push_back(z3);
        secondRailVector.push_back(x4 + 0.5);
        secondRailVector.push_back(y4);
        secondRailVector.push_back(z4);
        secondRailVector.push_back(x8 + 0.5);
        secondRailVector.push_back(y8);
        secondRailVector.push_back(z8);
        
        secondRailVector.push_back(x3 + 0.5);
        secondRailVector.push_back(y3);
        secondRailVector.push_back(z3);
        secondRailVector.push_back(x8 + 0.5);
        secondRailVector.push_back(y8);
        secondRailVector.push_back(z8);
        secondRailVector.push_back(x7 + 0.5);
        secondRailVector.push_back(y7);
        secondRailVector.push_back(z7);
        
        secondRailVector.push_back(x1 + 0.5);
        secondRailVector.push_back(y1);
        secondRailVector.push_back(z1);
        secondRailVector.push_back(x4 + 0.5);
        secondRailVector.push_back(y4);
        secondRailVector.push_back(z4);
        secondRailVector.push_back(x5 + 0.5);
        secondRailVector.push_back(y5);
        secondRailVector.push_back(z5);
        
        secondRailVector.push_back(x4 + 0.5);
        secondRailVector.push_back(y4);
        secondRailVector.push_back(z4);
        secondRailVector.push_back(x5 + 0.5);
        secondRailVector.push_back(y5);
        secondRailVector.push_back(z5);
        secondRailVector.push_back(x8 + 0.5);
        secondRailVector.push_back(y8);
        secondRailVector.push_back(z8);
        
        secondRailVector.push_back(x5 + 0.5);
        secondRailVector.push_back(y5);
        secondRailVector.push_back(z5);
        secondRailVector.push_back(x6 + 0.5);
        secondRailVector.push_back(y6);
        secondRailVector.push_back(z6);
        secondRailVector.push_back(x7 + 0.5);
        secondRailVector.push_back(y7);
        secondRailVector.push_back(z7);
        
        secondRailVector.push_back(x6 + 0.5);
        secondRailVector.push_back(y6);
        secondRailVector.push_back(z6);
        secondRailVector.push_back(x7 + 0.5);
        secondRailVector.push_back(y7);
        secondRailVector.push_back(z7);
        secondRailVector.push_back(x8 + 0.5);
        secondRailVector.push_back(y8);
        secondRailVector.push_back(z8);
        
        
        
        /*crossSectionVector.push_back(x1 + 1);
         crossSectionVector.push_back(y1);
         crossSectionVector.push_back(z1);
         crossSectionVector.push_back(x2 + 1);
         crossSectionVector.push_back(y2);
         crossSectionVector.push_back(z2);
         crossSectionVector.push_back(x3 + 1);
         crossSectionVector.push_back(y3);
         crossSectionVector.push_back(z3);
         
         crossSectionVector.push_back(x1 + 1);
         crossSectionVector.push_back(y1);
         crossSectionVector.push_back(z1);
         crossSectionVector.push_back(x3 + 1);
         crossSectionVector.push_back(y3);
         crossSectionVector.push_back(z3);
         crossSectionVector.push_back(x4 + 1);
         crossSectionVector.push_back(y4);
         crossSectionVector.push_back(z4);
         
         crossSectionVector.push_back(x1 + 1);
         crossSectionVector.push_back(y1);
         crossSectionVector.push_back(z1);
         crossSectionVector.push_back(x2 + 1);
         crossSectionVector.push_back(y2);
         crossSectionVector.push_back(z2);
         crossSectionVector.push_back(x5 + 1);
         crossSectionVector.push_back(y5);
         crossSectionVector.push_back(z5);
         
         crossSectionVector.push_back(x2 + 1);
         crossSectionVector.push_back(y2);
         crossSectionVector.push_back(z2);
         crossSectionVector.push_back(x5 + 1);
         crossSectionVector.push_back(y5);
         crossSectionVector.push_back(z5);
         crossSectionVector.push_back(x6 + 1);
         crossSectionVector.push_back(y6);
         crossSectionVector.push_back(z6);
         
         crossSectionVector.push_back(x2 + 1);
         crossSectionVector.push_back(y2);
         crossSectionVector.push_back(z2);
         crossSectionVector.push_back(x3 + 1);
         crossSectionVector.push_back(y3);
         crossSectionVector.push_back(z3);
         crossSectionVector.push_back(x6 + 1);
         crossSectionVector.push_back(y6);
         crossSectionVector.push_back(z6);
         
         crossSectionVector.push_back(x3 + 1);
         crossSectionVector.push_back(y3);
         crossSectionVector.push_back(z3);
         crossSectionVector.push_back(x6 + 1);
         crossSectionVector.push_back(y6);
         crossSectionVector.push_back(z6);
         crossSectionVector.push_back(x7 + 1);
         crossSectionVector.push_back(y7);
         crossSectionVector.push_back(z7);
         
         crossSectionVector.push_back(x3 + 1);
         crossSectionVector.push_back(y3);
         crossSectionVector.push_back(z3);
         crossSectionVector.push_back(x4 + 1);
         crossSectionVector.push_back(y4);
         crossSectionVector.push_back(z4);
         crossSectionVector.push_back(x8 + 1);
         crossSectionVector.push_back(y8);
         crossSectionVector.push_back(z8);
         
         crossSectionVector.push_back(x3 + 1);
         crossSectionVector.push_back(y3);
         crossSectionVector.push_back(z3);
         crossSectionVector.push_back(x8 + 1);
         crossSectionVector.push_back(y8);
         crossSectionVector.push_back(z8);
         crossSectionVector.push_back(x7 + 1);
         crossSectionVector.push_back(y7);
         crossSectionVector.push_back(z7);
         
         crossSectionVector.push_back(x1 + 1);
         crossSectionVector.push_back(y1);
         crossSectionVector.push_back(z1);
         crossSectionVector.push_back(x4 + 1);
         crossSectionVector.push_back(y4);
         crossSectionVector.push_back(z4);
         crossSectionVector.push_back(x5 + 1);
         crossSectionVector.push_back(y5);
         crossSectionVector.push_back(z5);
         
         crossSectionVector.push_back(x4 + 1);
         crossSectionVector.push_back(y4);
         crossSectionVector.push_back(z4);
         crossSectionVector.push_back(x5 + 1);
         crossSectionVector.push_back(y5);
         crossSectionVector.push_back(z5);
         crossSectionVector.push_back(x8 + 1);
         crossSectionVector.push_back(y8);
         crossSectionVector.push_back(z8);
         
         crossSectionVector.push_back(x5 + 1);
         crossSectionVector.push_back(y5);
         crossSectionVector.push_back(z5);
         crossSectionVector.push_back(x6 + 1);
         crossSectionVector.push_back(y6);
         crossSectionVector.push_back(z6);
         crossSectionVector.push_back(x7 + 1);
         crossSectionVector.push_back(y7);
         crossSectionVector.push_back(z7);
         
         crossSectionVector.push_back(x6 + 1);
         crossSectionVector.push_back(y6);
         crossSectionVector.push_back(z6);
         crossSectionVector.push_back(x7 + 1);
         crossSectionVector.push_back(y7);
         crossSectionVector.push_back(z7);
         crossSectionVector.push_back(x8 + 1);
         crossSectionVector.push_back(y8);
         crossSectionVector.push_back(z8);*/
    }
    
    railPositions = new float[railVector.size()];
    
    CrossSectionPositions = new float[crossSectionVector.size()];
    
    for (int i = 0; i < crossSectionVector.size(); i++)
    {
        CrossSectionPositions[i] = crossSectionVector[i];
    }
    for (int i = 0; i < railVector.size(); i++)
    {
        railPositions[i] = railVector[i];
    }
    secondRailPositions = new float[railVector.size()];
    
    for (int i = 0; i < railVector.size(); i++)
    {
        secondRailPositions[i] = secondRailVector[i];
    }
    
    for (int i = 0; i < crossSectionVector.size(); i += 108)
    {
        for (int j = 0; j < 6; j++)
        {
            crossTextVector.push_back(0.0f);
            crossTextVector.push_back(0.0f);
            crossTextVector.push_back(1.0f);
            crossTextVector.push_back(0.0f);
            crossTextVector.push_back(0.0f);
            crossTextVector.push_back(1.0f);
            crossTextVector.push_back(1.0f);
            crossTextVector.push_back(1.0f);
        }
    }
    woodTextCoord = new float[crossTextVector.size()];
    for (int i = 0; i < crossTextVector.size(); i++)
    {
        woodTextCoord[i] = crossTextVector[i];
    }
    
    for (int i = 0; i < railTextVector.size(); i += 108)
    {
        for (int j = 0; j < 6; j++)
        {
            railTextVector.push_back(0.0f);
            railTextVector.push_back(0.0f);
            railTextVector.push_back(1.0f);
            railTextVector.push_back(0.0f);
            railTextVector.push_back(0.0f);
            railTextVector.push_back(1.0f);
            railTextVector.push_back(1.0f);
            railTextVector.push_back(1.0f);
        }
    }
    railTextCoord = new float[railTextVector.size()];
    for (int i = 0; i < railTextVector.size(); i++)
    {
        railTextCoord[i] = railTextVector[i];
    }
}

void initScene(int argc, char *argv[])
{
    
    //glGenTextures(1, &texHandle);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    // do additional initialization here...
    
    openGLMatrix = new OpenGLMatrix();
    pipelineProgram = new BasicPipelineProgram();
    pipelineProgram->Init("../openGLHelper-starterCode");
    pipelineProgram->Bind();
    program = pipelineProgram->GetProgramHandle();
    glGenTextures(1, &texHandle);
    glGenTextures(1, &SkytexHandle);
    glGenTextures(1, &WoodHandle);
    int code = initTexture("ground.jpg", texHandle);
    if (code != 0)
    {
        printf("Error loading the texture image.\n");
        exit(EXIT_FAILURE);
    }
    
    code = initTexture("wood.jpg", WoodHandle);
    if (code != 0)
    {
        printf("Error loading the texture image.\n");
        exit(EXIT_FAILURE);
    }
    
    code = initTexture("sky.jpg", SkytexHandle);
    if (code != 0)
    {
        printf("Error loading the texture image.\n");
        exit(EXIT_FAILURE);
    }
    
    code = initTexture("rail.jpg", RailtexHandle);
    if (code != 0)
    {
        printf("Error loading the texture image.\n");
        exit(EXIT_FAILURE);
    }
    
    
    calculatePoint();
    initVBO();
    initVAO();
    
    initThirdVBO();
    initThirdVAO();
    
    initSecondVBO();
    initSecondVAO();
    
    
    initGroundVBO();
    initGroundVAO();
    
    initSkyVBO();
    initSkyVAO();
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: %s <trackfile>\n", argv[0]);
        exit(0);
    }
    loadSplines(argv[1]);
    
    printf("Loaded %d spline(s).\n", numSplines);
    for (int i = 0; i < numSplines; i++)
        printf("Num control points in spline %d: %d.\n", i, splines[i].numControlPoints);
    
    cout << "Initializing GLUT..." << endl;
    glutInit(&argc, argv);
    
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
    //glutDisplayFunc(displayFunc1);
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
    
    //return 0;
    initScene(argc, argv);
    
    // sink forever into the glut loop
    glutMainLoop();
}
