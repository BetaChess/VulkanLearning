#version 450

const vec2 OFFSETS[6] = vec2[](
	vec2(-1.0, -1.0),
	vec2(-1.0, 1.0),
	vec2(1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(-1.0, 1.0),
	vec2(1.0, 1.0)
);

layout (location = 0) out vec2 fragOffset;
layout (location = 1) out flat int lightIndex;

struct PointLight
{
	vec4 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo 
{
	mat4 projection;
	mat4 view;
	vec4 ambientLightColor;
	PointLight pointLights[20];
	int numPointLights;
} ubo;

void main()
{
	int index = int(floor(gl_VertexIndex/6));
	PointLight light = ubo.pointLights[index];

	fragOffset = OFFSETS[gl_VertexIndex % 6];
	vec3 cameraRightWorld = {ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]};
	vec3 cameraUpWorld = {ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]};

	vec3 positionWorld = light.position.xyz 
		+ light.position.w * fragOffset.x * cameraRightWorld
		+ light.position.w * fragOffset.y * cameraUpWorld;

	gl_Position = ubo.projection * ubo.view * vec4(positionWorld, 1.0);
	lightIndex = index;
}
