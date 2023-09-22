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
#include "include/model.h"

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
    camera->position = glm::vec3(0, 0, 1.5f);
    camera->lookAt = glm::vec3(0, 0, 0);
    camera->aspect = static_cast<float>(width) / height;
    camera->fov = 45;
    camera->near = 0.01f;
    camera->far = 100;

    auto* model = new Model();
    model->create(device, Model::Quad);

    auto* pipeline = new Pipeline();
    Pipeline::PipelineCreateInfo info{
        Pipeline::BaseMap, camera, texture
    };
    pipeline->create(device, info);

    while (!window->windowShouldClose())
    {
        window->update();
        // set camera ubo pre frame
        camera->updateCameraUBO();

        device->beginFrame();
        device->draw(model, pipeline);
        device->endFrame();
    }

    device->waitFenceAndReset();

    model->destroy();
    delete model;

    texture->destroy();
    delete texture;

    camera->destroy();
    delete camera;

    pipeline->destroy();
    delete pipeline;

    device->destroy();
    delete device;

    window->closeWindow();
    delete window;
	
    return 0;
}