#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <fstream>
#include <GLFW/glfw3.h>

namespace VKDemo
{
	class VKManager
	{
	public:
		bool CreateWithGLFWWindow(GLFWwindow* window, int width, int height)
		{
            uint32_t extensionCount = 0;
            const char** extensionBuffer = glfwGetRequiredInstanceExtensions(&extensionCount);
            for (uint32_t i = 0; i < extensionCount; ++i) {
                instanceExtensions.push_back(extensionBuffer[i]);
            }

            enableValidationLayers = true;
            if (checkValidationLayerSupport()) {
                ValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
            } else {
                enableValidationLayers = false;
            }

            VkApplicationInfo applicationInfo{};
            applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 2);
            applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
            applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);

            VkInstanceCreateInfo instanceCreateInfo{};
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pApplicationInfo = &applicationInfo;
            instanceCreateInfo.enabledLayerCount = enableValidationLayers ? static_cast<uint32_t>(ValidationLayers.size()) : 0;
            instanceCreateInfo.ppEnabledLayerNames = enableValidationLayers ? ValidationLayers.data() : nullptr;
            instanceCreateInfo.enabledExtensionCount = instanceExtensions.size();
            instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
            vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

            uint32_t gpuCount;
            vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
            std::vector<VkPhysicalDevice> GPUs(gpuCount);
            vkEnumeratePhysicalDevices(instance, &gpuCount, GPUs.data());
            VkPhysicalDevice physicalDevice = GPUs[0];

            uint32_t queueFamilyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> familyProperties(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, familyProperties.data());
          
            uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
            uint32_t presentQueueFamilyIndex = UINT32_MAX;
            for (uint32_t i = 0; i < queueFamilyCount; ++i) {
                if (familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    graphicsQueueFamilyIndex = i;
                }
            }

            const float priorities[]{ 1.0f };
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
            queueCreateInfo.pQueuePriorities = priorities;

            VkDeviceCreateInfo deviceCreateInfo{};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.queueCreateInfoCount = 1;
            deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
            deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
            deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
            deviceCreateInfo.enabledLayerCount = enableValidationLayers ? static_cast<uint32_t>(ValidationLayers.size()) : 0;
            deviceCreateInfo.ppEnabledLayerNames = enableValidationLayers ? ValidationLayers.data() : nullptr;
            vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);

            if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
                throw std::runtime_error("glfwCreateWindowSurface failed");
                return false;
            }
           
            VkBool32* pSupportsPresent = new VkBool32[queueFamilyCount];
            for (uint32_t i = 0; i < queueFamilyCount; i++) {
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &pSupportsPresent[i]);
            }

            if (presentQueueFamilyIndex == UINT32_MAX) {
                for (size_t i = 0; i < queueFamilyCount; ++i) {
                    if (pSupportsPresent[i] == VK_TRUE) {
                        presentQueueFamilyIndex = i;
                        break;
                    }
                }
            }
            delete[] pSupportsPresent;

            if (graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) {
                throw std::runtime_error("graphicsQueueFamilyIndex || presentQueueFamilyIndex ERROR");
                return false;
            }

            VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
            VkSurfaceCapabilitiesKHR surfCapabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCapabilities);
            VkExtent2D swapChainExtent{ (uint32_t) width, (uint32_t) height };
            VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
            swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapChainCreateInfo.pNext = nullptr;
            swapChainCreateInfo.surface = surface;
            swapChainCreateInfo.minImageCount = surfCapabilities.minImageCount;
            swapChainCreateInfo.imageFormat = format;
            swapChainCreateInfo.imageExtent.width = swapChainExtent.width;
            swapChainCreateInfo.imageExtent.height = swapChainExtent.height;
            swapChainCreateInfo.preTransform = surfCapabilities.currentTransform;
            swapChainCreateInfo.compositeAlpha = compositeAlpha;
            swapChainCreateInfo.imageArrayLayers = 1;
            swapChainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
            swapChainCreateInfo.oldSwapchain = nullptr;
            swapChainCreateInfo.clipped = true;
            swapChainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
            swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapChainCreateInfo.queueFamilyIndexCount = 0;
            swapChainCreateInfo.pQueueFamilyIndices = nullptr;
            uint32_t queueFamilyIndices[2] = { (uint32_t)graphicsQueueFamilyIndex, (uint32_t)graphicsQueueFamilyIndex };
            if (graphicsQueueFamilyIndex != presentQueueFamilyIndex) {
                swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                swapChainCreateInfo.queueFamilyIndexCount = 2;
                swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
            }

            if (vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS)
            {
                throw std::runtime_error("vkCreateSwapChainKHR failed");
                return false;
            }

            uint32_t imageCount = std::min(surfCapabilities.minImageCount + 1, surfCapabilities.maxImageCount);
            vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
            swapChainImages.resize(imageCount);
            vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

            swapChainImageViews.resize(swapChainImages.size());
            for (size_t i = 0; i < swapChainImages.size(); i++) {
                VkImageViewCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                createInfo.image = swapChainImages[i];
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.format = format;
                createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                createInfo.subresourceRange.baseMipLevel = 0;
                createInfo.subresourceRange.levelCount = 1;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.layerCount = 1;
                if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create image views!");
                    return false;
                }
            }

            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = format;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = 1;
            renderPassInfo.pAttachments = &colorAttachment;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;

            if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
                throw std::runtime_error("failed to create render pass!");
            }

            auto vertShaderCode = readFile("C:\\Users\\imqqy\\Documents\\WorkSpace\\VukanDemo\\x64\\Debug\\vs.spv");
            auto fragShaderCode = readFile("C:\\Users\\imqqy\\Documents\\WorkSpace\\VukanDemo\\x64\\Debug\\fs.spv");
            VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
            VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
            vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertShaderModule;
            vertShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
            fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragShaderModule;
            fragShaderStageInfo.pName = "main";

            VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

            std::vector<VkDynamicState> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
            };

            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)swapChainExtent.width;
            viewport.height = (float)swapChainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = swapChainExtent;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

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
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional

            VkPipelineColorBlendAttachmentState colorBlendAttachment{};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.blendEnable = VK_TRUE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 0; // Optional
            pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

            if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = nullptr; // Optional
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional

            if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
                throw std::runtime_error("failed to create graphics pipeline!");
            }

            vkDestroyShaderModule(device, fragShaderModule, nullptr);
            vkDestroyShaderModule(device, vertShaderModule, nullptr);

            return true;
		}

        void Destroy() {

            vkDestroyPipeline(device, graphicsPipeline, nullptr);

            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            vkDestroyRenderPass(device, renderPass, nullptr);

            for (auto imageView : swapChainImageViews)
            {
                vkDestroyImageView(device, imageView, nullptr);
            }

            vkDestroySwapchainKHR(device, swapChain, nullptr);
            vkDestroySurfaceKHR(instance, surface, nullptr);
            vkDestroyDevice(device, nullptr);
            vkDestroyInstance(instance, nullptr);
        }

	private:

        VkShaderModule createShaderModule(const std::vector<char>& code) {
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader module!");
            }

            return shaderModule;
        }



		static std::vector<char> readFile(const std::string& filename) {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if (!file.is_open()) {
                throw std::runtime_error("failed to open file!");
            }

            size_t fileSize = (size_t)file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();
            return buffer;
        } 

        std::vector<const char*> deviceExtensions = {
            "VK_KHR_swapchain",
        };

        std::vector<const char*> ValidationLayers = {};
        bool enableValidationLayers = true;
        bool checkValidationLayerSupport() {
            uint32_t layerCount;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            std::vector<VkLayerProperties> availableLayers(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

            for (const auto& layerProperties : availableLayers) {
                if (strcmp("VK_LAYER_KHRONOS_validation", layerProperties.layerName) == 0) {
                    return true;
                }
            }

            return false;
        }

		std::vector<const char*> instanceExtensions;

        VkDevice device = nullptr;
        VkInstance instance = nullptr;
        VkSurfaceKHR surface = nullptr;
        VkSwapchainKHR swapChain = nullptr;
        VkPipeline graphicsPipeline = nullptr;

        VkRenderPass renderPass;
        VkPipelineLayout pipelineLayout = nullptr;

        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkImage> swapChainImages;
	};
}

