#version 120


#pragma import_defines(MAX_LIGHTS)


// Layers are very rigid in their shading, so this is basically a non-parametric version of the model shader.


varying vec2 texCoord;
varying vec4 vertexColor;

varying vec3 lightColor;

// although the layer light is baked into the vertex colors, we still apply the ambient color since the LightManager uses it to "disable" lighting
uniform vec3  layerLightAmbient;

uniform vec3  objectLightDiffuse[MAX_LIGHTS];
uniform float objectLightIntensity[MAX_LIGHTS];
uniform float objectLightRadius[MAX_LIGHTS];
uniform vec3  objectLightPosition[MAX_LIGHTS];

vec3 calcLighting(vec3 vertex_cs, vec3 normal_cs)
{
    vec3 resultLightColor = layerLightAmbient;
    
    for(int i = 0; i < MAX_LIGHTS; ++i)
    {
        vec3 lightDir_cs = objectLightPosition[i] - vertex_cs;
        float distance = length(lightDir_cs);
        lightDir_cs = normalize(lightDir_cs);
        
        float normDistance = distance/objectLightRadius[i];
        float attenuation = -0.82824*normDistance*normDistance - 0.13095*normDistance + 1.01358;
        attenuation = clamp(attenuation, 0.0, 1.0);

        float cosTheta = max(dot(normal_cs, lightDir_cs), 0.0);
        
        resultLightColor += objectLightIntensity[i] * objectLightDiffuse[i] * cosTheta * attenuation;
    }
    
    return resultLightColor;
}

void main(void)
{
    vec4 vertex_ms = gl_Vertex;
    vec3 normal_ms = gl_Normal;

    vec4 vertex_cs = gl_ModelViewMatrix * vertex_ms;
    vec3 normal_cs = normalize(gl_NormalMatrix * normal_ms);
    
    lightColor = calcLighting(vertex_cs.xyz, normal_cs) + gl_Color.xyz; // layer lights are baked into vertex colors
    lightColor = clamp(lightColor, 0.0, 1.0);
    
    gl_Position = gl_ProjectionMatrix * vertex_cs;
    vertexColor = gl_Color;
    texCoord = gl_MultiTexCoord0.xy;
}

