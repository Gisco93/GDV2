// ========================================================================= //
// Author: Roman Getto, Matthias Bein, Johannes Fauser                       //
// mailto:johannes.fauser@gris.informatik.tu-darmstadt.de                    //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Creation Date: 26.05.2012                                                 //
// Changed: 06.06.2014                                                       //
// Changed: 04.06.2019                                                       //
// ========================================================================= //

#include "main.h"
#include "rendering.h"
#include "VolumeVisualization.h"

#include <GL/glut.h>

namespace
{
	using namespace gris;
	const auto filename = "C:/Users/Moritz/Documents/GDV2/Framework/data/neghip.raw";
}

int main(int argc, char **argv)
{
	std::cout << "main" << std::endl;
	//int wSizeH = 600;
	//int wSizeW = 400;
	//glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	//glutInitWindowPosition(200, 200);
	//glutInitWindowSize(wSizeH, wSizeW);
	//glutCreateWindow("Volume Visualization");
	//glutIgnoreKeyRepeat(1);
	//glutKeyboardFunc(keyPressed);
	//glutMouseFunc(mousePressed);
	//glutMotionFunc(mouseMoved);
	//glutDisplayFunc(renderScene);
	//glutReshapeFunc(changeSize);
	initialize(filename);
	VolumeVisualization::computeMesh(0.3);
	//glutMainLoop();
	return 0;
}