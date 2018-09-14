
#version 120

attribute vec4 influencingBones;
attribute vec4 vertexWeights;
uniform mat4 bones[64];

// output for fragment shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

void main(void)
{   
    mat4 totalBoneXform = mat4(0.0);
    for(int i = 0; i < 4; ++i)
    {
        int boneIndex = int(min(influencingBones[i], 63.4)); // 63.4 to prevent rounding errors in min function
        
        float vertexWeight = vertexWeights[i];
        mat4 bone = bones[boneIndex];
        
        totalBoneXform += bone * vertexWeight;
    }
    
    vec4 newPos = totalBoneXform * gl_Vertex;
    vec4 newNormal = totalBoneXform * vec4(gl_Normal, 0.0);
    
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(newPos.xyz, 1.0);
    vertexNormal = gl_NormalMatrix * newNormal.xyz;
    
    vertexColor = gl_Color;
    texCoord = gl_MultiTexCoord0.xy;
}


