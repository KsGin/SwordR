#pragma once
#include <GLFW/glfw3.h>

namespace SwordR
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
}
