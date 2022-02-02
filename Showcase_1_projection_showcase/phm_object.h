#ifndef PHM_OBJECT_H
#define PHM_OBJECT_H

#include <memory>

#include <glm/gtc/matrix_transform.hpp>

#include "phm_model.h"


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

	class PhmObject
	{
	public:
		std::shared_ptr<PhmModel> model{};
		glm::vec3 color{};
		Transform transform{};

	};
}


#endif /* PHM_OBJECT_H */