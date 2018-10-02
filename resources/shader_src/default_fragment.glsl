
#version 120

// Plain, stupid passthrough shader without lighting and stuff.
//  Sort of a fallback, also useful for non-3D GUI elements.

varying vec2 texCoord;

uniform sampler2D texture; 

void main(void)
{
    vec4 texelColor = texture2D(texture, texCoord.xy);
    gl_FragColor = clamp(texelColor, 0.0, 1.0);
}