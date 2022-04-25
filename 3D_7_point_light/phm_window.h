#ifndef PHM_WINDOW_H
#define PHM_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>


namespace phm
{
	class Window
	{
	public:
		// Constructor(s)
		Window(size_t w, size_t h, std::string name);

		// Delete copy constructor and assignment operator.
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		// Destructor
		~Window();

		// Public methods
		inline bool shouldClose() { return glfwWindowShouldClose(window_); };
		inline VkExtent2D getExtent() { return { static_cast<uint32_t>(width_), static_cast<uint32_t>(height_) }; }
		inline bool wasWindowResized() { return frameBufferResized_; };
		inline void resetWindowResizedFlag() { frameBufferResized_ = false; };
		inline GLFWwindow* getGLFWWindow() const { return window_; };

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		// Private members
		GLFWwindow* window_;
		std::string windowName_;

		size_t width_, height_;
		bool frameBufferResized_ = false;

		// Private methods
		void initWindow();

		// Static
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
}


#endif /* PHM_WINDOW_H */