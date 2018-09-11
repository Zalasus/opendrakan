#version 120

#define MAX_LIGHTS 8

// output for fragment shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

// lighting uniforms
uniform vec3  layerLightDiffuse;
uniform vec3  layerLightAmbient;
uniform vec3  layerLightDirection;

uniform vec3  objectLightDiffuse[MAX_LIGHTS];
uniform float objectLightIntensity[MAX_LIGHTS];
uniform float objectLightRadius[MAX_LIGHTS];
uniform vec3  objectLightPosition[MAX_LIGHTS];

vec4 calcLighting(vec3 vertex_cs, vec3 normal_cs, vec4 vertexColor)
{
    vec3 resultLightColor;
    
    // layer light first
    float layerCosTheta = max(dot(normal_cs, layerLightDirection), 0.0);
    resultLightColor = layerLightAmbient + layerLightDiffuse*layerCosTheta;
    
    for(int i = 0; i < MAX_LIGHTS; ++i)
    {
        vec3 lightDir_cs = objectLightPosition[i] - vertex_cs;
        float distance = length(lightDir_cs);
        lightDir_cs = normalize(lightDir_cs);
        
        float attenuation = clamp(1.0 - distance/objectLightRadius[i], 0.0, 1.0);

        float cosTheta = max(dot(normal_cs, lightDir_cs), 0.0);
        
        resultLightColor += objectLightIntensity[i] * objectLightDiffuse[i] * cosTheta * attenuation;
    }
    
    return vertexColor * vec4(resultLightColor, 1.0);
}

void main(void)
{
    vec4 vertex_cs = gl_ModelViewMatrix * gl_Vertex;
    vec3 normal_cs = normalize(gl_NormalMatrix * gl_Normal);    

    vertexColor = calcLighting(vertex_cs.xyz, normal_cs, gl_Color);
    vertexColor = clamp(vertexColor, 0.0, 1.0);
    
    gl_Position = gl_ProjectionMatrix * vertex_cs;
    vertexNormal = normal_cs;
    texCoord = gl_MultiTexCoord0.xy;
}

