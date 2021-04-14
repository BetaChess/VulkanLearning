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
		inline bool shouldClose() { return glfwWindowShouldClose(m_window); };

	private:
		// Private members
		GLFWwindow* m_window;
		size_t m_width, m_height;
		std::string m_windowName;

		// Private methods
		void initWindow();

	};
}


#endif /* PHM_WINDOW_H */