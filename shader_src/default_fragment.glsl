
#version 330

// from vertex shader
in vec3 vertexNormal;
in vec4 vertexColor;
in vec2 texCoord;

// textures
uniform sampler2D texture0; 

out vec4 colorOut;

void main(void)
{
    vec4 texelColor = texture2D(texture0, texCoord.xy);
    colorOut = texelColor;
}