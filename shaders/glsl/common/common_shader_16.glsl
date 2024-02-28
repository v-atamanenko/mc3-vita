uniform lowp sampler2D DiffuseMapSampler;

#ifdef CG_FOG
uniform lowp vec4 fogcolor;
varying lowp float fogFactor;
#endif //CG_FOG

varying lowp vec4 vColor0;
varying mediump vec2 vTexCoord0;

uniform lowp vec3 sunFactorA;

#define USE_ALPHA_TEXTURE
#ifdef USE_ALPHA_TEXTURE
uniform lowp sampler2D DiffuseMapSampler_alpha;
#endif

void main()
{
    lowp vec4 color = vColor0;
#ifdef USE_ALPHA_TEXTURE
    color *= vec4(texture2D(DiffuseMapSampler, vTexCoord0).rgb,
                    texture2D(DiffuseMapSampler_alpha, vTexCoord0).g);
#else
    color *= texture2D(DiffuseMapSampler, vTexCoord0);
#endif

#ifdef CG_FOG
    color.xyz = mix(color.xyz, fogcolor.xyz, fogFactor);
#endif //CG_FOG

    color.rgb += sunFactorA;

    gl_FragColor = color;
}
