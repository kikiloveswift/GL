#version 330 core
out vec4 FragColor;

in vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform float time;

void main()
{
    // 基础颜色混合
    vec4 texColor = texture(ourTexture, TexCoord);
    
    // 添加时间动画效果
    float wave = sin(time * 2.0 + TexCoord.x * 10.0) * 0.1 + 0.9;
    
    // 混合顶点颜色和纹理颜色
    FragColor = mix(vec4(vertexColor, 1.0), texColor, 0.5) * wave;
}
