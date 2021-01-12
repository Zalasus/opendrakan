
#version 120

// Plain, stupid passthrough shader without lighting and stuff.
//  Sort of a fallback, also useful for non-3D GUI elements.

#pragma import_defines(TEXTURE, COLOR_MODIFIER)

varying vec2 texCoord;
varying vec4 vertexColor;

#ifdef TEXTURE
    uniform sampler2D texture;
#endif

#ifdef COLOR_MODIFIER
    uniform vec4 colorModifier;
#endif

void main(void)
{

#ifndef TEXTURE
    vec4 texelColor = vec4(1.0);
#else
    vec4 texelColor = texture2D(texture, texCoord.xy);
#endif

#ifdef COLOR_MODIFIER
    texelColor *= colorModifier;
#endif

    gl_FragColor = clamp(texelColor * vertexColor, 0.0, 1.0);
}
