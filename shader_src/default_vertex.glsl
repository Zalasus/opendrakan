

#version 130

// default osg input stuff
in vec4 osg_Vertex;
in vec3 osg_Normal;
in vec4 osg_Color; 
in vec4 osg_MultiTexCoord0;
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