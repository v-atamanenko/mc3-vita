#define TEXTURE_2D 0
#define TEXTURE_3D 1
#define TEXTURE_CUBE_MAP 2
#define TEXTURE_RECT 3

#ifndef TEXTURE_TYPE
#    define TEXTURE_TYPE TEXTURE_2D
#endif

#if GL_ES && TEXTURE_TYPE == TEXTURE_3D && !defined(GL_OES_texture_3D)
#    undef TEXTURE_TYPE
#    define TEXTURE_TYPE TEXTURE_2D
#elif GL_ES && TEXTURE_TYPE == TEXTURE_3D
#    extension GL_OES_texture_3D : enable
#elif GL_ES && TEXTURE_TYPE == TEXTURE_RECT
#    error texture rectangle not supported with GLES2
#elif TEXTURE_TYPE == TEXTURE_RECT
#    extension GL_ARB_texture_rectangle : enable
#endif

#if TEXTURE_TYPE == TEXTURE_3D
uniform lowp    sampler3D texture;
#elif TEXTURE_TYPE == TEXTURE_CUBE_MAP
uniform lowp    samplerCube texture;
#elif TEXTURE_TYPE == TEXTURE_RECT
uniform lowp    sampler2DRect texture;
#else
uniform lowp    sampler2D texture;
uniform lowp    sampler2D texture_alpha;
#endif

#if TEXTURE_TYPE == TEXTURE_3D || TEXTURE_TYPE == TEXTURE_CUBE_MAP
varying mediump vec3 vCoord0;
#else
varying mediump vec2 vCoord0;
#endif
varying lowp vec4 vColor0;

void main()
{
#if TEXTURE_TYPE == TEXTURE_3D
    gl_FragColor = texture3D(texture, vCoord0) * vColor0;
#elif TEXTURE_TYPE == TEXTURE_CUBE_MAP
    gl_FragColor = textureCube(texture, vCoord0) * vColor0;
#elif TEXTURE_TYPE == TEXTURE_RECT
    gl_FragColor = texture2DRect(texture, vCoord0) * vColor0;
#else
    gl_FragColor = vec4(texture2D(texture, vCoord0).rgb,
                    texture2D(texture_alpha, vCoord0).g) * vColor0;
#endif
}
