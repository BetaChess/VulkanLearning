#include "phm_camera.h"

#include <cassert>
#include <limits.h>

namespace phm
{
	void PhmCamera::setOrthographicProjection(
		float left, float right, float top, float bottom, float near, float far) {
		projectionMatrix_ = glm::mat4{ 1.0f };
		projectionMatrix_[0][0] = 2.f / (right - left);
		projectionMatrix_[1][1] = 2.f / (bottom - top);
		projectionMatrix_[2][2] = 1.f / (far - near);
		projectionMatrix_[3][0] = -(right + left) / (right - left);
		projectionMatrix_[3][1] = -(bottom + top) / (bottom - top);
		projectionMatrix_[3][2] = -near / (far - near);
	}

	void PhmCamera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
		assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovy = tan(fovy / 2.f);
		projectionMatrix_ = glm::mat4{ 0.0f };
		projectionMatrix_[0][0] = 1.f / (aspect * tanHalfFovy);
		projectionMatrix_[1][1] = 1.f / (tanHalfFovy);
		projectionMatrix_[2][2] = far / (far - near);
		projectionMatrix_[2][3] = 1.f;
		projectionMatrix_[3][2] = -(far * near) / (far - near);
	}
}