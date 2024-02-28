#if defined(LIGHTMAP) || defined(CG_FOG)
    #define DIFFUSE
#endif

uniform sampler2D DiffuseMapSampler;
uniform samplerCube CubeMapSampler;
uniform sampler2D LightMapSampler;
uniform sampler2D NormalMapSampler;
uniform sampler2D ReflectionMapSampler;

varying mediump vec2 vCoordDiffuse;
varying mediump vec2 vCoordLightMap;
varying mediump vec2 vCoordReflectionMap;
varying mediump vec4 vDebug;

varying highp vec3 vReflection;
varying lowp vec3 vReflectionAmount;

uniform lowp vec4 fogcolor;
varying lowp float fogFactor;

uniform lowp vec4 lightmapFactor;
uniform lowp vec3 sunFactorA;

void main()
{
    lowp vec4 color = vec4(0,0,0,0);

#ifdef DIFFUSE
    color = texture2D(DiffuseMapSampler, vCoordDiffuse);
#endif

#ifdef LIGHTMAP
    lowp vec4 lightmap = texture2D(LightMapSampler, vCoordLightMap);
    lightmap *= (2.0 - lightmap.a);
    color *= lightmap * lightmapFactor;
#endif


    lowp vec4 reflectedColor = textureCube(CubeMapSampler, vReflection);
    reflectedColor.rgb *= vReflectionAmount;

#ifdef REFLECTIONMAP
    lowp vec4 reflectionMap = texture2D(ReflectionMapSampler, vCoordReflectionMap);
    reflectedColor.rgb *= reflectionMap.r;
#endif


    color += reflectedColor;


#ifdef CG_FOG
    color.xyz = mix(color.xyz, fogcolor.xyz, fogFactor);
#endif

#ifdef DIFFUSE
    color.rgb += sunFactorA;
#endif

    gl_FragColor = color;
}
