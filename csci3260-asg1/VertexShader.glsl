#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 initColor;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	vertexColor = initColor;
}