#pragma once
#include <GLFW/glfw3.h>

namespace VKDemo
{
	class GLFWManager
	{
	private:
		GLFWwindow* windowPtr;
		
	public:
		GLFWManager();
		~GLFWManager();
		
		bool CreateWindow(const char* windowName, const int width, const int height);
		bool WindowShouldClose();
		bool CloseWindow();
		bool Update();
		GLFWwindow* GetWindow();
	};

	inline GLFWManager::GLFWManager()
	{
		windowPtr = nullptr;
	}

	inline GLFWManager::~GLFWManager()
	{
		windowPtr = nullptr;
	}

	inline bool GLFWManager::CreateWindow(const char* windowName , const int width , const int height)
	{
		glfwInit();

		if (GLFW_FALSE == glfwVulkanSupported()) {
			glfwTerminate();
			return false;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		
		if (windowPtr == nullptr)
		{
			windowPtr = glfwCreateWindow(width, height, windowName , nullptr , nullptr);
		}
		return true;
	}
	
	inline bool GLFWManager::WindowShouldClose()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}
		
		return glfwWindowShouldClose(windowPtr);
	}

	inline bool GLFWManager::CloseWindow()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}
		
		glfwDestroyWindow(windowPtr);
		glfwTerminate();
		return true;
	}

	inline bool GLFWManager::Update()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}
		
		glfwPollEvents();
		return true;
	}

	inline GLFWwindow* GLFWManager::GetWindow()
	{
		return windowPtr;
	}
}
