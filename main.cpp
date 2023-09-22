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
#include "include/texture.h"
#include "include/camera.h"

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

    auto* texture = new Texture();
    texture->create(device, "textures\\logo.png");

    auto* camera = new Camera();
    camera->create(device);
    camera->position = glm::vec3(0, 0, 2);
    camera->lookAt = glm::vec3(0, 0, 0);
    camera->aspect = static_cast<float>(width) / height;
    camera->fov = 45;
    camera->near = 0.01f;
    camera->far = 100;

    auto* graphicsPipeline = new GraphicsPipeline();
    graphicsPipeline->create(device, camera, texture);

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
        // set camera ubo pre frame
        camera->updateCameraUBO();

        device->beginFrame();
        device->draw(vertexBuffer, indexBuffer, 6, graphicsPipeline->pipelineLayout, graphicsPipeline->getPipeline(InternalShaderType::Unlit_Texture), graphicsPipeline->descriptorSets);
        // device->draw(vertexBuffer, indexBuffer, 6, Device::InternalShader::Unlit_Texture);
        device->endFrame();
    }

    device->destroyVertexBuffer(vertexBuffer);
    device->destroyIndexBuffer(indexBuffer);

    texture->destroy();
    delete texture;

    camera->destroy();
    graphicsPipeline->destroy();

    device->destroy();
    delete device;
    device = nullptr;
    window->closeWindow();
    delete window;
    window = nullptr;
	
    return 0;
}