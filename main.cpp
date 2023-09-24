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
    const int width = 3840;
    const int height = 2400;

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

    auto* camera = new Camera();
    camera->create(device);
    camera->position = glm::vec3(0, 0, 2);
    camera->lookAt = glm::vec3(0, 0, 0);
    camera->aspect = static_cast<float>(width) / height;
    camera->fov = 90;
    camera->near = 0.01f;
    camera->far = 100;

    auto* model = new Model();
    model->create(device, Model::Quad);
    model->transform.position = glm::vec3(0, 0, 0);
    model->transform.scale = glm::vec3(6, 6, 6);

    auto* graphicsTexture = new Texture();
    graphicsTexture->create(device, { "textures\\logo.png", VK_FORMAT_R8G8B8A8_UNORM, Texture::Graphics });

    auto* modelGraphicsPipeline = new GraphicsPipeline();
    modelGraphicsPipeline->create(device, { GraphicsPipeline::ModelRenderPipeline , camera , graphicsTexture });

    auto* particleSystem = new ParticleSystem();
    particleSystem->create(device);

    auto* computeTexture = new Texture();
    computeTexture->create(device, { "textures\\logo.png", VK_FORMAT_R8G8B8A8_UNORM, Texture::Compute });

    auto* particleComputePipeline = new ComputePipeline();
    particleComputePipeline->create(device, {particleSystem, computeTexture });

    auto* particleGraphicsPipeline = new GraphicsPipeline();
    particleGraphicsPipeline->create(device, { GraphicsPipeline::ParticleRenderPipeline, camera });

    while (!window->windowShouldClose())
    {
        //model->transform.rotation = glm::vec3(0, device->timeSinceStartup * 1, 0);

    	window->update();
        camera->updateCameraUBO();

        device->dispatch(particleSystem, particleComputePipeline);
        device->beginFrame();
        device->draw(model, modelGraphicsPipeline);
        device->draw(particleSystem, particleGraphicsPipeline);
        device->endFrame();
    }

    device->waitFenceAndReset();

    particleSystem->destroy();
    delete particleSystem;

    particleComputePipeline->destroy();
    delete particleComputePipeline;

    model->destroy();
    delete model;

    graphicsTexture->destroy();
    delete graphicsTexture;

    computeTexture->destroy();
    delete computeTexture;

    camera->destroy();
    delete camera;

    particleGraphicsPipeline->destroy();
    delete particleGraphicsPipeline;

    modelGraphicsPipeline->destroy();
    delete modelGraphicsPipeline;

    device->destroy();
    delete device;

    window->closeWindow();
    delete window;
	
    return 0;
}