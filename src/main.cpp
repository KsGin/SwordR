/*
    2020.11.9 by yangfan
*/
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "Include/GLFWManager.hpp"
#include "Include/VkManager.hpp"

using namespace VKDemo;

int main() {

    const char* name = "Vulkan Demo";
    const int width = 800;
    const int height = 600;

	auto* glfwMgr = new GLFWManager();
    if (!glfwMgr->CreateWindow(name, width, height))
    {
        std::cout << "GLFW CreateWindow Failed!!!" << std::endl;
        return -1;
    }

    auto* vkMgr = new VKManager();
    if (!vkMgr->CreateWithGLFWWindow(glfwMgr->GetWindow(), width, height))
    {
        std::cout << "VK CreateWithGLFWWindow Failed!!!" << std::endl;
        glfwMgr->CloseWindow();
        delete glfwMgr;
        return -1;
    }
    

    while (!glfwMgr->WindowShouldClose())
    {
        glfwMgr->Update();
    }

    vkMgr->Destroy();
    delete vkMgr;
    vkMgr = nullptr;
    glfwMgr->CloseWindow();
    delete glfwMgr;
    glfwMgr = nullptr;
	
    return 0;
}