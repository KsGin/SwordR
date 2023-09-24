#version 450

layout(set = 0, binding = 0) uniform CameraUniformBufferPreFrame {
    mat4 view;
    mat4 proj;
} cameraUBO;


layout(set = 0, binding = 1) uniform ModelUniformBufferPreDraw {
    mat4 model;
} modelUBO;

layout(set = 0, binding = 2) uniform DeviceUniformBufferPreFrame {
    float time;
    float unused;
    float width;
    float height;
} deviceUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragUV;

void main() {
    gl_Position = cameraUBO.proj * cameraUBO.view * modelUBO.model * vec4(inPosition, 1.0);
    fragColor = vec4(cos(deviceUBO.time) / 2 + 0.8f, 0, cos(deviceUBO.time) / 2 + 0.8f + sin(deviceUBO.time) / 2 + 0.8f, pow(max(1 - (deviceUBO.time / 40) , 0.0f), 4));
    fragUV = inUV;
}