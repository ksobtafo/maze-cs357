#include <GL/glew.h> // for OpenGL extensions
#include <GL/glut.h> // for Glut utility kit
#include <cassert>   // for assert
#include "vmath.h"
#include "shader.h"
#include <iostream>

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

// Array of rotation angles (in degrees) for each coordinate axis x, y, z
GLfloat  Theta[3] = { 0.0, 0.0, 0.0 };

// The location of the "theta" shader uniform variable which we ship Theta down to
GLuint  iTheta;

// Our data model
const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

							// Buffers to hold points and colors to ship to graphics card
vmath::vec4 *points;
vmath::vec4 *colors;

// Vertices of a unit cube centered at origin, sides aligned with axes
vmath::vec4 vertices[8] = {
	vmath::vec4(-0.15, 0.4,  0.5, 1.0),
	vmath::vec4(0,  0,  0.5, 1.0),
	vmath::vec4(0.5,  0.5,  0.5, 1.0),
	vmath::vec4(1, -0.25, -0.5, 1.0),

	vmath::vec4(-0.15, 0.4,  -0.5, 1.0),
	vmath::vec4(0,  0,  -0.5, 1.0),
	vmath::vec4(0.5,  0.5,  -0.5, 1.0),
	vmath::vec4(1, -0.25, -0.5, 1.0)
	
};

// RGBA Colors
vmath::vec4 vertex_colors[8] = {
	vmath::vec4(0.0, 0.0, 0.0, 1.0),  // black
	vmath::vec4(1.0, 0.0, 0.0, 1.0),  // red
	vmath::vec4(1.0, 1.0, 0.0, 1.0),  // yellow
	vmath::vec4(0.0, 1.0, 0.0, 1.0),  // green
	vmath::vec4(0.0, 0.0, 1.0, 1.0),  // blue
	vmath::vec4(1.0, 0.0, 1.0, 1.0),  // magenta
	vmath::vec4(1.0, 1.0, 1.0, 1.0),  // white
	vmath::vec4(0.0, 1.0, 1.0, 1.0)   // cyan
};

int Index = 0;

void line(int a, int b)
{
	// Add vertices for the line
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
}

// generate 12 lines: 24 vertices and 24 colors
void meshcube()
{
	points = new vmath::vec4[24];
	colors = new vmath::vec4[24];

	line(0, 2);
	line(1, 3);
	line(2, 4);
	line(3, 5);

	line(4, 6);
	line(5, 7);
	line(6, 4);
	line(7, 5);

	line(0, 4);
	line(1, 5);
	line(2, 6);
	line(3, 7);


}

void cleanup()
{
	delete[] points;
	delete[] colors;
}
//----------------------------------------------------------------------------

GLuint  shaders[2];  // Storage for shader id's
GLuint  program;    // storage for the program id
GLuint  vao;        // storage for vertex array object id
GLuint  buffers[2]; // Storage for buffer id's
GLuint  locations[2]; // Storage for location ids

					  //----------------------------------------------------------------------------
					  // Setup the variables to control the perspective projection frustum
					  // See page 88 in OpenGL SuperBible
					  //
GLuint iNear, iFar, iRight, iLeft, iTop, iBottom;
GLfloat znear = 1.0, zfar = -1.0, xright = 1.0, xleft = -1.0, ytop = 1.0, ybottom = -1.0;

// OpenGL initialization
void init()
{
	// Compile the shaders
	shaders[0] = sb7::shader::load("vs.glsl", GL_VERTEX_SHADER);
	shaders[1] = sb7::shader::load("fs.glsl", GL_FRAGMENT_SHADER);

	program = sb7::program::link_from_shaders(shaders, 2, true);

	// call meshcube to initialize vertex arrays
	meshcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// set up vertex array for the points
	glGenBuffers(1, &buffers[0]);
	locations[0] = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(locations[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vmath::vec4)*NumVertices, points, GL_STATIC_DRAW);
	glVertexAttribPointer(locations[0], 4, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK_ERRORS

		// set up array for the vertex colors
		glGenBuffers(1, &buffers[1]);
	locations[1] = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(locations[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vmath::vec4)*NumVertices, colors, GL_STATIC_DRAW);
	glVertexAttribPointer(locations[1], 4, GL_FLOAT, GL_FALSE, 0, 0);
	GL_CHECK_ERRORS

		// Get the location of the uniform variables in the shader program
		iTheta = glGetUniformLocation(program, "theta");
	iNear = glGetUniformLocation(program, "near");
	iFar = glGetUniformLocation(program, "far");
	iRight = glGetUniformLocation(program, "right");
	iLeft = glGetUniformLocation(program, "left");
	iTop = glGetUniformLocation(program, "top");
	iBottom = glGetUniformLocation(program, "bottom");

	// Enable depth testing so we can remove hidden faces
	glEnable(GL_DEPTH_TEST);
	GL_CHECK_ERRORS
}

//----------------------------------------------------------------------------
void display(void)
{
	// Clear the color buffer and the depth buffer
	// if we don't clear the depth buffer we won't see
	// anything since we are doing depth testing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader program
	glUseProgram(program);

	// Set rotation angles in degrees based off of the time
	Theta[0] = 0.0;
	Theta[1] += 1.0;
	Theta[2] = 0.0;

	// Ship data down to the shader program
	glUniform3fv(iTheta, 1, Theta);
	glUniform1f(iNear, znear);
	glUniform1f(iFar, zfar);
	glUniform1f(iRight, xright);
	glUniform1f(iLeft, xleft);
	glUniform1f(iTop, ytop);
	glUniform1f(iBottom, ybottom);

	// Draw the triangles in the buffers we shipped down earlier
	glDrawArrays(GL_LINES, 0, NumVertices);

	// Swap buffers
	glutSwapBuffers();

	// Redraw the screen
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
GLfloat aspect;

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	aspect = GLfloat(width) / height;
}



void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;

	case 'z': znear *= 1.1; zfar *= 1.1;    break;
	case 'Z': znear *= 0.9; zfar *= 0.9;    break;
	case 'x': xleft *= 1.1; xright *= 1.1;  break;
	case 'X': xleft *= 0.9; xright *= 0.9;  break;
	case 'y': ytop *= 1.1;  ybottom *= 1.1; break;
	case 'Y': ytop *= 0.9;  ybottom *= 0.9; break;
	case ' ':  // reset values to their defaults
		znear = 10.0; zfar = -10.0;
		xright = 1.0; xleft = -1.0;
		ytop = 1.0; ybottom = -1.0;

		break;
	}

	// Ship Theta angles down to the shader program
	glUniform1f(iNear, znear);
	glUniform1f(iFar, zfar);
	glUniform1f(iRight, xright);
	glUniform1f(iLeft, xleft);
	glUniform1f(iTop, ytop);
	glUniform1f(iBottom, ybottom);

	glutPostRedisplay();
}


//----------------------------------------------------------------------------


int main(int argc, char **argv)
{
	std::cout << "View Frustum Matrix Example " << std::endl;
	std::cout << "Instructions: " << std::endl;
	std::cout << "z, Z: to decrease/increase z=near and Z=far" << std::endl;
	std::cout << "x, X: to decrease/increase x=right and x=left" << std::endl;
	std::cout << "z, Z: to decrease/increase y=top and y=bottom" << std::endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);

	glutCreateWindow("Color Cube");

	glewInit();

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	glutMainLoop();

	return 0;
}