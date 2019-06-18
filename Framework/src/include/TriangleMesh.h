// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Changed: 06.06.2014                             //
// Creation Date: 19.05.2012                                                 //
// Content: Simple class for triangle meshes                   //
// ==========================================================================//
#pragma once
#include "Vec3.h"

#include <string>
#include <vector>

namespace gris
{
  /** \brief
  */
  class TriangleMesh
  {
    public:
     // typedefs for data
      typedef Vec3i Triangle;
      typedef Vec3f Normal;
      typedef Vec3f Vertex;
      typedef std::vector<Triangle> Triangles;
      typedef std::vector<Normal> Normals;
      typedef std::vector<Vertex> Vertices;  

    public:
      TriangleMesh();
      ~TriangleMesh();

    public:
      // clears all data, sets defaults
      void clear();
      // get raw data references
      Vertices&   getVertices()   { return vertices; }
      Triangles&  getTriangles()  { return triangles; }
      Normals&    getNormals()    { return normals; }

    public:
      void saveAsPly(std::string outputfilename);

    private:
      // data of TriangleMesh
      Vertices vertices;
      Normals normals;
      Triangles triangles;
  };
}

