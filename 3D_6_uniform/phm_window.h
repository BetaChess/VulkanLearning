#ifndef PHM_WINDOW_H
#define PHM_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>


namespace phm
{
	class PhmWindow
	{
	public:
		// Constructor(s)
		PhmWindow(size_t w, size_t h, std::string name);

		// Delete copy constructor and assignment operator.
		PhmWindow(const PhmWindow&) = delete;
		PhmWindow& operator=(const PhmWindow&) = delete;

		// Destructor
		~PhmWindow();

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