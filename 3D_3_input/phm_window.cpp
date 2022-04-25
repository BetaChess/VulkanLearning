
#include "phm_window.h"

#include <stdexcept>

namespace phm
{
	Window::Window(size_t w, size_t h, std::string name)
		: width_(w), height_(h), windowName_(name)
	{
		initWindow();
	}

	Window::~Window()
	{
		// Destroy the window instance
		glfwDestroyWindow(window_);
		// Terminate GLFW
		glfwTerminate();

	}

	void Window::initWindow()
	{
		glfwInit();
		// We're not using openGL, so disable it.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		
		// Make the window resizable.
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		window_ = glfwCreateWindow(static_cast<int>(width_), static_cast<int>(height_), windowName_.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window_, this);
		glfwSetFramebufferSizeCallback(window_, framebufferResizeCallback);
	}

	void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface! ");
		}
	}

	/// <summary>
	/// This function is called (by GLFW) anytime the window is resized.
	/// </summary>
	/// <param name="width">The new width of the window</param>
	/// <param name="height">The new height of the window</param>
	void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto phmWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

		phmWindow->frameBufferResized_ = true;
		phmWindow->width_ = width;
		phmWindow->height_ = height;
	}


} // namespace phm

