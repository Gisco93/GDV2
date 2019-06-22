#pragma once

#include <string>

namespace gris
{
  // initialize OpenGL
  void initialize(const std::string& filename, int fileDim);
  void changeSize(int w, int h);
  
  // Rendering
  void renderScene(void);
  // Callbacks
  void keyPressed(unsigned char key, int x, int y);
  void mousePressed(int button, int state, int x, int y);
  void mouseMoved(int x, int y);

  // Volume Vis
  void drawPoints(float isovalue);
  void drawMesh(float isovalue);
}