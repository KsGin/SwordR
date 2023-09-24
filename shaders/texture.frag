#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

layout(binding = 3) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragUV);
    outColor.rgb = outColor.rgb * fragColor.rgb;
    outColor.a = fragColor.a;
}