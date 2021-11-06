#ifndef PHM_CAMERA_H
#define PHM_CAMERA_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace phm
{
	class PhmCamera
	{

	public:

		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		inline const glm::mat4& getProjection() const { return projectionMatrix_; };

	private:
		glm::mat4 projectionMatrix_{ 1.0f };
	};
}


#endif /* PHM_CAMERA_H */
