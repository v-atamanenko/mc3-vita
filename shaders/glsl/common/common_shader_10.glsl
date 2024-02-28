#ifdef TEXTURED
#define USE_ALPHA_TEXTURE
uniform lowp sampler2D Sampler0;
varying mediump vec2 vTexCoord0;
uniform mediump float sampler0_bias;
#endif
#ifdef MULTITEXTURED
uniform lowp sampler2D Sampler1;
varying mediump vec2 vTexCoord1;
uniform lowp float envmapIntensity;
uniform mediump float sampler1_bias;
#endif //MULTITEXTURED
#ifdef LIGHTMAP
varying mediump vec2 vTexCoord2;
uniform lowp sampler2D Sampler2;
uniform mediump float sampler2_bias;
uniform lowp vec4 lightmapFactor;
#endif

#ifdef ADJUST
uniform mediump float diffuseAdjustLightness;
uniform mediump float diffuseAdjustContrast;
uniform mediump float diffuseAdjustSaturation;
uniform mediump float lightmapAdjustLightness;
uniform mediump float lightmapAdjustContrast;
uniform mediump float lightmapAdjustSaturation;
uniform mediump float globalAdjustLightness;
uniform mediump float globalAdjustContrast;
uniform mediump float globalAdjustSaturation;
#endif

#ifdef CG_FOG
uniform lowp vec4 fogcolor;
varying lowp float fogFactor;
#endif //CG_FOG

varying lowp vec4 vColor0;

uniform lowp vec3 sunFactorA;

#ifdef USE_ALPHA_TEXTURE
uniform lowp sampler2D Sampler0_alpha;
#endif

lowp vec3 saturate(lowp vec3 color, mediump float saturation)
{
    lowp vec3 ratios = vec3(0.3, 0.59, 0.11);
    lowp vec3 gray = vec3(dot(ratios, color));
    return mix(gray, color, saturation);
}


void main()
{
    lowp vec4 color = vColor0;
#ifdef TEXTURED
#ifdef USE_ALPHA_TEXTURE
    color *= vec4(texture2D(Sampler0, vTexCoord0).rgb,
                texture2D(Sampler0_alpha, vTexCoord0).g);
#else
    color *= texture2D(Sampler0, vTexCoord0);
#endif

    #ifdef ADJUST
    color.rgb = diffuseAdjustLightness + saturate(color.rgb * diffuseAdjustContrast, diffuseAdjustSaturation) ;
    #endif
#endif
#ifdef LIGHTMAP
    lowp vec4 lightmap = texture2D(Sampler2, vTexCoord2);
    #ifdef ADJUST
    lightmap.rgb = lightmapAdjustLightness + saturate(lightmap.rgb * lightmapAdjustContrast, lightmapAdjustSaturation) ;
    #endif
    color *= lightmap * lightmapFactor;
#endif
#ifdef MULTITEXTURED
    color += texture2D(Sampler1, vTexCoord1) * envmapIntensity;
#endif //MULTITEXTURED

#ifdef ALPHATEST
    if (color.a < 0.5)
        discard;
#endif

#ifdef CG_FOG
    color.xyz = mix(color.xyz, fogcolor.xyz, fogFactor);
#ifdef ADDITIVEBLEND
    color.xyz *= fogFactor;
#endif
#endif //CG_FOG

#ifdef WRITEZ_IN_ALPHA_CHANNEL
    // encode z in a channel
    color.a = (gl_FragCoord.z / gl_FragCoord.w);
#endif

#ifndef ADDITIVEBLEND
    color.rgb += sunFactorA;
#endif
#ifdef ADJUST
    color.rgb = globalAdjustLightness + saturate(color.rgb * globalAdjustContrast, globalAdjustSaturation) ;
#endif
    gl_FragColor = color;
}
