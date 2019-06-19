cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
project(PraktikumGDV2)

set(Includes
	include/dualpointslist.h
	include/mcLookUp.h
	include/rendering.h
	include/TriangleMesh.h
	include/Vec3.h
	include/VolumeVisualization.h
)

file(GLOB_RECURSE ${PROJECT_NAME}_DATA "data/*.raw")

file(COPY Data DESTINATION /)

file(COPY "freeglut.dll" DESTINATION /)

source_group(Data FILES ${${PROJECT_NAME}_DATA})

set(Sources
	main.cpp
	rendering.cpp
	TriangleMesh.cpp
	VolumeVisualization.cpp
)

       
# files of project
add_executable(${PROJECT_NAME}
	${Includes}
	${Sources}
	${${PROJECT_NAME}_DATA}
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_SOURCE_DIR}/include/
	)

option(AUTO_SEARCH_AND_INCLUDE_OpenGL "You can activate this option or include OpenGL by yourself" ON)
option(AUTO_SEARCH_AND_INCLUDE_Glut "You can activate this option or include GLUT by yourself" ON)
option(AUTO_SEARCH_AND_INCLUDE_Glew "You can activate this option or include GLEW by yourself" ON)

# FIND AND INCLUDE OPENGL				
if(AUTO_SEARCH_AND_INCLUDE_OpenGL)
	find_package(OpenGL REQUIRED)
	include_directories(${PROJECT_NAME} ${OpenGL_INCLUDE_DIR} 
		)
	target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
endif()

# FIND AND INCLUDE GLUT 				 
if(AUTO_SEARCH_AND_INCLUDE_Glut)
	find_package(GLUT REQUIRED)
	include_directories( ${GLUT_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${GLUT_LIBRARIES})        
endif()

# FIND AND INCLUDE GLEW                                                   
if(AUTO_SEARCH_AND_INCLUDE_Glew)
	find_package(GLEW REQUIRED)
	include_directories( ${GLEW_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
else()
# At least define the glew library if the package is not searched       
    if (WIN32)
		set(GLEW_LIBRARY GLEW32)
    else()
		set(GLEW_LIBRARY GLEW)
    endif()
    target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARY})
endif()