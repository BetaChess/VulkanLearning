#ifndef PHM_POINTLIGHTCOMPONENT_H
#define PHM_POINTLIGHTCOMPONENT_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "phm_component.h"

namespace phm
{
	namespace ecs
	{
		class PointlightComponent : public Component
		{
		public:
			PointlightComponent() {};
			PointlightComponent(glm::vec3 color) : color_(color, 1.0f) {};
			PointlightComponent(glm::vec3 color, float intensity, float radius = 0.1f) : color_(color, intensity), radius_(radius) {};

			inline void setColor(glm::vec3 color) { color_ = glm::vec4(color, color_.w); };
			inline void setColor(glm::vec4 colorIntensity) { color_ = colorIntensity; };
			inline void setIntensity(float intensity) { color_.w = intensity; };
			inline void setRadius(float radius) { radius_ = radius; };

			[[nodiscard]] glm::vec3 getColor() const { return glm::vec3(color_.x, color_.y, color_.z); };
			[[nodiscard]] glm::vec4 getColorIntensity() const { return color_; };
			[[nodiscard]] float getIntensity() const { return color_.w; };
			[[nodiscard]] float getRadius() const { return radius_; };

		private:
			glm::vec4 color_ = { 1.0f, 1.0f, 1.0f, 1.0f }; // w is intensity
			float radius_ = 0.1f;
		};
	}
}


#endif /* PHM_POINTLIGHTCOMPONENT_H */
