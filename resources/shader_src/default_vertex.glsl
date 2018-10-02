#version 120

// Plain, stupid passthrough shader without lighting and stuff.
//  Sort of a fallback, also useful for non-3D GUI elements.

varying vec2 texCoord;

void main(void)
{    
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    texCoord = gl_MultiTexCoord0.xy;
}

