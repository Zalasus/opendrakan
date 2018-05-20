
#version 120

// from vertex shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

// textures
uniform sampler2D texture0; 

//out vec4 colorOut;

void main(void)
{
    vec4 texelColor = texture2D(texture0, texCoord.xy);
    //colorOut = texelColor;
    gl_FragColor = texelColor;
}