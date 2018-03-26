

#version 330

// default osg input stuff
layout(location = 0) in vec4 osg_Vertex;
layout(location = 1) in vec3 osg_Normal;
layout(location = 2) in vec4 osg_Color; 
layout(location = 3) in vec4 osg_MultiTexCoord0;
uniform mat4 osg_ModelViewMatrix;
uniform mat3 osg_NormalMatrix;
uniform mat4 osg_ProjectionMatrix; 

out vec3 vertexNormal;
out vec4 vertexColor;
out vec2 texCoord;

void main(void)
{
    gl_Position = osg_ProjectionMatrix * osg_ModelViewMatrix * osg_Vertex;
    vertexNormal = osg_NormalMatrix * osg_Normal;
    vertexColor = osg_Color;
    texCoord = osg_MultiTexCoord0.xy;
}