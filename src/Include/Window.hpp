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
		
		bool create(const char* windowName, const int width, const int height);
		bool windowShouldClose();
		bool closeWindow();
		bool update();
		GLFWwindow* getWindow();
	};

	inline Window::Window()
	{
		windowPtr = nullptr;
	}

	inline Window::~Window()
	{
		windowPtr = nullptr;
	}

	inline bool Window::create(const char* windowName , const int width , const int height)
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
	
	inline bool Window::windowShouldClose()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}
		
		return glfwWindowShouldClose(windowPtr);
	}

	inline bool Window::closeWindow()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}
		
		glfwDestroyWindow(windowPtr);
		glfwTerminate();
		return true;
	}

	inline bool Window::update()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}
		
		glfwPollEvents();
		return true;
	}

	inline GLFWwindow* Window::getWindow()
	{
		return windowPtr;
	}
}
