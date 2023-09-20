/*
    2020.11.9 by yangfan
*/
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "include/Window.hpp"
#include "include/Device.hpp"

using namespace VKDemo;

int main() {

    const char* name = "Vulkan Demo";
    const int width = 1920;
    const int height = 1080;

	auto* window = new Window();
    if (!window->Create(name, width, height))
    {
        std::cout << "GLFW CreateWindow Failed!!!" << std::endl;
        return -1;
    }

    auto* device = new Device();
    if (!device->CreateWithWindow(window->GetWindow(), width, height))
    {
        std::cout << "VK CreateWithGLFWWindow Failed!!!" << std::endl;
        window->CloseWindow();
        delete window;
        return -1;
    }
    

    while (!window->WindowShouldClose())
    {
        window->Update();
        if (device->beginFrame())
        {
            device->DrawTriangle(Device::InternalShader::Color);
            device->DrawTriangle(Device::InternalShader::Texture);
            device->endFrame();
        }
    }

    device->Destroy();
    delete device;
    device = nullptr;
    window->CloseWindow();
    delete window;
    window = nullptr;
	
    return 0;
}