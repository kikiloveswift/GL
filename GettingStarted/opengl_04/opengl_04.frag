#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float FaceId;

uniform sampler2D textures[6];

void main()
{
    int faceIndex = int(FaceId);
    vec2 adjustedTexCoord = TexCoord;
    
    // 根据不同面ID调整纹理坐标
    if (faceIndex == 0) {
        // 前面 - Y轴翻转
        adjustedTexCoord.y = 1.0 - TexCoord.y;
    }
    else if (faceIndex == 1) {
        // 后面 - Y轴翻转
        adjustedTexCoord.y = 1.0 - TexCoord.y;
    }
    else if (faceIndex == 2) {
        // 左面 - Y轴翻转
        adjustedTexCoord.y = 1.0 - TexCoord.y;
    }
    else if (faceIndex == 3) {
        // 右面 - Y轴翻转
        adjustedTexCoord.y = 1.0 - TexCoord.y;
    }
    else if (faceIndex == 4) {
        // 上面 - Y轴翻转
        adjustedTexCoord.y = 1.0 - TexCoord.y;
    }
    else if (faceIndex == 5) {
        // 下面 - Y轴翻转
        adjustedTexCoord.y = 1.0 - TexCoord.y;
    }
    
    FragColor = texture(textures[faceIndex], adjustedTexCoord);
}