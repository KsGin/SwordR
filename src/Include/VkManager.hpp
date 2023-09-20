#pragma once

#include <vulkan/vulkan.h>
#include <vector>
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
            VkExtent2D swapChainExtend{ (uint32_t) width, (uint32_t) height };
            VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
            swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapChainCreateInfo.pNext = nullptr;
            swapChainCreateInfo.surface = surface;
            swapChainCreateInfo.minImageCount = surfCapabilities.minImageCount;
            swapChainCreateInfo.imageFormat = format;
            swapChainCreateInfo.imageExtent.width = swapChainExtend.width;
            swapChainCreateInfo.imageExtent.height = swapChainExtend.height;
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

            return true;
		}

        void Destroy() {

	        if (device)
	        {
                for (auto imageView : swapChainImageViews)
                {
                    vkDestroyImageView(device, imageView, nullptr);
                }
	        }

            if (device && swapChain)
            {
                vkDestroySwapchainKHR(device, swapChain, nullptr);
            }

            if (instance && surface)
            {
                vkDestroySurfaceKHR(instance, surface, nullptr);
            }

            if (device)
            {
                vkDestroyDevice(device, nullptr);
            }
            
            if (instance)
            {
                vkDestroyInstance(instance, nullptr);
            }
        }

	private:

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

        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkImage> swapChainImages;
	};
}

