/*
    2020.11.9 by yangfan
*/
#include <iostream>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "Include/GLFWManager.hpp"

using namespace VKDemo;

int main() {
    auto glfwMgr = new GLFWManager();
    
    glfwMgr->CreateWindow("Vulkan Demo", 800, 600);
	

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported" << std::endl;

    while (!glfwMgr->WindowShouldClose())
    {
        glfwMgr->Update();
    }

    glfwMgr->CloseWindow();
	
    return 0;
}