#ifndef PHM_CAMERA_H
#define PHM_CAMERA_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace phm
{
	class Camera
	{

	public:

		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
		void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

		inline const glm::mat4& getProjection() const { return projectionMatrix_; };
		inline const glm::mat4& getView() const { return viewMatrix_; };

	private:
		glm::mat4 projectionMatrix_{ 1.0f };
		glm::mat4 viewMatrix_{ 1.0f };
	};
}


#endif /* PHM_CAMERA_H */
