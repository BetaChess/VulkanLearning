#ifndef PHM_OBJECT_H
#define PHM_OBJECT_H

#include <memory>

#include "phm_model.h"

struct Transform2D
{
	glm::vec2 translation{};
	glm::vec2 scale{ 1.0f, 1.0f };


	inline glm::mat4 mat4() const
	{
		glm::mat4 scaleMat
		{
			{scale.x, 0.0f, 0.0f, 0.0f},
			{0.0f, scale.y, 0.0f, 0.0f},
			{0.0f, 0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};

		glm::mat4 translateMat
		{
			{1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f, 0.0f},
			{translation.x, translation.y, 1.0f, 0.0f},
			{0.0f, 0.0f, 0.0f, 1.0f}
		};

		return translateMat * rotationMatrix_ * scaleMat;
	};

	inline void setRotation(float angle)
	{
		rotation_ = angle;

		const float s = glm::sin(angle);
		const float c = glm::cos(angle);

		rotationMatrix_ =
		{
			{c, s, 0.0f, 0.0f},			// First column (left to right is top to bottom).
			{-s, c, 0.0f, 0.0f},		// Second column
			{0.0f, 0.0f, 1.0f, 0.0f},	// Third column
			{0.0f, 0.0f, 0.0f, 1.0f}
		};
	}

	inline float rotation() { return rotation_; };

private:
	float rotation_ = 0;
	glm::mat4 rotationMatrix_ =
	{
		{glm::cos(rotation_), glm::sin(rotation_), 0.0f, 0.0f},	// First column (left to right is top to bottom).
		{-glm::sin(rotation_), glm::cos(rotation_), 0.0f, 0.0f},	// Second column
		{0.0f, 0.0f, 1.0f, 0.0f},									// Third column
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
};


namespace phm
{
	class PhmObject
	{
	public:
		std::shared_ptr<PhmModel> model{};
		glm::vec3 color{};
		Transform2D transform{};
	};
}


#endif /* PHM_OBJECT_H */