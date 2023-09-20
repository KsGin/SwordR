#pragma once
#include <GLFW/glfw3.h>

namespace VKDemo
{
	class Window
	{
	private:
		GLFWwindow* windowPtr;
		
	public:
		Window();
		~Window();
		
		bool Create(const char* windowName, const int width, const int height);
		bool WindowShouldClose();
		bool CloseWindow();
		bool Update();
		GLFWwindow* GetWindow();
	};

	inline Window::Window()
	{
		windowPtr = nullptr;
	}

	inline Window::~Window()
	{
		windowPtr = nullptr;
	}

	inline bool Window::Create(const char* windowName , const int width , const int height)
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
	
	inline bool Window::WindowShouldClose()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}
		
		return glfwWindowShouldClose(windowPtr);
	}

	inline bool Window::CloseWindow()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}
		
		glfwDestroyWindow(windowPtr);
		glfwTerminate();
		return true;
	}

	inline bool Window::Update()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}
		
		glfwPollEvents();
		return true;
	}

	inline GLFWwindow* Window::GetWindow()
	{
		return windowPtr;
	}
}
