

#version 120

// default osg input stuff
attribute vec4 osg_Vertex;
attribute vec3 osg_Normal;
attribute vec4 osg_Color; 
attribute vec4 osg_MultiTexCoord0;
uniform mat4 osg_ModelViewMatrix;
uniform mat3 osg_NormalMatrix;
uniform mat4 osg_ProjectionMatrix; 

varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

void main(void)
{
    gl_Position = osg_ProjectionMatrix * osg_ModelViewMatrix * osg_Vertex;
    vertexNormal = osg_NormalMatrix * osg_Normal;
    vertexColor = osg_Color;
    texCoord = osg_MultiTexCoord0.xy;
}