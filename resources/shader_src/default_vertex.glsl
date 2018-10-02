#version 120

// Plain, stupid passthrough shader without lighting and stuff.
//  Sort of a fallback, also useful for non-3D GUI elements.

varying vec2 texCoord;
varying vec4 vertexColor;

void main(void)
{    
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vertexColor = gl_Color;
    texCoord = gl_MultiTexCoord0.xy;
}

