#pragma once
#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class GraphicsPipeline
{
public:
    GraphicsPipeline(const VkDevice& device)
        : m_device(device)
    {
    }
    void init(uint32_t width, uint32_t height);
    void cleanup();

private:
    VkShaderModule createShaderModule(const std::vector<char>& code);
    const VkDevice& m_device;
    VkPipelineLayout m_pipelineLayout;
};
