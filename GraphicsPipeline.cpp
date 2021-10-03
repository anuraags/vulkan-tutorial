#include "GraphicsPipeline.h"

#include <fstream>

static std::vector<char> readFile(const std::string& filename)
{
    // The advantage of starting to read at the end of the file (std::ios::ate)
    // is that we can use the read position to determine the size of the file and
    // allocate a buffer :
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

void GraphicsPipeline::init(uint32_t width, uint32_t height)
{
    auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");
    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    // This tells Vulkan in which pipeline stage the shader is going to be used
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    // The VkPipelineVertexInputStateCreateInfo structure describes the format of the vertex data that will be passed to the vertex shader.
    // It describes this in roughly two ways:
    // Bindings: spacing between data and whether the data is per-vertex or per-instance (see instancing)
    // Attribute descriptions: type of the attributes passed to the vertex shader, which binding to load them from and at which offset
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

    // The VkPipelineInputAssemblyStateCreateInfo struct describes two things:
    // what kind of geometry will be drawn from the vertices and
    // if primitive restart should be enabled.
    // The former is specified in the topology member and can have values like:
    //  VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
    //  VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
    //  VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is used as start vertex for the next line
    //  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 vertices without reuse
    //  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of every triangle are used as first two vertices of the next triangle
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // A viewport basically describes the region of the framebuffer that the output will be rendered to.
    // This will almost always be (0, 0) to (width, height)
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;


    // While viewports define the transformation from the image to the framebuffer, scissor rectangles define in which
    // regions pixels will actually be stored. Any pixels outside the scissor rectangles will be discarded by the
    // rasterizer. They function like a filter or a mask rather than a transformation.
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent.width = width;
    scissor.extent.height = height;

    // Now this viewport and scissor rectangle need to be combined into a viewport state using the
    // VkPipelineViewportStateCreateInfo struct. It is possible to use multiple viewports and scissor rectangles
    // on some graphics cards, so its members reference an array of them. Using multiple requires enabling a GPU feature
    // (see logical device creation).
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;


    // The rasterizer takes the geometry that is shaped by the vertices from the vertex shader and turns it into
    // fragments to be colored by the fragment shader.
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;          // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

    // The VkPipelineMultisampleStateCreateInfo struct configures multisampling, which is one of the ways to perform
    // anti-aliasing. It works by combining the fragment shader results of multiple polygons that rasterize to the same
    // pixel. This mainly occurs along edges, which is also where the most noticeable aliasing artifacts occur.
    // Because it doesn't need to run the fragment shader multiple times if only one polygon maps to a pixel, it is
    // significantly less expensive than simply rendering to a higher resolution and then downscaling.
    // Enabling it requires enabling a GPU feature.
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional

    // After a fragment shader has returned a color, it needs to be combined with the color that is already in the framebuffer.
    // This transformation is known as color blending and there are two ways to do it:
    //      Mix the old and new value to produce a final color
    //      Combine the old and new value using a bitwise operation
    // There are two types of structs to configure color blending. The first struct, VkPipelineColorBlendAttachmentState
    // contains the configuration per attached framebuffer and the second struct, VkPipelineColorBlendStateCreateInfo
    // contains the global color blending settings.In our case we only have one framebuffer.
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;            // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
}

void GraphicsPipeline::cleanup()
{
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}
