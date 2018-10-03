#version 120


#pragma use_defines(LIGHTING, RIGGING, SPECULAR, MAX_LIGHTS, MAX_BONES)


#ifndef MAX_LIGHTS
    #define MAX_LIGHTS 8
#endif

#ifndef MAX_BONES
    #define MAX_BONES 64
#endif


varying vec2 texCoord;
varying vec4 vertexColor;


#ifdef LIGHTING
    #define MAX_LIGHTS 8

    varying vec3 lightColor;
    
    #ifdef SPECULAR
        varying vec3 specularColor;
    #endif
    
    uniform vec3  layerLightDiffuse;
    uniform vec3  layerLightAmbient;
    uniform vec3  layerLightDirection;
    
    uniform vec3  objectLightDiffuse[MAX_LIGHTS];
    uniform float objectLightIntensity[MAX_LIGHTS];
    uniform float objectLightRadius[MAX_LIGHTS];
    uniform vec3  objectLightPosition[MAX_LIGHTS];

    vec3 calcLighting(vec3 vertex_cs, vec3 normal_cs)
    {
        vec3 resultLightColor;
        
        // layer light first
        float layerCosTheta = max(dot(normal_cs, layerLightDirection), 0.0);
        resultLightColor = layerLightAmbient + layerLightDiffuse*layerCosTheta;
        
        #ifdef SPECULAR
            vec3 halfVector = normalize(layerLightDirection + normalize(-vertex_cs));
            float cosAlpha = max(dot(halfVector, normal_cs), 0.0);
            specularColor = layerLightDiffuse * pow(cosAlpha, 50);
        #endif
        
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
            
            #ifdef SPECULAR
                vec3 halfVector = normalize(lightDir_cs + normalize(-vertex_cs));
                float cosAlpha = max(dot(halfVector, normal_cs), 0.0);
                specularColor += objectLightDiffuse[i] * attenuation * pow(cosAlpha, 50);
            #endif
        }
        
        return resultLightColor;
    }
#endif


#ifdef RIGGING
    attribute vec4 influencingBones;
    attribute vec4 vertexWeights;
    uniform mat4 bones[MAX_BONES];
    
    mat4 calcTotalBoneTransform()
    {   
        mat4 totalBoneXform = mat4(0.0);
        
        for(int i = 0; i < 4; ++i)
        {
            int boneIndex = int(min(influencingBones[i], MAX_BONES - 0.6)); // subtract less than 1 to prevent rounding errors in min function
            
            float vertexWeight = vertexWeights[i];
            mat4 bone = bones[boneIndex];
            
            totalBoneXform += bone * vertexWeight;
        }
        
        return totalBoneXform;
    }
#endif


void main(void)
{
    vec4 vertex_ms = gl_Vertex;
    vec3 normal_ms = gl_Normal;

#ifdef RIGGING
    mat4 boneTransform = calcTotalBoneTransform();
    vertex_ms = boneTransform * vertex_ms;
    vertex_ms.w = 1.0;
    normal_ms = (boneTransform * vec4(normal_ms, 0.0)).xyz;
#endif

    vec4 vertex_cs = gl_ModelViewMatrix * vertex_ms;
    vec3 normal_cs = normalize(gl_NormalMatrix * normal_ms);
    
#ifdef LIGHTING
    lightColor = calcLighting(vertex_cs.xyz, normal_cs);
#endif
    
    gl_Position = gl_ProjectionMatrix * vertex_cs;
    vertexColor = gl_Color;
    texCoord = gl_MultiTexCoord0.xy;
}



