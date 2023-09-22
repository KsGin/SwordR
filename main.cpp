/*
    2020.11.9 by yangfan
*/
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "include/window.h"
#include "include/device.h"
#include "include/pipeline.h"

using namespace SwordR;

int main() {

    const char* name = "Vulkan Demo";
    const int width = 1920;
    const int height = 1080;

	auto* window = new Window();
    if (!window->create(name, width, height))
    {
        std::cout << "GLFW CreateWindow Failed!!!" << std::endl;
        return -1;
    }

    auto* device = new Device();
    if (!device->createWithWindow(window->getWindow(), width, height))
    {
        std::cout << "VK CreateWithGLFWWindow Failed!!!" << std::endl;
        window->closeWindow();
        delete window;
        return -1;
    }
    
    auto* graphicsPipeline = new GraphicsPipeline();
    graphicsPipeline->create(device);

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {0, 0}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f}, {1, 0}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f}, {0, 1}, {0.0f, 0.0f, 1.0f, 1.0f}},
        {{ 0.5f,  0.5f}, {1, 1}, {1.0f, 0.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 3, 3, 2, 0
    };

    VkBuffer vertexBuffer = device->createVertexBuffer(vertices);
    VkBuffer indexBuffer = device->createIndexBuffer(indices);

    while (!window->windowShouldClose())
    {
        window->update();
        device->beginFrame();
        device->draw(vertexBuffer, indexBuffer, 6, graphicsPipeline->pipelineLayout, graphicsPipeline->getPipeline(InternalShaderType::Color), graphicsPipeline->descriptorSets);
        // device->draw(vertexBuffer, indexBuffer, 6, Device::InternalShader::Texture);
        device->endFrame();
    }

    device->destroyVertexBuffer(vertexBuffer);
    device->destroyIndexBuffer(indexBuffer);

    graphicsPipeline->destroy();

    device->destroy();
    delete device;
    device = nullptr;
    window->closeWindow();
    delete window;
    window = nullptr;
	
    return 0;
}