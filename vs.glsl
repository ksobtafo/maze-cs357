#version 400 core
#extension GL_ARB_explicit_uniform_location : enable
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) uniform vec3 theta;
layout(location = 3) uniform float near;
layout(location = 4) uniform float far;
layout(location = 5) uniform float right;
layout(location = 6) uniform float left;
layout(location = 7) uniform float top;
layout(location = 8) uniform float bottom;


uniform vec4 TransVec;  // Translation Vector

out vec4 color;

void main()
{
	// Compute the sines and cosines of theta for each of
	// the three axes in one computation.
	vec3 angles = radians(theta);
	vec3 c = cos(angles);
	vec3 s = sin(angles);

	mat4 scale = mat4(
		0.5, 0.0, 0.0, 0.0, // Column 1
		0.0, 0.5, 0.0, 0.0, // Column 2
		0.0, 0.0, 0.5, 0.0, // Column 3
		0.0, 0.0, 0.0, 1.0); // Column 4

	mat4 TCenter = mat4(
		1.0, 0.0, 0.0, -0.5, // Column 1
		0.0, 1.0, 0.0, -0.5, // Column 2
		0.0, 0.0, 1.0, -0.5, // Column 3
		0.0, 0.0, 0.0, 1.0); // Column 4

	mat4 Tpos = mat4(
		1.0, 0.0, 0.0, 0.0, // Column 1
		0.0, 1.0, 0.0, 0.0, // Column 2
		0.0, 0.0, 1.0, -0.5, // Column 3
		0.0, 0.0, 0.0, 1.0); // Column 4

	mat4 ry = mat4(c.y, 0.0, -s.y, 0.0,
		0.0, 1.0, 0.0, 0.0,
		s.y, 0.0, c.y, 0.0,
		0.0, 0.0, 0.0, 1.0);

	// Perspective Frustum Matrix : See page 99 in OpenGL Superbible
	// NOTE: the last element is NOT zero as in the  matrix on page 88
	// since for depth testing we need the z value
	mat4 persp = mat4(
		(2.0*near)/(right-left), 0.0, 0.0, 0.0,  // Column 1
		0.0, (2.0*near) / (top - bottom), 0.0, 0.0,  // Column 2
		(right+left)/(right-left), (top+bottom)/(top-bottom), (near+far)/(near-far), -1.0,  // Column 3
		0.0, 0.0, (2.0*near*far)/(near-far), 1.0); // Column 4

	color = vColor;
	gl_Position = vPosition * TCenter * scale * ry * Tpos * persp +TransVec ;
}

