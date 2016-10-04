#version 430

in vec3 vertexColor;
out vec4 fragmentColor;


void main()
{
	fragmentColor = vec4(vertexColor,1.0);
}