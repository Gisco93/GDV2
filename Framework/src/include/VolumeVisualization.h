// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Creation Date: 06.06.2014                                                 //
// Content: Simple class for triangle meshes                   //
// ==========================================================================//

#pragma once

#include "Vec3.h"
#include "TriangleMesh.h"

#include <cmath>
#include <iostream>
#include <vector>

namespace gris
{
  /** \brief
  */
  class VolumeVisualization 
  {
    private:
      typedef struct 
      {
        Vec3f p[3];
        Vec3f g[3];
      } MC_TRIANGLE;

      typedef struct 
      {
        Vec3f p[8];
        float val[8];
      } GRIDCELL;  

    public:
      Vec3i*              getDimension()  { return &dimension; }
      Vec3f*              getSpacing()    { return &spacing; }
      std::vector<float>* getVolumeData() { return &volumedata; }
      TriangleMesh*       getMesh()       { return &mesh; }
      
    public:
      void loadRAW(std::istream& in, int dimX, int dimY, int dimZ, float dx=1, float dy=1, float dz=1);
      void computeMesh(float isovalue);

	  float VolumeVisualization::getVolumeData(int x, int y, int z);
	  void VolumeVisualization::generateBarthSextic(int dimXYZ, int spacingXYZ, float phi, float w);

      // Given a grid cell and an isolevel, calculate the triangular
      //  facets required to represent the isosurface through the cell.
      // Return the number of triangular facets, the array "triangles"
      //  will be loaded up with the vertices at most 5 triangular facets.
      //  0 will be returned if the grid cell is either totally above
      //  of totally below the isolevel.
      unsigned int Polygonise(GRIDCELL grid,float isolevel,MC_TRIANGLE *triangles);

      // Linearly interpolate the position where an isosurface cuts an
      //  edge between two vertices, each with their own scalar value
      Vec3f VertexInterp(float isolevel, Vec3f p1, Vec3f p2, float valp1, float valp2);

    private:
      typedef std::vector<float> VolumeData;

    private:
      // data
      VolumeData volumedata;
      Vec3i dimension;
      Vec3f spacing;
      //mesh
      TriangleMesh mesh;
      // lookup tables for all 256 mc cases
      static const int edgeTable[256];
      static const int triTable[256][16];
  };
}