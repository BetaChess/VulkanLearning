#ifndef PHM_OBJECT_H
#define PHM_OBJECT_H

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

#include "phm_model.h"

struct Transform
{
	glm::vec3 translation{};
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };


	inline glm::mat4 mat4() const
	{ 
		auto transform = glm::translate(glm::mat4{ 1.0f }, translation);
		
		transform = glm::rotate(transform, rotation.y, { 0.0f, 1.0f, 0.0f });
		transform = glm::rotate(transform, rotation.x, { 1.0f, 0.0f, 0.0f });
		transform = glm::rotate(transform, rotation.z, { 0.0f, 0.0f, 1.0f });

		transform = glm::scale(transform, scale);
		
		return transform;
	};

	glm::vec3 rotation{};
};

namespace phm
{
	class PhmObject
	{
	public:
		std::shared_ptr<PhmModel> model{};
		glm::vec3 color{};
		Transform transform{};

	};
}


#endif /* PHM_OBJECT_H */