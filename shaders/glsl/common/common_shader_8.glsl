#define TEXTURE_2D 0
#define TEXTURE_3D 1
#define TEXTURE_CUBE_MAP 2
#define TEXTURE_RECT 3

#ifndef TEXTURE_TYPE
#    define TEXTURE_TYPE TEXTURE_2D
#endif

attribute highp   vec4 Vertex;
#if TEXTURE_TYPE == TEXTURE_3D || TEXTURE_TYPE == TEXTURE_CUBE_MAP
attribute mediump vec3 TexCoord0;
#else
attribute mediump vec2 TexCoord0;
#endif
attribute lowp    vec4 Color0;

uniform   highp   mat4 WorldViewProjectionMatrix;

#if TEXTURE_TYPE == TEXTURE_3D || TEXTURE_TYPE == TEXTURE_CUBE_MAP
varying   mediump vec3 vCoord0;
#else
varying   mediump vec2 vCoord0;
#endif
varying   lowp    vec4 vColor0;

#ifdef TEXTURE_MATRIX
uniform   mediump mat4 TextureMatrix;
#endif

void main(void)
{
#ifdef TEXTURE_MATRIX
#    if TEXTURE_TYPE == TEXTURE_3D || TEXTURE_TYPE == TEXTURE_CUBE_MAP
    vCoord0 = (TextureMatrix * vec4(TexCoord0, 1.0)).xyz;
#    else
    vCoord0 = (TextureMatrix * vec4(TexCoord0, 0.0, 1.0)).xy;
#    endif
#else
    vCoord0 = TexCoord0;
#endif
    vColor0 = Color0;
    gl_Position = WorldViewProjectionMatrix * Vertex;
}
