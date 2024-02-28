#define TEXTURE_1D 0
#define TEXTURE_2D 1
#define TEXTURE_3D 2
#define TEXTURE_CUBE_MAP 3
#define TEXTURE_RECT 4

#ifndef TEXTURE_TYPE
#    define TEXTURE_TYPE TEXTURE_2D
#endif

#if GL_ES && TEXTURE_TYPE == TEXTURE_3D && !defined(GL_OES_texture_3D)
#    undef TEXTURE_TYPE
#    define TEXTURE_TYPE TEXTURE_2D
#elif GL_ES && TEXTURE_TYPE == TEXTURE_3D
#    extension GL_OES_texture_3D : enable
#elif GL_ES && TEXTURE_TYPE == TEXTURE_RECT
#    undef TEXTURE_TYPE
#    define TEXTURE_TYPE TEXTURE_2D
#elif TEXTURE_TYPE == TEXTURE_RECT
#    extension GL_ARB_texture_rectangle : enable
#elif GL_ES && TEXTURE_TYPE == TEXTURE_1D
#    undef TEXTURE_TYPE
#    define TEXTURE_TYPE TEXTURE_2D
#endif

#if TEXTURE_TYPE == TEXTURE_3D
uniform lowp    sampler3D texture;
#elif TEXTURE_TYPE == TEXTURE_CUBE_MAP
uniform lowp    samplerCube texture;
#elif TEXTURE_TYPE == TEXTURE_RECT
uniform lowp    sampler2DRect texture;
#elif TEXTURE_TYPE == TEXTURE_1D
uniform lowp    sampler1D texture;
#else
uniform lowp    sampler2D texture;
#endif

#if TEXTURE_TYPE == TEXTURE_3D || TEXTURE_TYPE == TEXTURE_CUBE_MAP
varying mediump vec3 vCoord0;
#else
varying mediump vec2 vCoord0;
#endif

void main()
{
#if TEXTURE_TYPE == TEXTURE_3D
    gl_FragColor = texture3D(texture, vCoord0);
#elif TEXTURE_TYPE == TEXTURE_CUBE_MAP
    gl_FragColor = textureCube(texture, vCoord0);
#elif TEXTURE_TYPE == TEXTURE_RECT
    gl_FragColor = texture2DRect(texture, vCoord0);
#elif TEXTURE_TYPE == TEXTURE_1D
    gl_FragColor = texture1D(texture, vCoord0.x);
#else
    gl_FragColor = texture2D(texture, vCoord0);
#endif
}
