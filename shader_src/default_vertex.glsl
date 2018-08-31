#version 120

// output for fragment shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

/*
Built-in uniforms that might be useful here:

struct gl_LightSourceParameters 
{ 
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
uniform gl_LightSourceParameters gl_LightSource[gl_MaxLights]; 

struct gl_MaterialParameters
{
    vec4  emission; 
    vec4  ambient;
    vec4  diffuse;
    vec4  specular;
    float shininess;
};
uniform gl_MaterialParameters  gl_FrontMaterial;
uniform gl_MaterialParameters  gl_BackMaterial;
*/

vec4 calcSingleLight(gl_LightSourceParameters light, vec3 vertex_cs, vec3 normal_cs, vec4 vertexColor)
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
    
    float attenuation = 1.0/(light.constantAttenuation + 
                light.linearAttenuation*distance + 
                light.quadraticAttenuation*distance*distance);
    attenuation = clamp(attenuation, 0.0, 1.0);
    
    // ambient term
    vec4 ambientColor = light.ambient*vertexColor;
    
    // diffuse term
    float cosTheta = clamp(dot(normal_cs, lightDir_cs), 0.0, 1.0);
    vec4 diffuseColor = vertexColor*cosTheta*attenuation*light.diffuse;
    
    // specular term
    float cosAlpha = dot(normal_cs, light.halfVector.xyz);
    vec4 specularColor = vertexColor*pow(cosAlpha, 20)*attenuation*light.specular;
      
    vec4 resultColor = ambientColor + diffuseColor + specularColor;
      
    return clamp(resultColor, 0.0, 1.0);
}

vec4 calcLighting()
{
    vec3 vertex_cs = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 normal_cs = normalize(gl_NormalMatrix * gl_Normal); 

    vec4 lightColor = vec4(0.0);
    for(int i = 0; i < gl_MaxLights; ++i)
    {
        lightColor += calcSingleLight(gl_LightSource[i], vertex_cs, normal_cs, gl_Color);
    }
    
    // don't let lighting mess with alpha for now
    lightColor.w = gl_Color.w;
    
    return clamp(lightColor, 0.0, 1.0);
}

void main(void)
{
    vertexColor = calcLighting();
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    vertexNormal = gl_NormalMatrix * gl_Normal;
    texCoord = gl_MultiTexCoord0.xy;
}

