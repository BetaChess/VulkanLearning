#include "pch.h"

#include "phm_device.h"

#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>


namespace phm
{

	// local callback functions
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{

		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			printWColor("\nvalidation layer !!!ERROR!!!: " << pCallbackData->pMessage, ERRORCOL);
		}
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			printWColor("\nvalidation layer WARNING: " << pCallbackData->pMessage, WARNCOL);
		}
		else
		{
#ifdef DEBUGWITHINFO
			printWColor("validation layer INFO: " << pCallbackData->pMessage, INFOCOL);
#endif
		}

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance_,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance_,
			"vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			return func(instance_, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(
		VkInstance instance_,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance_,
			"vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			func(instance_, debugMessenger, pAllocator);
		}

	}

	// class member functions

	PhmDevice::PhmDevice(PhmWindow& window)
		: window_{ window }
	{
		// Names of the methods should be explaination enough.
		DebugPrint("Starting Instance Creation");
		createInstance();
		DebugPrint("Setting up debug messenger");
		setupDebugMessenger();
		DebugPrint("Creating surface");
		createSurface();
		DebugPrint("Picking physical device");
		pickPhysicalDevice();
		DebugPrint("Creating logical device");
		createLogicalDevice();
		DebugPrint("Creating command pool");
		createCommandPool();
	}

	PhmDevice::~PhmDevice()
	{
		// Do the cleanup in the right order.

		vkDestroyCommandPool(device_, commandPool_, nullptr);
		vkDestroyDevice(device_, nullptr);

		// The DebugMessenger should only be destroyed if it was created in the first place.
		if (enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
		}

		vkDestroySurfaceKHR(instance_, surface_, nullptr);
		vkDestroyInstance(instance_, nullptr);
	}

	/// <summary>
	/// Method for creating the device instance.
	/// </summary>
	void PhmDevice::createInstance()
	{
		DebugPrint("Checking validation layer support");
		// Check if the validation layers are enabled and supported.
		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			// If they are enabled but not supported, throw a runtime error.
			throw std::runtime_error("Validation layers requested, but not available! ");
		}

		// Create the appinfo struct.
		VkApplicationInfo appInfo{};
		// Specify the struct as a type application info. (a struct used to give information about the instance).
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		// Set the name
		// TODO: Change this to take the actual name
		appInfo.pApplicationName = "Triangle";
		// Specify the application version
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		// A custom engine is used. 
		// TODO: give the engine a name at some point (maybe).
		appInfo.pEngineName = "No engine";
		// Specify the engine version
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		// Specift the vulkan API version.
		appInfo.apiVersion = VK_API_VERSION_1_0;

		// Create the instance_ create info
		VkInstanceCreateInfo createInfo{};
		// Specify that this struct is the info to create the instance.
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		// give it the application info created earlier.
		createInfo.pApplicationInfo = &appInfo;

		// Get and enable the glfw extensions
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());		// The number of enabled extensions
		createInfo.ppEnabledExtensionNames = extensions.data();								// The names of the actual extensions

		// create a temporary debugger for creation of the Vulkan instance_
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		// check if validation layers are enabled.
		if (enableValidationLayers)
		{
			// Specify the enabled validation layers
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());	// The number of validation layers
			createInfo.ppEnabledLayerNames = validationLayers.data();						// The names of the validation layers

			// Use a helper function to populate the createInfo for the debugMessenger.
			populateDebugMessengerCreateInfo(debugCreateInfo);
			// Give the struct the creation info.
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			// Specify that no layers are enabled (0)
			createInfo.enabledLayerCount = 0;

			// Give it a nullptr to the creation info.
			createInfo.pNext = nullptr;
		}

		// As something extra: Check which extensions are suported.
		// This is done in "hasGflwRequiredInstanceExtensions"
		/*{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> extensions(extensionCount);

			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			std::cout << "available extensions:\n";

			for (const auto& extension : extensions) {
				std::cout << '\t' << extension.extensionName << '\n';
			}
		}*/


		// Finally, create the instance_
		if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS)
		{
			// If the instance failed to be created, throw a runtime error.
			throw std::runtime_error("failed to create instance_!");
		}

		// TODO: WHAT DOES THIS DO?
		hasGflwRequiredInstanceExtensions();
	}

	/// <summary>
	/// Method responsible for picking the physical device (GPU).
	/// </summary>
	void PhmDevice::pickPhysicalDevice()
	{
		// Find the number of physical devices that support vulkan.
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

		// Check if any of the graphics cards support vulkan.
		if (deviceCount == 0)
		{
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");
		}

		// Store the physical handles in an array
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());


		// Use an ordered map to automatically sort candidates by increasing score
		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			int score = rateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		// Print the name of the chosen GPU
		{
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(candidates.rbegin()->second, &deviceProperties);

			std::cout << "Best GPU found is: " << deviceProperties.deviceName << std::endl;
		}

		// Check if the best candidate is suitable at all
		if (candidates.rbegin()->first > 0)
		{
			physicalDevice_ = candidates.rbegin()->second;
		}
		else
		{
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	/// <summary>
	/// Method for creating the logical device (the device that interfaces with the physical device).
	/// </summary>
	void PhmDevice::createLogicalDevice()
	{
		// Get the queue family indices
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice_);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			// Set the queue priority
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Declare used device features
		VkPhysicalDeviceFeatures deviceFeatures{};

		// Declare the locial device create info
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		// Give it the queue create infos
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		// Give it the device feature information
		createInfo.pEnabledFeatures = &deviceFeatures;
		// Tell it how many extensions are enabled
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		// Tell it which extensions are enabled
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// Enable seperate validation layers
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		// Try to create the logical device
		if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		// Set the graphics queue
		vkGetDeviceQueue(device_, indices.graphicsFamily.value(), 0, &graphicsQueue_);
		// Set the presentation queue
		vkGetDeviceQueue(device_, indices.presentFamily.value(), 0, &presentQueue_);
	}

	/// <summary>
	/// Method for creating the command pool.
	/// </summary>
	void PhmDevice::createCommandPool()
	{
		// Get the supported queue families from the GPU.
		QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

		// Create the struct for creating the command pool
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		// Get the index for the graphics command pool.
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		poolInfo.flags =
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
			VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		// Try to create the command pool.
		if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}

	/// <summary>
	/// Method for initiating the surface member. Uses the window and instance members.
	/// </summary>
	void PhmDevice::createSurface() { window_.createWindowSurface(instance_, &surface_); }

	/// <summary>
	/// Checks if a physical device (GPU) is suitable for use.
	/// </summary>
	/// <param name="device">: The physical device (GPU) to check.</param>
	/// <returns>True if the device is suitable. False otherwise. </returns>
	bool PhmDevice::isDeviceSuitable(VkPhysicalDevice device)
	{
		// Get the device properties
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		// Get the device features
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		// Get the device queue families
		QueueFamilyIndices indices = findQueueFamilies(device);

		// Check for extension support
		bool extensionsSupported = checkDeviceExtensionSupport(device);

		// Check for swap chain adequacy
		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	/// <summary>
	/// Just a helper function for populating the debug messenger creation info.
	/// </summary>
	/// <param name="createInfo">: The createinfo to be populated. </param>
	void PhmDevice::populateDebugMessengerCreateInfo(
		VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr;  // Optional
	}

	/// <summary>
	/// Methods to set up the debug messenger. Automatically returns if validation layers are disabled.
	/// </summary>
	void PhmDevice::setupDebugMessenger()
	{
		// Return if the debug messenger isn't nececary (if validation layers are not enabled).
		if (!enableValidationLayers) return;

		// Create the debug messenger create info.
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		// Try to create the debug messenger. Throw a runtime error if it failed.
		if (CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugMessenger_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	/// <summary>
	/// Methods to check for validation layer support
	/// </summary>
	/// <returns>True if the nececary validation layers are supported. False otherwise. </returns>
	bool PhmDevice::checkValidationLayerSupport()
	{
		// Get the validation layer count
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		// Create a vector to store the available layers
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// print the available layers
		std::cout << "available layers:\n";

		for (const auto& layer : availableLayers)
			std::cout << '\t' << layer.layerName << '\n';

		// Iterate through each layername in the list of required layers
		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			// Check if the layer is supported by iterating through the supported layers
			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			// If any layer is not found. Return false.
			if (!layerFound)
			{
				DebugPrint("Layer was not found during Validation Layer Support checking! Layer name is: " << layerName);
				return false;
			}
		}

		return true;
	}

	/// <summary>
	/// Method for getting the extensions required by the application.
	/// </summary>
	/// <returns>A vector of all of the required extensions. </returns>
	std::vector<const char*> PhmDevice::getRequiredExtensions()
	{
		// Get the number of extensions required by glfw.
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		// Put them in the vector of required extensions
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		// If validation layers are enabled. Throw in the Debug Utils extension.
		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		// Return the vector.
		return extensions;
	}

	/// <summary>
	/// Check if all of the required extensions are supported. Throws a runtime error if they are not.
	/// </summary>
	void PhmDevice::hasGflwRequiredInstanceExtensions()
	{
		// Create a variable to store the number of supported extensions
		uint32_t extensionCount = 0;

		// Get the number of avaliable extensions. (The middle parameter of the function writes this to the specified memory location)
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		// Change the size of the extensions vetor to match the number of avaliable extensions.
		std::vector<VkExtensionProperties> extensions(extensionCount);
		// Write the extensions to the vector. 
		// (the last parameter is a memory location to the place the extension properties should be written, 
		//		in this case, the data location of the vector)
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// List out the avaliable extensions and put the names in a string data structure.
		std::cout << "available extensions:" << std::endl;
		std::unordered_set<std::string> available;
		for (const auto& extension : extensions)
		{
			std::cout << "\t" << extension.extensionName << std::endl;
			available.insert(extension.extensionName);
		}

		// List out the required extensions.
		std::cout << "required extensions:" << std::endl;
		// Get the required extensions.
		auto requiredExtensions = getRequiredExtensions();
		// Iterate through the required extensions.
		for (const auto& required : requiredExtensions)
		{
			std::cout << "\t" << required << std::endl;
			// If the required extension is not avaliable, throw a runtime error.
			if (available.find(required) == available.end())
			{
				throw std::runtime_error("Missing required glfw extension");
			}
		}
	}

	/// <summary>
	/// Method for checking if the physical device supports the required extensions.
	/// </summary>
	/// <param name="device">: The physical device to check. </param>
	/// <returns>True if the device supports all of the extendsions. False otherwise. </returns>
	bool PhmDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		// Get the extension count
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		// Get the avaliable extensions
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		// Make a string set of the required extensions
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		// Erase any supported extensions from the set
		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		// Check if the set is empty. If it is, all of the extensions are supported.
		return requiredExtensions.empty();
	}

	/// <summary>
	/// Find the queue families supported by a device.
	/// </summary>
	/// <param name="device">: The physical device (GPU) to check.</param>
	/// <returns>QueueFamilyIndices</returns>
	QueueFamilyIndices PhmDevice::findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;
		// Logic to find queue family indices to populate struct with
		// Queue family count
		uint32_t queueFamilyCount = 0;
		// Get the number of queue families.
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		// Get queue family properties
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		// Find a queue family that supports "VK_QUEUE_GRAPHICS_BIT"
		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			VkBool32 presentSupport = false;
			// Get the surface support of the device
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}
			if (presentSupport)
			{
				indices.presentFamily = i;
			}
			if (indices.isComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}

	/// <summary>
	/// Method for getting the swap chain support details based on the given physical device.
	/// </summary>
	/// <param name="device">: The physical device. </param>
	/// <returns>The swapchain support details (SwapChainSupportDetails). </returns>
	SwapChainSupportDetails PhmDevice::querySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		// Get the surface capabilities of the physical device
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

		// Get the format count
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

		// if the format count isn't 0, resize the format list and insert the format details.
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
		}

		// Get the present mode count
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

		// if the present mode count isn't 0, resize the mode list and populate it.
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	VkFormat PhmDevice::findSupportedFormat(
		const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		// TODO: ADD COMMENTS
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice_, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (
				tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}
		throw std::runtime_error("failed to find supported format!");
	}

	/// <summary>
	///	Retrieves the index to a suitable memory type based on the given filters/flags.
	/// </summary>
	/// <param name="typeFilter">: Typefilter</param>
	/// <param name="properties">: A bitmask of the required properties. </param>
	/// <returns>The index to the suitable memory. </returns>
	uint32_t PhmDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		// Get the memory properties of the physical device
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memProperties);

		// Iterate through the memory types retrieved
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			// If the typefilter matches and the memory type has the correct properties (set by flags), return the index to the memory type.
			if (
				(typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties
				)
			{
				return i;
			}
		}

		// If the correct memory type was not found. Throw a runtime error.
		throw std::runtime_error("failed to find suitable memory type!");
	}

	/// <summary>
	/// Method for creating a buffer and binding it to memory visible to the physical device. 
	/// </summary>
	/// <param name="size">: Size of the buffer to be made. </param>
	/// <param name="usage">: Usage bitmask of the buffer. </param>
	/// <param name="properties">: The required properties of the memory visible to the physical device. </param>
	/// <param name="buffer">: The buffer reference that will be written to. </param>
	/// <param name="bufferMemory">: The buffer device memory that will be written to. </param>
	void PhmDevice::createBuffer(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory)
	{
		// Create the bufferinfo struct and populate it.
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		// Access to any range or image subresource of the object will be exclusive to a single queue family at a time
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// Attempt to create the buffer.
		if (vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}

		// Allocate memory on the physical device and bind the buffer to the memory 
		// (The buffer is CPU side, so the memory has to be synced to the GPU visible memory)
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

		// Create the allocation info struct.
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		// Attempt to allocate memory on the physical device.
		if (vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		// Bind the created buffer to the allocated memory without any offset. Memory management for creating offsets will be added later.
		vkBindBufferMemory(device_, buffer, bufferMemory, 0);
	}

	/// <summary>
	/// Creates a single use command buffer.
	/// </summary>
	/// <returns>The created command buffer. </returns>
	VkCommandBuffer PhmDevice::beginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool_;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	/// <summary>
	/// Submits the commands in a single use command buffer, then destroys it.
	/// </summary>
	/// <param name="commandBuffer">: The single use command buffer to be destroyed. </param>
	void PhmDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		// Submit the command buffer.
		vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue_);

		// Then destroy the command buffer right after.
		vkFreeCommandBuffers(device_, commandPool_, 1, &commandBuffer);
	}

	/// <summary>
	/// Method for copying the contents of one buffer to another buffer. 
	/// </summary>
	/// <param name="srcBuffer">: The source buffer. </param>
	/// <param name="dstBuffer">: The destination buffer. </param>
	/// <param name="size">: The size of the region to be copied. </param>
	void PhmDevice::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		// Create a single use command to copy the contents
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		// Create the copy buffer info and populate it.
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;  // Optional
		copyRegion.dstOffset = 0;  // Optional
		copyRegion.size = size;
		// Submit the command to copy the buffer to.
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		// Submit and destroy the command buffer.
		endSingleTimeCommands(commandBuffer);
	}

	/// <summary>
	/// Method for copying a VkBuffer to a VkImage.
	/// </summary>
	/// <param name="buffer">: The source buffer. </param>
	/// <param name="image">: The image to be copied to. </param>
	/// <param name="width">: The width. </param>
	/// <param name="height">: The height. </param>
	/// <param name="layerCount">: The number of layers. </param>
	void PhmDevice::copyBufferToImage(
		VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount)
	{
		// Create a single use command buffer. 
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		// Specify the region to be copied
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		// Specify the copy mask. In this case the color should be copied. 
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		// Submit the copy command to the command buffer. 
		vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region);

		// Submit the command and destroy the single use command buffer. 
		endSingleTimeCommands(commandBuffer);
	}

	/// <summary>
	/// Method for creating a VkImage from given info.
	/// </summary>
	/// <param name="imageInfo">: The create info of the image. </param>
	/// <param name="properties">: The property bitmask for the memory used to store the image. </param>
	/// <param name="image">: The reference the VkImage will be written to. </param>
	/// <param name="imageMemory">: The reference the image memory will be written to. </param>
	void PhmDevice::createImageWithInfo(
		const VkImageCreateInfo& imageInfo,
		VkMemoryPropertyFlags properties,
		VkImage& image,
		VkDeviceMemory& imageMemory)
	{
		// First. Attemt to create the image itself. 
		if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		// Get the memory requirements.
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device_, image, &memRequirements);

		// Create the memory allocation info and populate it.
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		// Get the index of the memory.
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		// Attempt to allocate the memory.
		if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}
		// Attempt to bind the memory to the image.
		if (vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to bind image memory!");
		}
	}

	/// <summary>
	/// Helper methods for scoring a physical device based on it's suitability. 
	/// </summary>
	/// <param name="device">: The physical device to be scored. </param>
	/// <returns>The score of the device. </returns>
	int PhmDevice::rateDeviceSuitability(VkPhysicalDevice device)
	{
		// First, check if the device is suitable at all.
		if (!isDeviceSuitable(device))
			return 0;

		// Get the device properties
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		// Get the device features
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

		return score;
	}

}  // namespace phm
