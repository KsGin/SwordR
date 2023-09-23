#pragma once

#include <chrono>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <GLFW/glfw3.h>

namespace SwordR
{
	class Device
	{
        friend class GraphicsPipeline;
        friend class ComputePipeline;
        friend class Texture;
        friend class Camera;
        friend class Model;
        friend class ParticleSystem;

	public:
        void draw(Model* model, GraphicsPipeline* pipeline);
        void draw(ParticleSystem* particleSystem, GraphicsPipeline* pipeline);
        void dispatch(ParticleSystem* particleSystem, ComputePipeline* pipeline);
        bool createWithWindow(GLFWwindow* window, int width, int height);
        void destroy();
        void beginFrame();
        void endFrame();
        void waitFenceAndReset();


	private:
        VkShaderModule createShaderModule(const std::vector<char>& code);
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        std::vector<const char*> deviceExtensions = {
            "VK_KHR_swapchain",
        };

        std::vector<const char*> ValidationLayers = {};
        bool enableValidationLayers = true;
        inline bool checkValidationLayerSupport();

		std::vector<const char*> instanceExtensions;

        VkInstance instance = nullptr;
        VkSurfaceKHR surface = nullptr;
        VkSwapchainKHR swapChain = nullptr;
        VkDevice logicalDevice = nullptr;
        VkPhysicalDevice physicalDevice = nullptr;
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkCommandPool commandPool;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkQueue computeQueue;
        VkExtent2D swapChainExtent;

        VkRenderPass renderPass;
        VkCommandBuffer graphicsCMD;
        VkCommandBuffer computeCMD;

        std::vector<VkImageView> swapChainImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkFramebuffer> swapChainFramebuffers;

        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;

        VkFence computeInFlightFence;
        VkSemaphore computeFinishedSemaphore;

        uint32_t imageIndex;
        const int MAX_FRAMES_IN_FLIGHT = 2;

        struct UniformBufferPreFrame {
            float time;
            float unused;
            float width;
            float height;
        };

        UniformBufferPreFrame ubo{};

        std::vector<VkBuffer> modelUniformBuffers;
        std::vector<VkDeviceMemory> modelUniformBuffersMemory;
        std::vector<void*> modelUniformBuffersMapped;

        std::vector<VkBuffer> deviceUniformBuffers;
        std::vector<VkDeviceMemory> deviceUniformBuffersMemory;
        std::vector<void*> deviceUniformBuffersMapped;

        std::chrono::time_point<std::chrono::steady_clock> startTime;
        float timeSinceStartup = 0;
        float deltaTime = 0;
	};
}

