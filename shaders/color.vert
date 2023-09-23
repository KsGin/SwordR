#version 450

layout(set = 0, binding = 0) uniform CameraUniformBufferPreFrame {
    mat4 view;
    mat4 proj;
} cameraUBO;

layout(set = 0, binding = 1) uniform DeviceUniformBufferPreFrame {
    float time;
    float unused;
    float width;
    float height;
} deviceUBO;

struct Particle {
    vec2 position;
    vec4 color;
};

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_PointSize = 2.0f;
    gl_Position = vec4((inPosition.x / deviceUBO.width), (inPosition.y / deviceUBO.height), 0.0, 1.0);
    fragColor = inColor;
}