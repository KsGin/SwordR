#pragma once
#include "../include/device.h"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan.hpp>

#include "../include/model.h"
#include "../include/graphicsPipeline.h"
#include "../include/particle.h"
#include "../include/computePipeline.h"

namespace SwordR {
    bool Device::createWithWindow(GLFWwindow* window, int width, int height)
    {
        uint32_t extensionCount = 0;
        const char** extensionBuffer = glfwGetRequiredInstanceExtensions(&extensionCount);
        for (uint32_t i = 0; i < extensionCount; ++i) {
            instanceExtensions.push_back(extensionBuffer[i]);
        }

        enableValidationLayers = true;
        if (checkValidationLayerSupport()) {
            ValidationLayers.push_back("VK_LAYER_KHRONOS_validation");
        }
        else {
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
        physicalDevice = GPUs[0];

        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> familyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, familyProperties.data());

        uint32_t graphicsAndComputeQueueFamilyIndex = UINT32_MAX;
        uint32_t presentQueueFamilyIndex = UINT32_MAX;
        for (uint32_t i = 0; i < queueFamilyCount; ++i) {
            // must both supports graphics and compute
            if ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
                && (familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) 
            {
                graphicsAndComputeQueueFamilyIndex = i;
            }
        }

        const float priorities[]{ 1.0f };
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.queueFamilyIndex = graphicsAndComputeQueueFamilyIndex;
        queueCreateInfo.pQueuePriorities = priorities;

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.enabledLayerCount = enableValidationLayers ? static_cast<uint32_t>(ValidationLayers.size()) : 0;
        deviceCreateInfo.ppEnabledLayerNames = enableValidationLayers ? ValidationLayers.data() : nullptr;
        vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &logicalDevice);

        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("glfwCreateWindowSurface failed");
            return false;
        }

        auto* pSupportsPresent = new VkBool32[queueFamilyCount];
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

        if (graphicsAndComputeQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) {
            throw std::runtime_error("graphicsQueueFamilyIndex || presentQueueFamilyIndex ERROR");
            return false;
        }

        // 获取graphics队列，用于基础渲染
        vkGetDeviceQueue(logicalDevice, graphicsAndComputeQueueFamilyIndex, 0, &graphicsQueue);
        // 获取compute队列，用于compute
        vkGetDeviceQueue(logicalDevice, graphicsAndComputeQueueFamilyIndex, 0, &computeQueue);
        // 获取present队列，用于上屏
        vkGetDeviceQueue(logicalDevice, presentQueueFamilyIndex, 0, &presentQueue);

        VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
        VkSurfaceCapabilitiesKHR surfCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCapabilities);
        swapChainExtent = VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
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
        uint32_t queueFamilyIndices[2] = { (uint32_t)graphicsAndComputeQueueFamilyIndex, (uint32_t)graphicsAndComputeQueueFamilyIndex };
        if (graphicsAndComputeQueueFamilyIndex != presentQueueFamilyIndex) {
            swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapChainCreateInfo.queueFamilyIndexCount = 2;
            swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        }

        if (vkCreateSwapchainKHR(logicalDevice, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("vkCreateSwapChainKHR failed");
            return false;
        }

        uint32_t imageCount = std::min(surfCapabilities.minImageCount + 1, surfCapabilities.maxImageCount);
        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

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
            if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
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

        if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }

        swapChainFramebuffers.resize(swapChainImageViews.size());
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            VkImageView attachments[] = {
                swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = graphicsAndComputeQueueFamilyIndex;
        if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &graphicsCMD) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &computeCMD) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to create semaphores!");
        }

        if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &computeFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(logicalDevice, &fenceInfo, nullptr, &computeInFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute synchronization objects for a frame!");
        }

        VkDeviceSize bufferSize = sizeof(UniformBufferPreFrame);

        deviceUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        deviceUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        deviceUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, deviceUniformBuffers[i], deviceUniformBuffersMemory[i]);
            vkMapMemory(logicalDevice, deviceUniformBuffersMemory[i], 0, bufferSize, 0, &deviceUniformBuffersMapped[i]);
        }

        VkDeviceSize modelBufferSize = sizeof(Model::UniformBufferPreDraw);
        modelUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        modelUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        modelUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(modelBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, modelUniformBuffers[i], modelUniformBuffersMemory[i]);
            vkMapMemory(logicalDevice, modelUniformBuffersMemory[i], 0, modelBufferSize, 0, &modelUniformBuffersMapped[i]);
        }

        startTime = std::chrono::high_resolution_clock::now();
        return true;
    }

    void Device::destroy() {
        vkDestroySemaphore(logicalDevice, computeFinishedSemaphore, nullptr);
        vkDestroyFence(logicalDevice, computeInFlightFence, nullptr);
        vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
        vkDestroyFence(logicalDevice, inFlightFence, nullptr);
        vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
        for (auto framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(logicalDevice, deviceUniformBuffers[i], nullptr);
            vkDestroyBuffer(logicalDevice, modelUniformBuffers[i], nullptr);
            vkFreeMemory(logicalDevice, deviceUniformBuffersMemory[i], nullptr);
            vkFreeMemory(logicalDevice, modelUniformBuffersMemory[i], nullptr);
        }

        vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

        for (auto imageView : swapChainImageViews)
        {
            vkDestroyImageView(logicalDevice, imageView, nullptr);
        }

        vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyDevice(logicalDevice, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void Device::beginFrame() {
        vkWaitForFences(logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(logicalDevice, 1, &inFlightFence);

        vkAcquireNextImageKHR(logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphore, nullptr, &imageIndex);
        vkResetCommandBuffer(graphicsCMD, 0);
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = nullptr; // Optional

        if (vkBeginCommandBuffer(graphicsCMD, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = swapChainFramebuffers[imageIndex];
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = swapChainExtent;

        VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(graphicsCMD, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChainExtent.width);
        viewport.height = static_cast<float>(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(graphicsCMD, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;
        vkCmdSetScissor(graphicsCMD, 0, 1, &scissor);

        const float currentTime = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - startTime).count();
        deltaTime = currentTime - timeSinceStartup;
        timeSinceStartup = currentTime;

        ubo.param.x = timeSinceStartup;
        ubo.param.z = static_cast<float>(swapChainExtent.width);
        ubo.param.w = static_cast<float>(swapChainExtent.height);
        memcpy(deviceUniformBuffersMapped[imageIndex], &ubo, sizeof(ubo));
    }

    void Device::endFrame() {
        vkCmdEndRenderPass(graphicsCMD);
        if (vkEndCommandBuffer(graphicsCMD) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { computeFinishedSemaphore, imageAvailableSemaphore };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 2;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &graphicsCMD;

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = { swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;
        vkQueuePresentKHR(presentQueue, &presentInfo);
    }

    VkShaderModule Device::createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    void Device::waitFenceAndReset()
    {
        vkWaitForFences(logicalDevice, 1, &computeInFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(logicalDevice, 1, &computeInFlightFence);
        vkWaitForFences(logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(logicalDevice, 1, &inFlightFence);
    }

    void Device::draw(Model* model, GraphicsPipeline* pipeline) {
        model->updateModelUBO(timeSinceStartup, modelUniformBuffersMapped);

        vkCmdBindDescriptorSets(graphicsCMD, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &pipeline->descriptorSets[imageIndex], 0, nullptr);

    	vkCmdBindPipeline(graphicsCMD, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);
        VkBuffer vertexBuffers[] = { model->vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(graphicsCMD, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(graphicsCMD, model->indexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(graphicsCMD, model->getIndexCount(), 1, 0, 0, 0);
    }

    void Device::draw(ParticleSystem* particleSystem, GraphicsPipeline* pipeline)
    {
        vkCmdBindDescriptorSets(graphicsCMD, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &pipeline->descriptorSets[imageIndex], 0, nullptr);
        vkCmdBindPipeline(graphicsCMD, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(graphicsCMD, 0, 1, &particleSystem->shaderStorageBuffers[imageIndex], offsets);
        vkCmdDraw(graphicsCMD, particleSystem->particleSize, 1, 0, 0);
    }

    void Device::dispatch(ParticleSystem* particleSystem, ComputePipeline* pipeline)
    {
        vkWaitForFences(logicalDevice, 1, &computeInFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(logicalDevice, 1, &computeInFlightFence);

        vkResetCommandBuffer(computeCMD, 0);
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(computeCMD, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        particleSystem->updateUBO();
    	vkCmdBindPipeline(computeCMD, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->computePipeline);
        vkCmdBindDescriptorSets(computeCMD, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->computePipelineLayout, 0, 1, &pipeline->computeDescriptorSets[imageIndex], 0, 0);
    	vkCmdDispatch(computeCMD, particleSystem->rowSize / 32, particleSystem->colSize / 32, 1);

    	if (vkEndCommandBuffer(computeCMD) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &computeCMD;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &computeFinishedSemaphore;
        if (vkQueueSubmit(computeQueue, 1, &submitInfo, computeInFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit compute command buffer!");
        }
    }


    void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);
    }


    void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
    }

    uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }


    bool Device::checkValidationLayerSupport() {
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
}
