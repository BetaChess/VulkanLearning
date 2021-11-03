#version 450

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 color;

layout(push_constant) uniform Push
{
	mat4 transform;
	vec3 color;
} push;

void main()
{
	vec4 pos = push.transform * position;
	gl_Position = vec4(pos[0], pos[1], 0.0, 1.0);
}

