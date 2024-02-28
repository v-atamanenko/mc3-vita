uniform sampler2D DiffuseMapSampler;
uniform sampler2D Diffuse2MapSampler;

uniform sampler2D LightMapSampler;
uniform lowp vec4 lightmapFactor;

uniform sampler2D BlendMapSampler;
uniform lowp float threshWidth;

varying mediump vec2 vCoord0;
varying mediump vec2 vBlendEdges;
varying mediump float vBlend;

varying mediump vec2 vCoord1;
uniform lowp vec4 fogcolor;
varying lowp float fogFactor;

uniform lowp vec3 sunFactorA;

void main()
{
    lowp vec4 color = texture2D(DiffuseMapSampler, vCoord0);
    lowp vec4 color2 = texture2D(Diffuse2MapSampler, vCoord0);

#ifdef BLENDMAP
    lowp float blendMap = texture2D(BlendMapSampler, vCoord0).r;
//  lowp float blend = 1.0-smoothstep(vBlendEdges.x,vBlendEdges.y,blendMap);

    lowp float blend = clamp((blendMap - vBlendEdges.x) * vBlendEdges.y, 0.0, 1.0);
    color.rgb = mix(color2.rgb, color.rgb, blend);

#elif defined(STEP)
    lowp float blendMap = texture2D(BlendMapSampler, vCoord0).r;
    lowp float blend = clamp((blendMap - vBlend) * 1000.0, 0.0, 1.0);
    //lowp float blend = step(blendMap, vBlend);
    color.rgb = mix(color.rgb, color2.rgb, blend);
#else
    color.rgb = mix(color.rgb, color2.rgb, vBlend);
#endif



#ifdef LIGHTMAP
    lowp vec4 lightmap = texture2D(LightMapSampler, vCoord1);
    color *= lightmap * lightmapFactor;
#endif

#ifdef CG_FOG
    color.rgb = mix(color.rgb, fogcolor.rgb, fogFactor);
#endif

#ifdef GAMMA
    color.rgb *= color.rgb;
#endif

    color.rgb += sunFactorA;

    gl_FragColor = color;
}
