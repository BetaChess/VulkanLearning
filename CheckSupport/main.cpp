#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void errorCallback(int code, const char* error)
{
	std::cout << error << '\n';
}

int main()
{
	glfwSetErrorCallback(errorCallback);

	glfwInit();

	printf("Vulkan loader: %s\n",
		   glfwVulkanSupported() ? "available" : "missing");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* window = glfwCreateWindow(200, 200, "Version", NULL, NULL);

	uint32_t loader_version = VK_API_VERSION_1_0;

	if (vkEnumerateInstanceVersion)
	{
		uint32_t version;
		if (vkEnumerateInstanceVersion(&version) == VK_SUCCESS)
			loader_version = version;
	}

	printf("Vulkan loader API version: %i.%i\n",
		   VK_VERSION_MAJOR(loader_version),
		   VK_VERSION_MINOR(loader_version));

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (int i = 0; i < glfwExtensionCount; i++)
		std::cout << glfwExtensions[i] << '\n';

	return 0;
}

