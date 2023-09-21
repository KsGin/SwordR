#pragma once
#include "../include/window.h"

namespace SwordR
{
	Window::Window()
	{
		windowPtr = nullptr;
	}

	Window::~Window()
	{
		windowPtr = nullptr;
	}

	bool Window::create(const char* windowName, const int width, const int height)
	{
		glfwInit();

		if (GLFW_FALSE == glfwVulkanSupported()) {
			glfwTerminate();
			return false;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		if (windowPtr == nullptr)
		{
			windowPtr = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
		}
		return true;
	}

	bool Window::windowShouldClose()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}

		return glfwWindowShouldClose(windowPtr);
	}

	bool Window::closeWindow()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}

		glfwDestroyWindow(windowPtr);
		glfwTerminate();
		return true;
	}

	bool Window::update()
	{
		if (windowPtr == nullptr)
		{
			return false;
		}

		glfwPollEvents();
		return true;
	}

	GLFWwindow* Window::getWindow()
	{
		return windowPtr;
	}
}