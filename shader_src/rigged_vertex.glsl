
#version 330

// default osg input stuff
layout(location = 0) in vec4 osg_Vertex;
layout(location = 1) in vec3 osg_Normal;
layout(location = 2) in vec4 osg_Color; 
layout(location = 3) in vec4 osg_MultiTexCoord0;
uniform mat4 osg_ModelViewMatrix;
uniform mat3 osg_NormalMatrix;
uniform mat4 osg_ProjectionMatrix; 

in uvec4 influencingBones;
in vec4  vertexWeights;
uniform mat4 bones[64];

out vec3 vertexNormal;
out vec4 vertexColor;
out vec2 texCoord;

void main(void)
{
    vec4 newPos = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 newNormal = vec4(0.0, 0.0, 0.0, 0.0);
    
    for(int i = 0; i < 4; ++i)
    {
        newPos += (bones[influencingBones[i]] * osg_Vertex) * vertexWeights[i];
        newNormal += (bones[influencingBones[i]] * vec4(osg_Normal, 0.0)) * vertexWeights[i];
    }
    
    // remove once we really upload bones and set influence attributes
    newPos = osg_Vertex;
    newNormal = vec4(osg_Normal, 0.0);
    
    gl_Position = osg_ProjectionMatrix * osg_ModelViewMatrix * vec4(newPos.xyz, 1.0);
    vertexNormal = osg_NormalMatrix * newNormal.xyz;
    
    vertexColor = osg_Color;
    texCoord = osg_MultiTexCoord0.xy;
}