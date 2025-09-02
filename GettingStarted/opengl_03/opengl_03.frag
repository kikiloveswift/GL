#version 330
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform bool flipY;
void main() {
    vec2 flipCoord = flipY ? vec2(TexCoord.x, 1 - TexCoord.y) : TexCoord;
    FragColor = texture(ourTexture, flipCoord);
}