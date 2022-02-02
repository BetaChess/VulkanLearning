#ifndef PHM_KEYBOARDCONTROLLER_H
#define PHM_KEYBOARDCONTROLLER_H

#include "phm_object.h"
#include "phm_window.h"


namespace phm
{
	class PhmKeyboardController
	{
	public:
		struct KeyMappings
		{
			int changeCamera = GLFW_KEY_C;
			int changeProjectionView = GLFW_KEY_V;

			int moveLeft = GLFW_KEY_A;
			int moveRight = GLFW_KEY_D;
			int moveForward = GLFW_KEY_W;
			int moveBackward = GLFW_KEY_S;
			int moveUp = GLFW_KEY_SPACE;
			int moveDown = GLFW_KEY_LEFT_SHIFT;
			int lookRight = GLFW_KEY_RIGHT;
			int lookLeft = GLFW_KEY_LEFT;
			int lookUp = GLFW_KEY_UP;
			int lookDown = GLFW_KEY_DOWN;
		};

		void moveInPlaneXZ(GLFWwindow* window, float dt, PhmObject& object) const;
		bool shouldChangeCamera(GLFWwindow* window) const;
		bool shouldChangeProjectionView(GLFWwindow* window) const;

		KeyMappings keys{};
		float moveSpeed{ 3.0f };
		float lookSpeed{ 1.5f };
	};
}



#endif /* PHM_KEYBOARDCONTROLLER_H */
