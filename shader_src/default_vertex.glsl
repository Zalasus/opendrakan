

#version 120

// output for fragment shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

uniform mat4 osg_ViewMatrix;

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

void main(void)
{
     vec4 lightColor = vec4(1.0, 0.95, 0.9, 1.0);
     float lightPower = 1.0;
     float ambientFactor = 0.2;
     vec3 lightPosition = vec3(109.0, 100.0, 79.0);

    vec3 vertex_cs = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vec3 lightPos_cs = (osg_ViewMatrix * vec4(lightPosition, 1.0)).xyz;
    vec3 lightDir_cs = lightPos_cs - vertex_cs;
    float distance = length(lightDir_cs);
    lightDir_cs = normalize(lightDir_cs);
    vec3 normal_cs = normalize(gl_NormalMatrix * gl_Normal); 
      
    float cosTheta = clamp(dot(normal_cs, lightDir_cs), 0.0, 1.0);
    vec4 ambienceColor = vec4(ambientFactor, ambientFactor, ambientFactor, 1.0) * gl_Color;
    distance = 1;
    vertexColor = ambienceColor + gl_Color * cosTheta * lightColor * lightPower / (distance*distance);
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    vertexNormal = gl_NormalMatrix * gl_Normal;
    texCoord = gl_MultiTexCoord0.xy;
}