
#version 120

// from vertex shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

// textures
uniform sampler2D texture0; 

uniform float fullScreenGamma;

void main(void)
{
    vec4 texelColor = texture2D(texture0, texCoord.xy);
    
    vec3 gammaLighting = pow(vertexColor.xyz, vec3(1.0/fullScreenGamma));
    vec4 gammaColorWithAlpha = vec4(gammaLighting, vertexColor.w); // keep alpha linear (we can probably remove alpha from light)

    gl_FragColor = clamp(texelColor * gammaColorWithAlpha, 0.0, 1.0);
}