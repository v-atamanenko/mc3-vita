#define TEXTURE_1D 0
#define TEXTURE_2D 1
#define TEXTURE_3D 2
#define TEXTURE_CUBE_MAP 3
#define TEXTURE_RECT 4

#ifndef TEXTURE_TYPE
#    define TEXTURE_TYPE TEXTURE_2D
#endif

#if GL_ES
#    if TEXTURE_TYPE == TEXTURE_3D && !defined(GL_OES_texture_3D)
#        undef TEXTURE_TYPE
#        define TEXTURE_TYPE TEXTURE_2D
#    elif TEXTURE_TYPE == TEXTURE_RECT
#        undef TEXTURE_TYPE
#        define TEXTURE_TYPE TEXTURE_2D
#    endif
#endif

attribute highp vec4 Vertex;
#if TEXTURE_TYPE == TEXTURE_3D || TEXTURE_TYPE == TEXTURE_CUBE_MAP
attribute mediump vec3 TexCoord0;
#else
attribute mediump vec2 TexCoord0;
#endif

#if TEXTURE_TYPE == TEXTURE_3D || TEXTURE_TYPE == TEXTURE_CUBE_MAP
varying mediump vec3 vCoord0;
#else
varying mediump vec2 vCoord0;
#endif

void main(void)
{
    vCoord0 = TexCoord0;
    gl_Position = Vertex;
}
