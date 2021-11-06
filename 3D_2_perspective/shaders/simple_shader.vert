#version 450

<<<<<<< HEAD
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 fragColor;


layout(push_constant) uniform Push
{
	mat4 transform;
=======
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(push_constant) uniform Push
{
	mat2 transform;
	vec2 offset;
>>>>>>> 36771473584509ef4e0550e17a1c205503f0cd01
	vec3 color;
} push;

void main()
{
<<<<<<< HEAD
	gl_Position = push.transform * vec4(position, 1.0);
	fragColor = color;
=======
	gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0);
>>>>>>> 36771473584509ef4e0550e17a1c205503f0cd01
}

