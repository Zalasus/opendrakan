

#version 120

// output for fragment shader
varying vec3 vertexNormal;
varying vec4 vertexColor;
varying vec2 texCoord;

void main(void)
{
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    vertexNormal = gl_NormalMatrix * gl_Normal;
    vertexColor = gl_Color;
    texCoord = gl_MultiTexCoord0.xy;
}