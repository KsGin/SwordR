#pragma once
#include <filesystem>
#include <iostream>
#include "../include/pipeline.h"
#include "../include/camera.h"
#include "../include/model.h"
#include "../include/texture.h"
#include "../include/utils.h"

namespace SwordR
{
    VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device->logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    
    void Pipeline::create(Device* device, PipelineCreateInfo info)
    {
        this->device = device;
        this->info = info;
        createPipeline();
        createDescriptorPool();
        createDescriptorSets();
    }

    void Pipeline::createPipeline() {

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        auto bindingDescription = Model::Vertex::getBindingDescription();
        auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data(); // Optional

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(device->swapChainExtent.width);
        viewport.height = static_cast<float>(device->swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = device->swapChainExtent;

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
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

        VkDescriptorSetLayoutBinding cameraUBOLayoutBinding{};
        cameraUBOLayoutBinding.binding = 0;
        cameraUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        cameraUBOLayoutBinding.descriptorCount = 1;
        cameraUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        cameraUBOLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutBinding modelUBOLayoutBinding{};
        modelUBOLayoutBinding.binding = 1;
        modelUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        modelUBOLayoutBinding.descriptorCount = 1;
        modelUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        modelUBOLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutBinding systemUBOLayoutBinding{};
        systemUBOLayoutBinding.binding = 2;
        systemUBOLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        systemUBOLayoutBinding.descriptorCount = 1;
        systemUBOLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        systemUBOLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

        switch (info.type)
        {
			case PipelineType::BaseMap : {
                VkDescriptorSetLayoutBinding samplerLayoutBinding{};
                samplerLayoutBinding.binding = 3;
                samplerLayoutBinding.descriptorCount = 1;
                samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                samplerLayoutBinding.pImmutableSamplers = nullptr;
                samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

                std::array<VkDescriptorSetLayoutBinding, 4> bindings = { cameraUBOLayoutBinding, modelUBOLayoutBinding, systemUBOLayoutBinding, samplerLayoutBinding };
                layoutInfo.bindingCount = 4;
                layoutInfo.pBindings = bindings.data();
		        break;
	        }

			case BaseColor:{
                std::array<VkDescriptorSetLayoutBinding, 3> bindings = { cameraUBOLayoutBinding, modelUBOLayoutBinding, systemUBOLayoutBinding};
                layoutInfo.bindingCount = 3;
                layoutInfo.pBindings = bindings.data();
                break;
			}
            default:{
                throw std::runtime_error("type not found");
			}
        }

        if (vkCreateDescriptorSetLayout(device->logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1; // Optional
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

        if (vkCreatePipelineLayout(device->logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        auto shaderName = shaderNameList[static_cast<int>(info.type)];
        std::cout << "Current Path : " << std::filesystem::current_path() << std::endl;
        auto vsShaderCompileCommand = std::format("sdk\\VulkanSDK\\1.2.154.1\\Bin\\glslc.exe shaders\\{}.vert -o x64\\Debug\\{}_vs.spv", shaderName, shaderName);
        auto fsShaderCompileCommand = std::format("sdk\\VulkanSDK\\1.2.154.1\\Bin\\glslc.exe shaders\\{}.frag -o x64\\Debug\\{}_fs.spv", shaderName, shaderName);
        std::cout << "Complie Vertex Shader : Execute " << vsShaderCompileCommand << std::endl;
        system(vsShaderCompileCommand.c_str());
        std::cout << "Complie Fragment Shader : Execute " << fsShaderCompileCommand << std::endl;
        system(fsShaderCompileCommand.c_str());

        auto vertexShaderCodePath = std::format("x64\\Debug\\{}_vs.spv", shaderName);
        auto fragmentShaderCodePath = std::format("x64\\Debug\\{}_fs.spv", shaderName);
        auto vertShaderCode = utils::readFile(vertexShaderCodePath);
        auto fragShaderCode = utils::readFile(fragmentShaderCodePath);
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
        pipelineInfo.renderPass = device->renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = nullptr; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(device->logicalDevice, nullptr, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device->logicalDevice, fragShaderModule, nullptr);
        vkDestroyShaderModule(device->logicalDevice, vertShaderModule, nullptr);
    }

    void Pipeline::createDescriptorPool() {
        std::array<VkDescriptorPoolSize, 4> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(device->MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(device->MAX_FRAMES_IN_FLIGHT);
        poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[2].descriptorCount = static_cast<uint32_t>(device->MAX_FRAMES_IN_FLIGHT);
        poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[3].descriptorCount = static_cast<uint32_t>(device->MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(device->MAX_FRAMES_IN_FLIGHT);
        if (vkCreateDescriptorPool(device->logicalDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void Pipeline::createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(device->MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(device->MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();
        descriptorSets.resize(device->MAX_FRAMES_IN_FLIGHT);

        if (vkAllocateDescriptorSets(device->logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < device->MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo cameraUBOInfo{};
            cameraUBOInfo.buffer = info.camera->uniformBuffers[i];
            cameraUBOInfo.offset = 0;
            cameraUBOInfo.range = sizeof(Camera::UniformBufferPreFrame);

            VkDescriptorBufferInfo modelUBOInfo{};
            modelUBOInfo.buffer = device->modelUniformBuffers[i];
            modelUBOInfo.offset = 0;
            modelUBOInfo.range = sizeof(Model::UniformBufferPreDraw);

            VkDescriptorBufferInfo deviceUBOInfo{};
            deviceUBOInfo.buffer = device->deviceUniformBuffers[i];
            deviceUBOInfo.offset = 0;
            deviceUBOInfo.range = sizeof(Device::UniformBufferPreFrame);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = info.texture->imageView;
            imageInfo.sampler = info.texture->sampler;

            std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &cameraUBOInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = &modelUBOInfo;

            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = descriptorSets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pBufferInfo = &deviceUBOInfo;

            descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[3].dstSet = descriptorSets[i];
            descriptorWrites[3].dstBinding = 3;
            descriptorWrites[3].dstArrayElement = 0;
            descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[3].descriptorCount = 1;
            descriptorWrites[3].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device->logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void Pipeline::destroy()
    {
        vkDestroyDescriptorPool(device->logicalDevice, descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(device->logicalDevice, descriptorSetLayout, nullptr);

        vkDestroyPipeline(device->logicalDevice, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device->logicalDevice, pipelineLayout, nullptr);
    }
}
