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
    

    const std::vector<Device::Vertex> vertices = {
        {{-0.5f, -0.5f}, {0, 0}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f}, {1, 0}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f}, {0, 1}, {0.0f, 0.0f, 1.0f, 1.0f}},
        {{ 0.5f,  0.5f}, {1, 1}, {1.0f, 0.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 3, 3, 2, 0
    };

    Device::VertexBuffer vertexBuffer = device->createVertexBuffer(vertices);
    Device::IndexBuffer indexBuffer = device->createIndexBuffer(indices);

    while (!window->windowShouldClose())
    {
        window->update();
        device->beginFrame();
        device->draw(vertexBuffer, indexBuffer, 6, Device::InternalShader::Color);
        // device->draw(vertexBuffer, indexBuffer, 6, Device::InternalShader::Texture);
        device->endFrame();
    }

    device->destroyVertexBuffer(vertexBuffer);
    device->destroyIndexBuffer(indexBuffer);

    device->destroy();
    delete device;
    device = nullptr;
    window->closeWindow();
    delete window;
    window = nullptr;
	
    return 0;
}