
#version 120

// from vertex shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

// textures
uniform sampler2D texture0; 


void main(void)
{
    vec4 texelColor = texture2D(texture0, texCoord.xy);

    gl_FragColor = texelColor * vertexColor;
}