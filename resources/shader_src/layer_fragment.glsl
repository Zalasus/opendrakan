
#version 120

#pragma import_defines(GAMMA_CORRECTION)

// Layers are very rigid in their shading, so this is basically a non-parametric version of the model shader.
//  Later, we might want to account for blending between layers, different water shaders etc.

varying vec2 texCoord;

uniform sampler2D texture; 

varying vec3 lightColor;


#ifdef GAMMA_CORRECTION
    uniform float fullScreenGamma;
#endif


void main(void)
{    
    vec4 textureAndLightColor = texture2D(texture, texCoord.xy);
    
    vec3 lightFactor = vec3(1.0);
#ifdef GAMMA_CORRECTION
    lightFactor = pow(lightColor, vec3(1.0/fullScreenGamma));
#else
    lightFactor = lightColor;
#endif
    
    textureAndLightColor *= vec4(lightFactor, 1.0);
    
    gl_FragColor = clamp(textureAndLightColor, 0.0, 1.0);
}
