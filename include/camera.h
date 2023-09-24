#pragma once
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "device.h"

namespace SwordR
{
	class Camera
	{
		friend class GraphicsPipeline;

		Device* device;

		struct UniformBufferPreFrame
		{
			alignas(64) glm::mat4 view;
			alignas(64) glm::mat4 proj;
		};

		UniformBufferPreFrame ubo{};

		std::vector<VkBuffer> uniformBuffers;
		std::vector<VkDeviceMemory> uniformBuffersMemory;
		std::vector<void*> uniformBuffersMapped;

	public:
		enum ViewType { Orthographic, Perspective};
		ViewType viewType = Perspective;

		glm::vec3 position {0,0,2};
		glm::vec3 lookAt {0,0,0};
		float fov = 60;
		float near = 0.01f;
		float far = 100.0f;
		float aspect = 1.78f;
		float left = -100.0f;
		float right = 100.0f;
		float top = 100.0f;
		float bottom = -100.0f;
		void create(Device* device);
		void updateCameraUBO();
		void destroy();
	};
}
