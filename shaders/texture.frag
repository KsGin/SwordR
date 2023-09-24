#version 450

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragUV;

layout(binding = 3) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

// 2d gradient noise
vec2 random2(vec2 st){
    st = vec2( dot(st,vec2(127.1,311.7)),
              dot(st,vec2(269.5,183.3)) );
    return -1.0 + 2.0*fract(sin(st)*43758.5453123);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( random2(i + vec2(0.0,0.0) ), f - vec2(0.0,0.0) ),
                     dot( random2(i + vec2(1.0,0.0) ), f - vec2(1.0,0.0) ), u.x),
                mix( dot( random2(i + vec2(0.0,1.0) ), f - vec2(0.0,1.0) ),
                     dot( random2(i + vec2(1.0,1.0) ), f - vec2(1.0,1.0) ), u.x), u.y);
}

void main() {
    
    outColor = texture(texSampler, fragUV);
    outColor.rgb = outColor.rgb * fragColor.rgb;
    if (outColor.a >= fragColor.a) {
        outColor.a = fragColor.a;
    }
    float noiseAlpha = noise(fragUV * 30);
    if (outColor.a < noiseAlpha) {
        outColor.a = 0;
    }
}