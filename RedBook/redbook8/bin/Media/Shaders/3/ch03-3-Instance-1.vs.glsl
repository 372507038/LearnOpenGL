// ch03-3-Instance-1.vs.glsl

#version 330

// position and normal are regular vertex attributes
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

// Color is a per-instance attribute
layout(location = 2) in vec4 color;

// model_matrix will be use as a per-instance attribute
// matrix. Note that a mat4 consumes 4 consecutive locations, so
// this will actually sit in locations, 3, 4, 5, and 6.
layout(location = 3) in mat4 model_matrix;

// The view Matrix and the project matrix are constant across a draw 
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

// the output of the vertex shader (matched to fragment shader)
out VERTEX
{
	vec3 normal;
	vec4 color;
} vertex;

void main()
{
	mat4 model_view_matrix = view_matrix * model_matrix;
	gl_Position = projection_matrix * model_view_matrix * position;

	vertex.normal =  mat3(model_view_matrix) * normal;
	vertex.color = color;
}

