#version 450

<<<<<<< HEAD
layout (location = 0) in vec3 fragColor;

layout (location = 0) out vec4 outColor;


layout(push_constant) uniform Push
{
	mat4 transform;
=======
layout (location = 0) out vec4 outColor;

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
	outColor = vec4(fragColor, 1.0); 
=======
	outColor = vec4(push.color, 1.0); 
>>>>>>> 36771473584509ef4e0550e17a1c205503f0cd01
}

