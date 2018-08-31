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

vec4 calcSingleLight(gl_LightSourceParameters light, gl_MaterialParameters material, vec3 vertex_cs, vec3 normal_cs)
{  
    // calculate light direction and attenuation
    vec3 lightDir_cs;
    float attenuation;
    if(light.position.w == 0.0) // w = 0 means directional light
    {
        lightDir_cs = normalize(light.position.xyz);
        attenuation = 1.0; // no attenuation for directional lights
        
    }else
    {
        lightDir_cs = light.position.xyz - vertex_cs;
        float distance = length(lightDir_cs);
        lightDir_cs = normalize(lightDir_cs);
        
        attenuation = 1.0/(light.constantAttenuation + 
                light.linearAttenuation*distance + 
                light.quadraticAttenuation*distance*distance);
        attenuation = clamp(attenuation, 0.0, 1.0);
    }
    
    // ambient term
    vec4 ambientColor = light.ambient * material.ambient;
    
    // diffuse term
    float cosTheta = max(dot(normal_cs, lightDir_cs), 0.0);
    vec4 diffuseColor = cosTheta * attenuation * light.diffuse * material.diffuse;
    
    // specular term
    float cosAlpha = max(dot(normal_cs, light.halfVector.xyz), 0.0);
    vec4 specularColor = pow(cosAlpha, material.shininess) * attenuation * light.specular * material.specular;
      
    return clamp(ambientColor + diffuseColor, 0.0, 1.0) + specularColor; // FIXME: this will break ambient and diffuse lighting once we use many lights
}

vec4 calcLighting()
{
    vec3 vertex_cs = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 normal_cs = normalize(gl_NormalMatrix * gl_Normal); 

    vec4 lightColor = vec4(0.0);
    for(int i = 0; i < gl_MaxLights; ++i)
    {
        lightColor += calcSingleLight(gl_LightSource[i], gl_FrontMaterial, vertex_cs, normal_cs);
    }
    
    return lightColor;
}

void main(void)
{
    vertexColor = gl_Color * calcLighting();
    vertexColor.w = 1.0;
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    vertexNormal = gl_NormalMatrix * gl_Normal;
    texCoord = gl_MultiTexCoord0.xy;
}

