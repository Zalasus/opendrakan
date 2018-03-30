
#version 330 core

// default osg input stuff
layout(location = 0) in vec4 osg_Vertex;
layout(location = 1) in vec3 osg_Normal;
layout(location = 2) in vec4 osg_Color; 
layout(location = 3) in vec4 osg_MultiTexCoord0;
uniform mat4 osg_ModelViewMatrix;
uniform mat3 osg_NormalMatrix;
uniform mat4 osg_ProjectionMatrix; 

layout(location = 14) in vec4 influencingBones;
layout(location = 15) in vec4 vertexWeights;
uniform mat4 bones[64];

out vec3 vertexNormal;
out vec4 vertexColor;
out vec2 texCoord;

void main(void)
{   
    mat4 totalBoneXform = mat4(0.0);
    for(int i = 0; i < 4; ++i)
    {
        int boneIndex = min(int(influencingBones[i]), 63);
        
        float vertexWeight = vertexWeights[i];
        mat4 bone = bones[boneIndex];
        
        totalBoneXform += bone * vertexWeight;
    }
    
    vec4 newPos = totalBoneXform * osg_Vertex;
    vec4 newNormal = totalBoneXform * vec4(osg_Normal, 0.0);
    
    gl_Position = osg_ProjectionMatrix * osg_ModelViewMatrix * vec4(newPos.xyz, 1.0);
    vertexNormal = osg_NormalMatrix * newNormal.xyz;
    
    vertexColor = osg_Color;
    texCoord = osg_MultiTexCoord0.xy;
}


