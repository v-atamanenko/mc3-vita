uniform sampler2D DiffuseMapSampler;
uniform sampler2D LightMapSampler;
uniform sampler2D LightMapSampler2;
uniform sampler2D ReflectionMapSampler;
uniform samplerCube CubeMapSampler;
uniform lowp float lightMapSetFactor;
uniform lowp vec4 lightmapFactor;
varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;

uniform sampler2D DetailMapSampler;
uniform mediump vec2 detailTile;
uniform lowp float detailAmount;

uniform lowp vec4 fogcolor;
varying lowp float fogFactor;
varying highp vec3 vReflection;

uniform mediump float minReflectionAmount;
uniform mediump float maxReflectionAmount;

#define USE_ALPHA_TEXTURE
#ifdef USE_ALPHA_TEXTURE
uniform lowp sampler2D DiffuseMapSampler_alpha;
#endif

uniform lowp vec3 sunFactorA;

void main()
{
#ifdef USE_ALPHA_TEXTURE
    lowp vec4 color = vec4(texture2D(DiffuseMapSampler, vCoord0).rgb,
                            texture2D(DiffuseMapSampler_alpha, vCoord0).g);
#else
    lowp vec4 color = texture2D(DiffuseMapSampler, vCoord0);
#endif

    lowp vec4 lightmap = texture2D(LightMapSampler, vCoord1);

#ifdef DUAL
    lowp vec4 lightmap2 = texture2D(LightMapSampler2, vCoord1);
    lightmap = mix(lightmap, lightmap2, lightMapSetFactor);
#endif

    lightmap *= lightmapFactor;

#ifdef DETAIL
    lowp vec4 detailmap = (texture2D(DetailMapSampler, vCoord0 * detailTile)-0.5);
    color += detailmap * detailAmount;
#endif

    color.rgb *= lightmap.rgb;

#ifdef CUBE


    mediump vec4 reflectedColor = textureCube(CubeMapSampler, vReflection);
    reflectedColor *= minReflectionAmount + (lightmap * maxReflectionAmount - minReflectionAmount);
    #ifdef REFLECTIONMAP
        lowp vec4 reflectionMap = texture2D(ReflectionMapSampler, vCoord0);
        reflectedColor *= reflectionMap;
    #endif
    color += reflectedColor;
#endif

#ifdef FOG
    color.xyz = mix(color.xyz, fogcolor.xyz, fogFactor);
#endif //FOG

    color.rgb += sunFactorA;

#ifdef GAMMA
    color.rgb *= color.rgb;
#endif

    gl_FragColor = color;
}
