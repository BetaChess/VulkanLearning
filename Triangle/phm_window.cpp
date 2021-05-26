
#include "phm_window.h"

#include <stdexcept>

namespace phm
{
	PhmWindow::PhmWindow(size_t w, size_t h, std::string name)
		: m_width(w), m_height(h), m_windowName(name)
	{
		initWindow();
	}

	PhmWindow::~PhmWindow()
	{
		// Destroy the window instance
		glfwDestroyWindow(m_window);
		// Terminate GLFW
		glfwTerminate();
	}

	void PhmWindow::initWindow()
	{
		glfwInit();
		// We're not using openGL, so disable it.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// Make the window non-resizable.
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
	}

	void PhmWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface! ");
		}
	}


} // namespace phm
