#version 450

layout (location = 0) in vec2 fragOffset;
layout (location = 1) in flat int lightIndex;

layout (location = 0) out vec4 outColor;

struct PointLight
{
	vec4 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo 
{
	mat4 projection;
	mat4 view;
	mat4 inverseView;
	vec4 ambientLightColor;
	PointLight pointLights[20];
	int numPointLights;
} ubo;
void main()
{
	PointLight light = ubo.pointLights[lightIndex];
	outColor = vec4(light.color.xyz, (1 - dot(fragOffset, fragOffset)) * 1.0);
}