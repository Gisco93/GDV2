#include "rendering.h"
#include "VolumeVisualization.h"

#include <GL/glut.h>

#include <algorithm>
//#include <iostream>
#include <fstream>

namespace
{
  using namespace gris;
  GLdouble near1 = 0.1, far1 = 30;
  // Camera information
  Vec3f camPos(0.5f, 0.5f, -2.0f);        // camera position
  Vec3f camDir(0.0f, 0.0f, 1.0f);         // camera lookat (always Z)
  Vec3f camUp(0.0f, 1.0f, 0.0f);          // camera up direction (always Y)
  float camAngleX=0.0f, camAngleY=0.0f;   // camera angles
  // Mouse information
  int mouseX, mouseY, mouseButton;
  float mouseSensitivy = 1.0f;
  // Iso surface information
  float isovalue = 0.3; 
  
  VolumeVisualization volumevis;

  int wSizeH = 600;
  int wSizeW = 400;
}

namespace gris
{
  void initialize(const std::string& filename, int fileDim)
  {
    glClearColor(1.0,1.0,1.0,0.0);
    // enable depth buffer
    glEnable(GL_DEPTH_TEST);
    // shading model
    glShadeModel(GL_SMOOTH);
    // size window
    changeSize(wSizeH,wSizeW);
    // load a volume data set
    std::ifstream vin(filename, std::ios::binary);  
    //volumevis.loadRAW(vin,64,64,64);
	volumevis.loadRAW(vin, fileDim, fileDim, fileDim);
	//volumevis.generateBarthSextic(48, 1, 0.5*(1+sqrt(5)), 1);
	volumevis.computeMesh(isovalue);
    // cout
    std::cout << "(Simple) Volume Data Visualization\n";
    std::cout << "Usage:\nesc: exit program\n  -: decrease threshold (isovalue)\n  +: increase threshold (isovalue) \n  s: save Mesh to .ply File\n \n";
    std::cout << "mouse left: rotate\nmouse middle: move (pan)\nmouse right: zoom" << std::endl;
  }

  void changeSize(int w, int h)
  {
    // Prevent a division by zero, when window is too short
    if(h == 0) h = 1;
    float wRatio = 1.0* w / h;
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);
    // Set the correct perspective.
    gluPerspective(45,wRatio,near1,far1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camPos.x,             camPos.y,             camPos.z,               // Position
      camPos.x + camDir.x,  camPos.y + camDir.y,  camPos.z + camDir.z,    // Lookat
      camUp.x,              camUp.y,              camUp.z);               // Up-direction
  }

  // Rendering
  void renderScene(void)
  {
    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    // apply camPos before rotation
    glLoadIdentity();  
    gluLookAt(camPos.x,             camPos.y,             camPos.z,               // Position
      camPos.x + camDir.x,  camPos.y + camDir.y,  camPos.z + camDir.z,    // Lookat
      camUp.x,              camUp.y,              camUp.z);               // Up-direction
                                                                          // apply rotation
    glRotatef(camAngleX,0,1,0); // window x axis rotates around up vector
    glRotatef(camAngleY,1,0,0); // window y axis rotates around x 
                                // draw scene
    //drawPoints(isovalue);
	drawMesh(isovalue);
    // swap Buffers
    glutSwapBuffers();
  }

  // Callbacks

  void keyPressed(unsigned char key, int x, int y)
  {
    float increment = 0.05;
    switch (key) {
      // esc => exit
    case 27:
      exit(0);
      break;
      // v => reset view
    case 'v':
    case 'V':
      camPos.set(0.0f, 2.0f, -10.0f);
      camAngleX = 180.0f;
      camAngleY = 0.0f;
      break;
    case 's':
    case 'S':
      volumevis.getMesh()->saveAsPly("MarchingCubesOutput");
      break;
      // decrement iso value
    case '-':
      isovalue -= increment;
      if (isovalue < 0) isovalue = 0;
      break;
      // increment iso value
    case '+': 
      isovalue += increment;
      if (isovalue > 1) isovalue = 1;
      break;
    }
	(*volumevis.getMesh()).clear();
	volumevis.computeMesh(isovalue);
    glutPostRedisplay();
  }

  void mousePressed(int button, int state, int x, int y)
  {
    switch(button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        mouseButton = 1;
        mouseX = x;
        mouseY = y;
      }
      else mouseButton = 0;
      break;
    case GLUT_RIGHT_BUTTON:
      if (state == GLUT_DOWN) {
        mouseButton = 2;
        mouseX = x;
        mouseY = y;
      }
      else mouseButton = 0;
      break;
    case GLUT_MIDDLE_BUTTON:      
      if (state == GLUT_DOWN) {
        mouseButton = 3;
        mouseX = x;
        mouseY = y;
      }
      else mouseButton = 0;
      break;
    }
  }

  void mouseMoved(int x, int y)
  { 
    switch(mouseButton) {
      // 1 => rotate
    case 1:
      // update angle with relative movement
      camAngleX = fmod(camAngleX + (x-mouseX)*mouseSensitivy,360.0f);
      camAngleY -= (y-mouseY)*mouseSensitivy;
      // limit y angle by 85 degree
      if (camAngleY > 85) camAngleY = 85;
      if (camAngleY < -85) camAngleY = -85;
      break;
      // 2 => zoom
    case 2: 
      camPos -= Vec3f(0.0f,0.0f,0.1f)*(y-mouseY)*mouseSensitivy;
      break;
      // 3 => translate 
    case 3:
      // update camPos
      camPos += Vec3f(0.01f,0.0f,0.0f)*(x-mouseX)*mouseSensitivy;
      camPos += Vec3f(0.0f,0.01f,0.0f)*(y-mouseY)*mouseSensitivy;
      break;
    default: break;
    }  
    // update mouse for next relative movement
    mouseX = x;
    mouseY = y;
    // redraw if mouse moved, since there is no idleFunc defined
    glutPostRedisplay();
  }

  void drawPoints(float isovalue)
  {    
    Vec3i* dimension = volumevis.getDimension();
    Vec3f* spacing = volumevis.getSpacing();
    std::vector<float>* volumeData = volumevis.getVolumeData();
    // first, find largest dimension
    int dimMax = std::max(std::max(spacing->x*dimension->x, spacing->y*dimension->y), spacing->z*dimension->z);
    // now, scale such that largest edge of our cube is one unit ! 
    float scaleFac = 1.0f / dimMax;
    glScalef(scaleFac, scaleFac, scaleFac);
    glPointSize(2.0);
    glBegin(GL_POINTS);
    for (int z = 0; z < dimension->z; ++z)  {
      for (int y = 0; y < dimension->y; ++y)  {
        for (int x = 0; x < dimension->x; ++x)  {
          float value = volumeData->at(z*dimension->x*dimension->y + y*dimension->x + x);
          if (value > isovalue) {
            glColor3f(value,value,value);
            glVertex3f(x,y,z);
          }
        }
      }
    }
    glEnd();
  }

  void drawMesh(float isovalue)
  {

	  Vec3i* dimension = volumevis.getDimension();
	  Vec3f* spacing = volumevis.getSpacing();
	  std::vector<float>* volumeData = volumevis.getVolumeData();
	  // first, find largest dimension
	  int dimMax = std::max(std::max(spacing->x*dimension->x, spacing->y*dimension->y), spacing->z*dimension->z);
	  // now, scale such that largest edge of our cube is one unit ! 
	  float scaleFac = 1.0f / dimMax;
	  glScalef(scaleFac, scaleFac, scaleFac);
	  glPointSize(2.0);
    //TODO
	  
	  TriangleMesh* mesh = volumevis.getMesh();

	  glBegin(GL_TRIANGLES);
	  Vec3f v;
	  glColor3f(1.0f, 0.0f, 0.0f); 
	  //std::cout << (*mesh).getTriangles().size() << "\n";
	  for (int i = 0; i < (*mesh).getTriangles().size(); i++) {
		  v = (*mesh).getVertices().at(i*3 +0);
		  glVertex3f(v.x, v.y, v.z);
		  v = (*mesh).getVertices().at(i * 3 + 1);
		  glVertex3f(v.x, v.y, v.z);
		  v = (*mesh).getVertices().at(i * 3 + 2);
		  glVertex3f(v.x, v.y, v.z);
	  }
	  glEnd();
	  //(*mesh).clear();

  }
}