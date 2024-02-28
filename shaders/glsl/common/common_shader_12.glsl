attribute highp vec4 Position;

attribute lowp vec4 Color0;

#ifdef TEXTURED
attribute mediump vec2 TexCoord0;
varying mediump vec2 vTexCoord0;
uniform mediump mat4 TextureMatrix0;
#endif
#ifdef MULTITEXTURED
varying mediump vec2 vTexCoord1;
uniform highp mat4 matWorldViewIT;
#endif //MULTITEXTURED
#ifdef LIGHTMAP
varying mediump vec2 vTexCoord2;
uniform mediump mat4 TextureMatrix2;
#    ifdef TEXTURED
#    define LightMapCoord TexCoord1
attribute mediump vec2 TexCoord1;
#    else
#    define LightMapCoord TexCoord0
attribute mediump vec2 TexCoord0;
#    endif
#endif

uniform highp mat4 WorldViewProjectionMatrix;

#ifdef CG_FOG
uniform highp mat4 WorldViewT;
varying lowp float fogFactor;
uniform mediump vec2 fogstartend;
#endif //CG_FOG

#if defined LIGHTING || defined MULTITEXTURED
attribute highp vec3 Normal;
#endif

#if defined LIGHTING || defined MULTITEXTURED
uniform highp mat4 matWorldIT;
#endif

#ifdef LIGHTING
// Light0
uniform highp vec4 Light0Position;
uniform vec4 Light0Diffusecolor;

// MaterialColor
uniform vec4 SceneAmbientLight;
#endif //LIGHTING

varying lowp vec4 vColor0;

void main(void)
{
    gl_Position = WorldViewProjectionMatrix * Position;

#if defined LIGHTING || defined MULTITEXTURED
    vec3 transformedNormal = (matWorldIT * vec4(Normal, 0.0)).xyz;
#endif
#ifdef CG_FOG
    float depth = dot(WorldViewT[2], Position);
#endif

#ifdef TEXTURED
    vTexCoord0 = (TextureMatrix0 * vec4(TexCoord0, 1, 0)).xy;
#endif
#ifdef MULTITEXTURED
    vec2 n = (matWorldViewIT * vec4(Normal, 1)).xy;
    vTexCoord1.x = n.x * 0.5 + 0.5;
    vTexCoord1.y = 0.5 - n.y * 0.5;
#endif
#ifdef LIGHTMAP
    vTexCoord2 = (TextureMatrix2 * vec4(LightMapCoord, 1, 0)).xy;
#endif

#ifdef CG_FOG
    fogFactor = clamp((-depth - fogstartend[0])/(fogstartend[1] - fogstartend[0]), 0.0, 1.0);
#endif

#ifdef LIGHTING
    // light 0 (directional light)
    vec3 VP = Light0Position.xyz;
    float nDotVP = max(0.0, dot(transformedNormal, VP));
    vec4 Diffuse = Light0Diffusecolor * nDotVP;
    vColor0 = SceneAmbientLight + Diffuse;
#else
    vColor0 = Color0;
#endif
}
