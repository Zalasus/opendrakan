#version 120


#pragma use_defines(LIGHTING, SPECULAR, GAMMA_CORRECTION, COLOR_MODIFIER)

varying vec2 texCoord;
varying vec4 vertexColor;

uniform sampler2D texture; 


#ifdef LIGHTING
    varying vec3 lightColor;
    
    #ifdef SPECULAR
        varying vec3 specularColor;    
    #endif
#endif


#ifdef GAMMA_CORRECTION
    uniform float fullScreenGamma;
#endif

#ifdef COLOR_MODIFIER
    uniform vec4 colorModifier;
#endif


void main(void)
{    
    vec4 textureAndLightColor = texture2D(texture, texCoord.xy);
    
#ifdef LIGHTING
    vec3 lightFactor = vec3(1.0);
    #ifdef GAMMA_CORRECTION
        lightFactor = pow(lightColor, vec3(1.0/fullScreenGamma));
    #else
        lightFactor = lightColor;
    #endif
    
    textureAndLightColor *= vec4(lightFactor, 1.0);
#endif

#ifdef COLOR_MODIFIER
    textureAndLightColor *= colorModifier;
#endif

#ifdef SPECULAR
    textureAndLightColor += vec4(specularColor, 0.0);    
#endif
    
    gl_FragColor = clamp(vertexColor * textureAndLightColor, 0.0, 1.0);
}



