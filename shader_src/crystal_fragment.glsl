
#version 120

// from vertex shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

// textures
uniform sampler2D texture0; 

// crystal color
uniform vec4 colorModifier;


void main(void)
{
    vec4 texelColor = texture2D(texture0, texCoord.xy);

    gl_FragColor = clamp(texelColor * vertexColor * colorModifier, 0.0, 1.0);
}