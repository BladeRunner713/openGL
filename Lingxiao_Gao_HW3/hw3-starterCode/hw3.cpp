/* **************************
 * CSCI 420
 * Assignment 3 Raytracer
 * Name: Lingxiao Gao
 * *************************
*/

#ifdef WIN32
  #include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
  #include <GL/gl.h>
  #include <GL/glut.h>
#elif defined(__APPLE__)
  #include <OpenGL/gl.h>
  #include <GLUT/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
  #define strcasecmp _stricmp
#endif

#include <imageIO.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>
#include <iostream>

#define MAX_TRIANGLES 20000
#define MAX_SPHERES 100
#define MAX_LIGHTS 100

char * filename = NULL;

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2

int mode = MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
#define WIDTH 640
#define HEIGHT 480
#define PI 3.14159265
#define EPSILON 0.00001

unsigned char buffer[HEIGHT][WIDTH][3];

struct Vertex
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double normal[3];
  double shininess;
};

struct Ray{
    glm::vec3 origin;
    glm::vec3 direction;
};

struct Triangle
{
  Vertex v[3];
};

struct Sphere
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double shininess;
  double radius;
};

struct Light
{
  double position[3];
  double color[3];
};

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles = 0;
int num_spheres = 0;
int num_lights = 0;

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);

void GenerateRay(int x, int y, Ray &ray);
glm::vec3 trace(const Ray &ray);

void Barycentric(const glm::vec3 &p, const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, float &u, float &v, float &w);
glm::vec3 triangleColor(glm::vec3 P, Triangle* triangle, float &u, float &v, float &w);
//glm::vec3 sphereColor(glm::vec3 P, Sphere* sphere);
bool intersect(const Sphere &s, const Ray &ray, float &t0, float &t1);
bool intersect(const Triangle &t, const Ray &ray, float &t0);

//glm::vec3 calcNormal();

//MODIFY THIS FUNCTION
void draw_scene()
{
  //a simple test output
  for(unsigned int x=0; x<WIDTH; x++)
  {
    glPointSize(2.0);  
    glBegin(GL_POINTS);
    for(unsigned int y=0; y<HEIGHT; y++)
    {
        glm::vec3 color;
        
        Ray r;
        GenerateRay(x, y, r);
        
        glm::vec3 vec = trace(r);
        if (length(vec) > 0){
            color.x = vec.x * 255.f;
            color.y = vec.y * 255.f;
            color.z = vec.z * 255.f;
        }
//        std::cout << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
       //plot_pixel(x, HEIGHT - y - 1, (int)color.x , (int)color.y , (int)color.z );
      plot_pixel(x, HEIGHT - y - 1, (int)color.x % 256, (int)color.y % 256, (int)color.z % 256);
        //plot_pixel(x, y, x % 256, y % 256, (x+y) % 256);
    }
    glEnd();
    glFlush();
  }
  printf("Done!\n"); fflush(stdout);
}

void GenerateRay(int w, int h, Ray& ray){
    float angle = tan(60.0 * PI * 0.5/180);
    float aspectratio = float(WIDTH) / float(HEIGHT);
    float invWidth = 1 / float(WIDTH), invHeight = 1 / float(HEIGHT);
    
    float x = (2 * ((w + 0.5) * invWidth) - 1) * angle * aspectratio;
    float y = (1 - 2 * ((h + 0.5) * invHeight)) * angle;
    float z = -1;
    
    ray.origin = glm::vec3(0,0,0);
    ray.direction = glm::vec3(x, y, z);
    ray.direction = normalize(ray.direction);
}

glm::vec3 trace(const Ray &ray){
    float tnear = INFINITY;
    Sphere* sphere = NULL;
    Triangle* triangle = NULL;

    for (unsigned int i = 0; i < num_spheres; i++) {
        float t0 = INFINITY, t1 = INFINITY;
        if (intersect(spheres[i], ray, t0, t1)) {
            if (t0 < 0) {
                t0 = t1;
            }
            if (t0 < tnear) {
                tnear = t0;
                sphere = &spheres[i];
            }
        }
    }
    
    for(unsigned int i = 0; i < num_triangles; i++){
        float t0 = INFINITY;
        if (intersect(triangles[i], ray, t0)) {
            if (t0 < tnear) {
                tnear = t0;
                triangle = &triangles[i];
            }
        }
    }
    //return backGround color
    if (!sphere && !triangle) {
        return glm::vec3(1.0f,1.0f,1.0f);
    }

    glm::vec3 finalColor(0.0f,0.0f,0.0f);
    
    glm::vec3 P = ray.origin + ray.direction * tnear; // point of intersection
    glm::vec3 normal;//normal at P
    glm::vec3 kd, ks, reflection;//coeffitions and reflection vector
    float sh;//shininess
    
    //if intersects with a sphere
    if (triangle == NULL) {
        ks = glm::vec3((float)sphere->color_specular[0],(float)sphere->color_specular[1],(float)sphere->color_specular[2]);
        kd = glm::vec3((float)(sphere->color_diffuse[0]),(float)(sphere->color_diffuse[1]),(float)(sphere->color_diffuse[2]));
        sh = sphere->shininess;
        
        normal = P - glm::vec3(sphere->position[0],sphere->position[1],sphere->position[2]);
        normal = glm::normalize(normal);
    }
    
    //if intersects with a triangle
    else {
        //for interpolation
        float alpha, beta, gamma;
        
        kd = triangleColor(P, triangle, alpha, beta, gamma);
        ks = alpha * glm::vec3((float)triangle->v[0].color_specular[0],
                               (float)triangle->v[0].color_specular[1],
                               (float)triangle->v[0].color_specular[2]) +
             beta * glm::vec3((float)triangle->v[1].color_specular[0],
                              (float)triangle->v[1].color_specular[1],
                              (float)triangle->v[1].color_specular[2]) +
             gamma * glm::vec3((float)triangle->v[2].color_specular[0],
                               (float)triangle->v[2].color_specular[1],
                               (float)triangle->v[2].color_specular[2]);
        sh = alpha * (float)(triangle->v[0].shininess) +
             beta * (float)(triangle->v[1].shininess) +
             gamma * (float)(triangle->v[2].shininess);

        normal = alpha * glm::vec3((float)(triangle->v[0].normal[0]),
                                   (float)(triangle->v[0].normal[1]),
                                   (float)(triangle->v[0].normal[2])) +
                 beta * glm::vec3((float)(triangle->v[1].normal[0]),
                                  (float)(triangle->v[1].normal[1]),
                                  (float)(triangle->v[1].normal[2])) +
                 gamma * glm::vec3((float)(triangle->v[2].normal[0]),
                                   (float)(triangle->v[2].normal[1]),
                                   (float)(triangle->v[2].normal[2]));
        normal = glm::normalize(normal);
    }
    
    for (unsigned int i = 0; i < num_lights; i++) {
        
        //setting up shadow ray
        Ray shadowRay;
        shadowRay.origin = P;
        glm::vec3 lpos(lights[i].position[0], lights[i].position[1], lights[i].position[2]);
        shadowRay.direction = lpos - P;
        shadowRay.direction = glm::normalize(shadowRay.direction);
        
        float dist = glm::distance(lpos, P);
        
        glm::vec3 Illumination;
        bool blocked = false;
        
        //checking for shadows
        for (unsigned int j = 0; j < num_spheres; j++) {
            float t0, t1;
            shadowRay.origin = P + normal * (float)EPSILON;
            if (intersect(spheres[j], shadowRay, t0, t1) && t0 < dist) {
                Illumination = glm::vec3(0.f,0.f,0.f);
                blocked = true;
                break;
            }
        }
        
        for(unsigned int j = 0; j < num_triangles; j++){
            float t0;
            shadowRay.origin = P + normal * (float)EPSILON;
            if (!blocked && intersect(triangles[j], shadowRay, t0) && t0 < dist ) {
                Illumination = glm::vec3(0.f,0.f,0.f);
                blocked = true;
                break;
            }
        }
        
        if (!blocked) {
            //calculating phong model
            glm::vec3 light = shadowRay.direction;
            glm::vec3 vertex = -ray.direction;
            
            reflection = 2.f * (glm::dot(light, normal) * normal) - light;
            
            glm::vec3 lightColor((float)lights[i].color[0], (float)lights[i].color[1], (float)lights[i].color[2]);
            
            float LN = fmax(glm::dot(light, normal),0);
            float RV = fmax(glm::dot(reflection, vertex),0);
            RV = pow(RV, sh);
            
            float Ir = lightColor.x * (kd.x * LN + ks.x * RV);
            float Ig = lightColor.y * (kd.y * LN + ks.y * RV);
            float Ib = lightColor.z * (kd.z * LN + ks.z * RV);
            
            Illumination = glm::vec3(fmin(Ir,1.f), fmin(Ig,1.f), fmin(Ib,1.f));
        }
        finalColor += Illumination;
    }
    
    //adding ambient light
    glm::vec3 ambientColor((float)ambient_light[0], (float)ambient_light[1], (float)ambient_light[2]);
    finalColor += ambientColor;
    
    finalColor.x = fmin(finalColor.x, 1.f);
    finalColor.y = fmin(finalColor.y, 1.f);
    finalColor.z = fmin(finalColor.z, 1.f);
    
    return finalColor;
}

void Barycentric(const glm::vec3 &p, const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, float &u, float &v, float &w){
    glm::vec3 v0 = b - a;
    glm::vec3 v1 = c - a;
    glm::vec3 v2 = p - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}

glm::vec3 triangleColor(glm::vec3 P, Triangle* triangle, float &u, float &v, float &w){
    glm::vec3 v1 = glm::vec3((float)triangle->v[0].position[0], (float)triangle->v[0].position[1], (float)triangle->v[0].position[2]);
    glm::vec3 v2 = glm::vec3((float)triangle->v[1].position[0], (float)triangle->v[1].position[1], (float)triangle->v[1].position[2]);
    glm::vec3 v3 = glm::vec3((float)triangle->v[2].position[0], (float)triangle->v[2].position[1], (float)triangle->v[2].position[2]);
    
    Barycentric(P, v1, v2, v3, u, v, w);
    
    glm::vec3 c1 = glm::vec3((float)triangle->v[0].color_diffuse[0], (float)triangle->v[0].color_diffuse[1], (float)triangle->v[0].color_diffuse[2]);
    glm::vec3 c2 = glm::vec3((float)triangle->v[1].color_diffuse[0], (float)triangle->v[1].color_diffuse[1], (float)triangle->v[1].color_diffuse[2]);
    glm::vec3 c3 = glm::vec3((float)triangle->v[2].color_diffuse[0], (float)triangle->v[2].color_diffuse[1], (float)triangle->v[2].color_diffuse[2]);
    
    return u * c1 + v * c2 + w * c3;
}

bool intersect(const Sphere &s, const Ray &ray, float &t0, float &t1){
    glm::vec3 d = ray.direction;
    glm::vec3 o = ray.origin;
    glm::vec3 c(s.position[0], s.position[1], s.position[2]);//center of sphere
    float r = (float)s.radius;
    float r2 = r*r;

    float fb = 2*(d.x * (o.x - c.x) + d.y * (o.y - c.y) + d.z * (o.z - c.z));
    float fc = pow((o.x - c.x),2) + pow((o.y - c.y),2) + pow((o.z - c.z),2) - r2;
    float fbc = pow(fb,2) - 4*fc;
    
    if (fbc < 0)
        return false;
    fbc = sqrt(fbc);
    
    t0 = (-fb - fbc)/2.0;
    t1 = (-fb + fbc)/2.0;
    
    if (t0 < 0) {
        return false;
    }
    
    return true;
}

bool intersect(const Triangle &triangle, const Ray &ray, float &t){
    glm::vec3 d = ray.direction;
    glm::vec3 o = ray.origin;
    
    glm::vec3 e1, e2;  //edge1, edge2
    glm::vec3 P, Q, T; //intersection,
    float det, invdet, u, v;
    
    //3 vertex of the triangle
    glm::vec3 v0 = glm::vec3((float)triangle.v[0].position[0], (float)triangle.v[0].position[1], (float)triangle.v[0].position[2]);
    glm::vec3 v1 = glm::vec3((float)triangle.v[1].position[0], (float)triangle.v[1].position[1], (float)triangle.v[1].position[2]);
    glm::vec3 v2 = glm::vec3((float)triangle.v[2].position[0], (float)triangle.v[2].position[1], (float)triangle.v[2].position[2]);

    //2 edges of the triangle extending from v1
    e1 = v1 - v0;
    e2 = v2 - v0;
    
    //calculate determinant
    P = glm::cross(d, e2);
    det = glm::dot(e1, P);
    
    //check if determinant is near zero
    if (det > -EPSILON && det < EPSILON) {
        return false;
    }
    invdet = 1.0f / det;
    
    //test u parameter
    T = o - v0;
    u = glm::dot(T, P) * invdet;
    if (u < 0.f || u > 1.f) {
        return false;
    }
    
    //test v parameter
    Q = glm::cross(T, e1);
    v = glm::dot(d, Q) * invdet;
    if (v < 0.f || u + v > 1.f) {
        return false;
    }
    
    t = glm::dot(e2, Q) * invdet;
    
    //ray intersects
    if (t > 0) {
        return true;
    }
    
    return false;
}

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
  glColor3f(((float)r) / 255.0f, ((float)g) / 255.0f, ((float)b) / 255.0f);
  glVertex2i(x,y);
}

void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
  buffer[y][x][0] = r;
  buffer[y][x][1] = g;
  buffer[y][x][2] = b;
}

void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
  plot_pixel_display(x,y,r,g,b);
  if(mode == MODE_JPEG)
    plot_pixel_jpeg(x,y,r,g,b);
}

void save_jpg()
{
  printf("Saving JPEG file: %s\n", filename);

  ImageIO img(WIDTH, HEIGHT, 3, &buffer[0][0][0]);
  if (img.save(filename, ImageIO::FORMAT_JPEG) != ImageIO::OK)
    printf("Error in Saving\n");
  else 
    printf("File saved Successfully\n");
}

void parse_check(const char *expected, char *found)
{
  if(strcasecmp(expected,found))
  {
    printf("Expected '%s ' found '%s '\n", expected, found);
    printf("Parse error, abnormal abortion\n");
    exit(0);
  }
}

void parse_doubles(FILE* file, const char *check, double p[3])
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check(check,str);
  fscanf(file,"%lf %lf %lf",&p[0],&p[1],&p[2]);
  printf("%s %lf %lf %lf\n",check,p[0],p[1],p[2]);
}

void parse_rad(FILE *file, double *r)
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check("rad:",str);
  fscanf(file,"%lf",r);
  printf("rad: %f\n",*r);
}

void parse_shi(FILE *file, double *shi)
{
  char s[100];
  fscanf(file,"%s",s);
  parse_check("shi:",s);
  fscanf(file,"%lf",shi);
  printf("shi: %f\n",*shi);
}

int loadScene(char *argv)
{
  FILE * file = fopen(argv,"r");
  int number_of_objects;
  char type[50];
  Triangle t;
  Sphere s;
  Light l;
  fscanf(file,"%i", &number_of_objects);

  printf("number of objects: %i\n",number_of_objects);

  parse_doubles(file,"amb:",ambient_light);

  for(int i=0; i<number_of_objects; i++)
  {
    fscanf(file,"%s\n",type);
    printf("%s\n",type);
    if(strcasecmp(type,"triangle")==0)
    {
      printf("found triangle\n");
      for(int j=0;j < 3;j++)
      {
        parse_doubles(file,"pos:",t.v[j].position);
        parse_doubles(file,"nor:",t.v[j].normal);
        parse_doubles(file,"dif:",t.v[j].color_diffuse);
        parse_doubles(file,"spe:",t.v[j].color_specular);
        parse_shi(file,&t.v[j].shininess);
      }

      if(num_triangles == MAX_TRIANGLES)
      {
        printf("too many triangles, you should increase MAX_TRIANGLES!\n");
        exit(0);
      }
      triangles[num_triangles++] = t;
    }
    else if(strcasecmp(type,"sphere")==0)
    {
      printf("found sphere\n");

      parse_doubles(file,"pos:",s.position);
      parse_rad(file,&s.radius);
      parse_doubles(file,"dif:",s.color_diffuse);
      parse_doubles(file,"spe:",s.color_specular);
      parse_shi(file,&s.shininess);

      if(num_spheres == MAX_SPHERES)
      {
        printf("too many spheres, you should increase MAX_SPHERES!\n");
        exit(0);
      }
      spheres[num_spheres++] = s;
    }
    else if(strcasecmp(type,"light")==0)
    {
      printf("found light\n");
      parse_doubles(file,"pos:",l.position);
      parse_doubles(file,"col:",l.color);

      if(num_lights == MAX_LIGHTS)
      {
        printf("too many lights, you should increase MAX_LIGHTS!\n");
        exit(0);
      }
      lights[num_lights++] = l;
    }
    else
    {
      printf("unknown type in scene description:\n%s\n",type);
      exit(0);
    }
  }
  return 0;
}

void display()
{
}

void init()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(0,WIDTH,0,HEIGHT,1,-1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  //hack to make it only draw once
  static int once=0;
  if(!once)
  {
    draw_scene();
    if(mode == MODE_JPEG)
      save_jpg();
  }
  once=1;
}

int main(int argc, char ** argv)
{
  if ((argc < 2) || (argc > 3))
  {  
    printf ("Usage: %s <input scenefile> [output jpegname]\n", argv[0]);
    exit(0);
  }
  if(argc == 3)
  {
    mode = MODE_JPEG;
    filename = argv[2];
  }
  else if(argc == 2)
    mode = MODE_DISPLAY;

  glutInit(&argc,argv);
  loadScene(argv[1]);

  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(WIDTH,HEIGHT);
  int window = glutCreateWindow("Ray Tracer");
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  init();
  glutMainLoop();
}

