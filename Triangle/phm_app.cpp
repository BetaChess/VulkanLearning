#include "phm_app.h"

namespace phm
{
	void Application::run()
	{
		while (!m_window.shouldClose())
		{
			glfwPollEvents();
		}
	}


}


