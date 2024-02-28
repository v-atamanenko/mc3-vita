#ifdef NORMALMAP
    #define SPECULAR
#endif

// Smaplers
uniform lowp sampler2D DiffuseMapSampler;
uniform highp vec3 light0diffuseColor;
uniform highp vec4 SceneAmbientLight;
uniform lowp sampler2D NormalMapSampler;
uniform lowp samplerCube CubeMapSampler;
uniform lowp samplerCube NormalCubeSampler;
uniform lowp sampler2D SpecularMapSampler;

varying highp vec2 vCoord0;
varying highp vec3 vNormal;
varying lowp vec3 vDiffuse;
varying highp vec3 vEyeDirection;

// Fog
uniform lowp vec4 fogcolor;
varying lowp float fogFactor;

// Specular
uniform highp float specPower;
varying lowp vec3 vHalf;
varying mediump vec3 vSpecIntensity;

// Rim
varying highp vec2 vRimCoord;
uniform lowp sampler2D RimLightMapSampler;

// Reflection
varying mediump float vReflectionAmount;
varying highp vec3 vReflection;

uniform lowp vec3 sunFactorA;

#define USE_ALPHA_TEXTURE
#ifdef USE_ALPHA_TEXTURE
uniform lowp sampler2D DiffuseMapSampler_alpha;
#endif

void main()
{
#ifdef USE_ALPHA_TEXTURE
    lowp vec4 Color = vec4(texture2D(DiffuseMapSampler, vCoord0).rgb,
                            texture2D(DiffuseMapSampler_alpha, vCoord0).g);
#else
    lowp vec4 Color = texture2D(DiffuseMapSampler, vCoord0);
#endif
    Color.rgb *= vDiffuse;

#ifdef CG_FOG
    Color.rgb = mix(Color.rgb, fogcolor.rgb, fogFactor);
#endif


#if defined(SPECULARMAP) || defined(REFLECTIONMAP)
    highp vec3 maps = texture2D(SpecularMapSampler, vCoord0).rgb;
#endif


#if defined(SPECULAR) || defined(CUBE)

    #ifdef NORMALMAP
        lowp vec3 normal = (texture2D(NormalMapSampler, vCoord0)*2.0-1.0).xyz;
    #else
        highp vec3 normal = vNormal;
    #endif

#endif

#ifdef SPECULAR

    mediump float power = specPower;
    #if defined(SPECULARMAP) && !defined(UNIFORMPOWER)
        power *= maps.g;
    #endif

    mediump float spec = max(dot(normal, textureCube(NormalCubeSampler,vHalf).xyz*2.0-1.0),0.0);
    #ifdef UNIFORMPOWER
        spec *= spec;
        spec *= spec;
        spec *= spec;
    #else
        spec = pow(spec, power);
    #endif



    #ifdef SPECULARMAP
        spec *= maps.r;
    #endif

    Color.rgb += spec * vSpecIntensity;

#endif

#ifdef CUBE

    #ifdef NORMALMAP
        highp vec3 reflection = reflect(vEyeDirection, normal);
    #else
        highp vec3 reflection = vReflection;
    #endif

    mediump vec4 reflectionColor = textureCube(CubeMapSampler, reflection);

    #ifdef REFLECTIONMAP
        reflectionColor.rgb *= maps.b;
    #endif

    Color.rgb += reflectionColor.rgb * vReflectionAmount;
#endif

#ifdef RIM
    Color.rgb += texture2D(RimLightMapSampler, vRimCoord).rgb;
#endif
    Color.rgb += sunFactorA;

    gl_FragColor = Color;// * 0.001 + 20.0 * abs(textureCube(NormalCubeSampler,debug.xyz)-dn);
}
