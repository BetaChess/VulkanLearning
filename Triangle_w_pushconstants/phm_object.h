#ifndef PHM_OBJECT_H
#define PHM_OBJECT_H

#include <memory>

#include "phm_model.h"

struct Transform2d
{
	glm::vec2 translation{};
	glm::vec2 scale{ 1.0f, 1.0f };


	inline glm::mat2 mat2() 
	{ 
		glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} };
		return rotationMatrix_ * scaleMat;
	};

	inline void setRotation(float angle)
	{
		rotation_ = angle;

		const float s = glm::sin(angle);
		const float c = glm::cos(angle);

		rotationMatrix_ = { {c, s}, {-s, c} };
	}

	inline float rotation() { return rotation_; };

private:
	float rotation_ = 0;
	glm::mat2 rotationMatrix_ = { {glm::cos(rotation_), glm::sin(rotation_)},  {-glm::sin(rotation_), glm::cos(rotation_)} };

};

namespace phm
{
	class PhmObject
	{
	public:
		std::shared_ptr<PhmModel> model{};
		glm::vec3 color{};
		Transform2d transform{};

	};
}


#endif /* PHM_OBJECT_H */