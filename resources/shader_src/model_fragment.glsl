#version 120


#pragma use_defines(LIGHTING, GAMMA_CORRECTION, COLOR_MODIFIER)


varying vec3 vertexNormal;
varying vec2 texCoord;

uniform sampler2D texture0; 


#ifdef LIGHTING
    varying vec3 lightColor;
#endif


#ifdef GAMMA_CORRECTION
    uniform float fullScreenGamma;
#endif

#ifdef COLOR_MODIFIER
    uniform vec4 colorModifier;
#endif


void main(void)
{    
    vec4 fragmentColor = texture2D(texture0, texCoord.xy);
    
#ifdef LIGHTING
    vec3 lightFactor = vec3(1.0);
    #ifdef GAMMA_CORRECTION
        lightFactor = pow(lightColor, vec3(1.0/fullScreenGamma));
    #else
        lightFactor = lightColor;
    #endif
    
    fragmentColor *= vec4(lightFactor, 1.0);
#endif

#ifdef COLOR_MODIFIER
    fragmentColor *= colorModifier;
#endif
    
    gl_FragColor = clamp(fragmentColor, 0.0, 1.0);
}



