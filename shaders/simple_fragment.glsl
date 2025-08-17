#version 330 core
out vec4 FragColor;

uniform vec3 color;
uniform float time;

void main()
{
    // 简单的颜色动画
    vec3 animatedColor = color * (sin(time) * 0.5 + 0.5);
    FragColor = vec4(animatedColor, 1.0);
}
