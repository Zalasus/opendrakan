

#version 120

// output for fragment shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

/*struct gl_LightSourceParameters { 
    vec4  ambient; 
    vec4  diffuse; 
    vec4  specular; 
    vec4  position; 
    vec4  halfVector; 
    vec3  spotDirection; 
    float  spotExponent; 
    float  spotCutoff; 
    float  spotCosCutoff; 
    float  constantAttenuation; 
    float  linearAttenuation; 
    float  quadraticAttenuation; 
};

uniform gl_LightSourceParameters gl_LightSource[gl_MaxLights]; */

vec4 calcLight(gl_LightSourceParameters light, vec3 vertex_cs, vec3 normal_cs, vec4 vertexColor)
{  
    // calculate light direction and distance
    vec3 lightDir_cs;
    if(light.position.w == 0.0) // w = 0 means directional light
    {
        lightDir_cs = light.position.xyz;
        
    }else
    {
        lightDir_cs = light.position.xyz - vertex_cs;
    }
    float distance = length(lightDir_cs);
    lightDir_cs = normalize(lightDir_cs);
    
    vec4 ambientColor = light.ambient * vertexColor;
    
    float cosTheta = clamp(dot(normal_cs, lightDir_cs), 0.0, 1.0);
    float attenuation = 1.0/(light.constantAttenuation + 
                light.linearAttenuation*distance + 
                light.quadraticAttenuation*distance*distance);
      
    vec4 resultColor = ambientColor + vertexColor*cosTheta*attenuation*light.diffuse;
      
    return clamp(resultColor, 0.0, 1.0);
}

void main(void)
{
    vec3 vertex_cs = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 normal_cs = normalize(gl_NormalMatrix * gl_Normal); 

    vec4 lightColor = vec4(0.0);
    for(int i = 0; i < gl_MaxLights; ++i)
    {
        lightColor += calcLight(gl_LightSource[i], vertex_cs, normal_cs, gl_Color);
    }
    vertexColor = clamp(lightColor, 0.0, 1.0);
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    vertexNormal = gl_NormalMatrix * gl_Normal;
    texCoord = gl_MultiTexCoord0.xy;
}

