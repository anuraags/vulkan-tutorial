#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

class VulkanApplication {
private:
	VkApplicationInfo m_appInfo;

public:
	VulkanApplication(const char* name)
		: m_appInfo({})
	{
		m_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		m_appInfo.pApplicationName = name;
		m_appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		m_appInfo.pEngineName = "No Engine";
		m_appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		m_appInfo.apiVersion = VK_API_VERSION_1_0;
	}

};