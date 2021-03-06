// ========================================================================= //
// Authors: Roman Getto, Matthias Bein                                       //
// mailto:roman.getto@gris.informatik.tu-darmstadt.de                        //
//                                                                           //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universit�t Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Creation Date: 06.06.2014                                                 //
// Content: Simple class for triangle meshes                   //
// ==========================================================================//

#include "VolumeVisualization.h"

#include "dualpointslist.h"
#include "mcLookUp.h"
#include "TriangleMesh.h"

namespace gris
{
	/**
	*/
	void VolumeVisualization::loadRAW(std::istream& in, int dimX, int dimY, int dimZ, float dx, float dy, float dz)
	{
		spacing.x = dx; spacing.y = dy; spacing.z = dz;
		dimension.x = dimX; dimension.y = dimY; dimension.z = dimZ;
		volumedata.clear();
		volumedata.resize(dimX*dimY*dimZ);
		// we know the bit depth (8 bits by now), so all that's left is to cast each char to a float between [0,1) 
		for (int i = 0; i < dimX*dimY*dimZ; ++i)
		{
			volumedata[i] = ((unsigned char)in.get()) / 256.0f;
		}
	}

	void VolumeVisualization::generateBarthSextic(int dimXYZ, int spacingXYZ, float scale, float phi, float w) {
		spacing.x = spacingXYZ; spacing.y = spacingXYZ; spacing.z = spacingXYZ;
		dimension.x = dimXYZ; dimension.y = dimXYZ; dimension.z = dimXYZ;
		volumedata.clear();
		volumedata.resize(dimXYZ*dimXYZ*dimXYZ);

		for (int x = 0; x < dimXYZ; x++) {
			for (int y = 0; y < dimXYZ; y++) {
				for (int z = 0; z < dimXYZ; z++) {
					float locX = (x-dimXYZ/2)*scale; float locY = (y-dimXYZ/2)*scale; float locZ = (z-dimXYZ/2)*scale;
					volumedata[x + y * dimXYZ + z * dimXYZ * dimXYZ] =
						4 * (phi*phi*locX*locX - locY*locY)*(phi*phi*locY*locY - locZ*locZ)*(phi*phi*locZ*locZ - locX*locX) -
						(1 + 2 * phi)*(locX*locX + locY*locY + locZ*locZ - w*w)*(locX*locX + locY*locY + locZ*locZ - w*w)*w*w;
					//std::cout << volumedata[x + y * dimXYZ + z * dimXYZ * dimXYZ] << "\n";
				}
			}
		}
	}

	/**
	*/
	void VolumeVisualization::computeMesh(float isovalue)
	{
		//TODO
		std::cout << "compute mesh" << std::endl;
		auto vertices = mesh.getVertices();

		//ggf. order tauschen => speicher zugriffs effizienz
		for (int x = 0; x < dimension.x-1; x++) {
			for (int y = 0; y < dimension.y-1; y++) {
				for (int z = 0; z < dimension.z-1; z++) {
					//x,y,z indexing with x being slice direction... may needs change to 
					//volumedata[x * dimension.y * dimension.z + y * dimension.z + z];
					GRIDCELL cube;//= new GRIDCELL;
					// on unit Cube: 0,0,0;|0
					cube.val[0] = getVolumeData(x, y, z);
					cube.p[0].set(x, y, z);
					// 1,0,0 |1
					cube.val[1] = getVolumeData(x + 1, y, z);
					cube.p[1].set(x + 1, y, z);
					// 1,0,1 |2
					cube.val[2] = getVolumeData(x + 1, y, z + 1);
					cube.p[2].set(x + 1, y, z + 1);
					// 0,0,1 |3
					cube.val[3] = getVolumeData(x, y, z + 1);
					cube.p[3].set(x, y, z + 1);
					// 0,1,0 |4
					cube.val[4] = getVolumeData(x, y + 1, z);
					cube.p[4].set(x, y + 1, z);
					// 1,1,0 |5
					cube.val[5] = getVolumeData(x + 1, y + 1, z);
					cube.p[5].set(x + 1, y + 1, z);
					// 1,1,1 |6
					cube.val[6] = getVolumeData(x + 1, y + 1, z + 1);
					cube.p[6].set(x + 1, y + 1, z + 1);
					// 0,1,1 |7
					cube.val[7] = getVolumeData(x, y + 1, z + 1);
					cube.p[7].set(x, y + 1, z + 1);

					MC_TRIANGLE triangles[5];
					auto numtriangles = Polygonise(cube, isovalue, triangles);
					//std::cout << numtriangles << std::endl;
					for (int i = 0; i < numtriangles; i++) {
						// VertexInterp muss hier benutzt werden zum erzeugen der vertices (3 pro triangle)
						mesh.getVertices().push_back(triangles[i].p[0]);
						mesh.getVertices().push_back(triangles[i].p[1]);
						mesh.getVertices().push_back(triangles[i].p[2]);
						//??? nicht was ich als dreiecke speichern w�rde???
						mesh.getTriangles().push_back(Vec3i(3*i, 3*i+1, 3*i+2));
						// sind g schon die normalen?
						//mesh.getNormals().push_back(crossProduct(triangles[i].p[1]- triangles[i].p[0], triangles[i].p[2]- triangles[i].p[0]));
						Vec3f normal = (triangles[i].p[1] - triangles[i].p[0]) ^ (triangles[i].p[2] - triangles[i].p[0]);
						normal.normalize();
						mesh.getNormals().push_back(normal);
					}

				}
			}
		}
	}

	float VolumeVisualization::getVolumeData(int x, int y, int z) {
		int pos = x  +  dimension.x * y  + dimension.x* dimension.y * z;
		return volumedata[pos];
	}

	/**
	*/
	unsigned int VolumeVisualization::Polygonise(GRIDCELL grid, float isolevel, MC_TRIANGLE *triangles)
	{
		int i, ntriang, cubeindex;
		Vec3f vertlist[12];
		// Determine the index into the edge table which tells us which vertices are inside of the surface
		cubeindex = 0;
		if (grid.val[0] < isolevel) cubeindex |= 1;
		if (grid.val[1] < isolevel) cubeindex |= 2;
		if (grid.val[2] < isolevel) cubeindex |= 4;
		if (grid.val[3] < isolevel) cubeindex |= 8;
		if (grid.val[4] < isolevel) cubeindex |= 16;
		if (grid.val[5] < isolevel) cubeindex |= 32;
		if (grid.val[6] < isolevel) cubeindex |= 64;
		if (grid.val[7] < isolevel) cubeindex |= 128;
		// Cube is entirely in/out of the surface
		if (edgeTable[cubeindex] == 0)
			return(0);
		// Find the edges where the surface intersects the cube
		if (edgeTable[cubeindex] & 1)
			vertlist[0] =
			VertexInterp(isolevel, grid.p[0], grid.p[1], grid.val[0], grid.val[1]);
		if (edgeTable[cubeindex] & 2)
			vertlist[1] =
			VertexInterp(isolevel, grid.p[1], grid.p[2], grid.val[1], grid.val[2]);
		if (edgeTable[cubeindex] & 4)
			vertlist[2] =
			VertexInterp(isolevel, grid.p[2], grid.p[3], grid.val[2], grid.val[3]);
		if (edgeTable[cubeindex] & 8)
			vertlist[3] =
			VertexInterp(isolevel, grid.p[3], grid.p[0], grid.val[3], grid.val[0]);
		if (edgeTable[cubeindex] & 16)
			vertlist[4] =
			VertexInterp(isolevel, grid.p[4], grid.p[5], grid.val[4], grid.val[5]);
		if (edgeTable[cubeindex] & 32)
			vertlist[5] =
			VertexInterp(isolevel, grid.p[5], grid.p[6], grid.val[5], grid.val[6]);
		if (edgeTable[cubeindex] & 64)
			vertlist[6] =
			VertexInterp(isolevel, grid.p[6], grid.p[7], grid.val[6], grid.val[7]);
		if (edgeTable[cubeindex] & 128)
			vertlist[7] =
			VertexInterp(isolevel, grid.p[7], grid.p[4], grid.val[7], grid.val[4]);
		if (edgeTable[cubeindex] & 256)
			vertlist[8] =
			VertexInterp(isolevel, grid.p[0], grid.p[4], grid.val[0], grid.val[4]);
		if (edgeTable[cubeindex] & 512)
			vertlist[9] =
			VertexInterp(isolevel, grid.p[1], grid.p[5], grid.val[1], grid.val[5]);
		if (edgeTable[cubeindex] & 1024)
			vertlist[10] =
			VertexInterp(isolevel, grid.p[2], grid.p[6], grid.val[2], grid.val[6]);
		if (edgeTable[cubeindex] & 2048)
			vertlist[11] =
			VertexInterp(isolevel, grid.p[3], grid.p[7], grid.val[3], grid.val[7]);
		// Create the triangles
		ntriang = 0;
		for (i = 0; triTable[cubeindex][i] != -1; i += 3)
		{
			triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i]];
			triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i + 1]];
			triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i + 2]];
			++ntriang;
		}
		return(ntriang);
	}

	/**
	*/
	Vec3f VolumeVisualization::VertexInterp(float isolevel, Vec3f p1, Vec3f p2, float valp1, float valp2)
	{
		if (abs(isolevel - valp1) < 0.00001)
			return(p1);
		if (abs(isolevel - valp2) < 0.00001)
			return(p2);
		if (abs(valp1 - valp2) < 0.00001)
			return(p1);
		float mu;
		Vec3f p;
		mu = (isolevel - valp1) / (valp2 - valp1);
		p.x = p1.x + mu * (p2.x - p1.x);
		p.y = p1.y + mu * (p2.y - p1.y);
		p.z = p1.z + mu * (p2.z - p1.z);
		return p;
	}

	Vec3f VolumeVisualization::crossProduct(Vec3f vector1, Vec3f vector2) {
		Vec3f normalVector;

		// Cross product. The normalVector contains the normal for the
		// surface, which is perpendicular both to vector1 and vector2.
		normalVector.x = vector1.y * vector2.z - vector1.z * vector2.y;
		normalVector.y = vector1.z * vector2.x - vector1.x * vector2.z;
		normalVector.z = vector1.x * vector2.y - vector1.y * vector2.x;

		return normalVector;
	}
}