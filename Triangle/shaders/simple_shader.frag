#version 450

layout(location = 0) in vec3 fragCol;

layout (location = 0) out vec4 outColor;


void main()
{
	outColor = vec4(fragCol, 1.0); 
}

