#ifndef PHM_TRANSFORM_H
#define PHM_TRANSFORM_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace phm
{
	struct Transform
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };


		glm::mat4 mat4() const;
		glm::mat3 normalMatrix() const;

		glm::vec3 rotation{};
	};
}


#endif // PHM_TRANSFORM_H