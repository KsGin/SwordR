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
#include "include/GraphicsPipeline.h"
#include "include/texture.h"
#include "include/camera.h"
#include "include/model.h"
#include "include/computePipeline.h"
#include "include/particle.h"

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

    auto* modelPipeline = new GraphicsPipeline();
    modelPipeline->create(device, { GraphicsPipeline::ModelRenderPipeline , camera , texture });

    auto* particleSystem = new ParticleSystem();
    particleSystem->create(device);

    auto* computePipeline = new ComputePipeline();
    computePipeline->create(device, particleSystem);

    auto* particlePipeline = new GraphicsPipeline();
    particlePipeline->create(device, { GraphicsPipeline::ParticleRenderPipeline, camera });

    while (!window->windowShouldClose())
    {
        window->update();
        camera->updateCameraUBO();

        device->dispatchCompute(particleSystem, computePipeline);

        device->beginFrame();
        device->draw(model, modelPipeline);
        device->draw(particleSystem, particlePipeline);
        device->endFrame();
    }

    device->waitFenceAndReset();

    particleSystem->destroy();
    delete particleSystem;

    computePipeline->destroy();
    delete computePipeline;

    model->destroy();
    delete model;

    texture->destroy();
    delete texture;

    camera->destroy();
    delete camera;

    particlePipeline->destroy();
    delete particlePipeline;

    modelPipeline->destroy();
    delete modelPipeline;

    device->destroy();
    delete device;

    window->closeWindow();
    delete window;
	
    return 0;
}