#version 450

layout(binding = 0) uniform CameraUniformBufferPreFrame {
    mat4 view;
    mat4 proj;
} cameraUBO;

layout(binding = 1) uniform ModelUniformBufferPreFrame {
    mat4 model;
} modelUBO;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;

void main() {
    gl_Position = cameraUBO.proj * cameraUBO.view * modelUBO.model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor.rgb;
    fragUV = inUV;
}