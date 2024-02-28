#if defined(LIGHTMAP) || defined(CG_FOG)
    #define DIFFUSE
#endif

#define IRRADIANCE

// Input
attribute highp   vec4 Vertex;
attribute highp   vec3 Normal;
attribute mediump vec2 TexCoord0;
attribute mediump vec2 TexCoord1;
attribute mediump vec2 TexCoord2;

// Matrices
uniform highp mat4 WorldViewProjectionMatrix;
uniform highp mat4 WorldViewT;

// Texture coords
varying mediump vec2 vCoordDiffuse;
varying mediump vec2 vCoordLightMap;
varying mediump vec2 vCoordReflectionMap;


// Reflection
varying mediump vec4 vDebug;
uniform highp mat4 matWorldIT;
uniform highp mat4 matviewi;
uniform highp mat4 matworldi;
uniform highp mat4 matWorldT;
varying highp vec3 eyeDirection;
uniform mediump float fresnelPower;
uniform mediump vec4 minReflectionAmount;
uniform mediump vec4 maxReflectionAmount;
varying highp vec3 vReflection;
varying lowp vec3 vReflectionAmount;


// Fog
varying lowp float fogFactor;
uniform mediump vec2 fogstartend;

// Irradiance

uniform highp vec4 cAr;
uniform highp vec4 cAg;
uniform highp vec4 cAb;
uniform highp vec4 cBr;
uniform highp vec4 cBg;
uniform highp vec4 cBb;
uniform highp vec3 cC;
uniform mediump float minIrradiance;

highp vec3 irradiance(highp vec3 normal3)
{
    highp vec4 normal = vec4(normal3, 1.0);
    normal.z = -normal.z;

    highp vec3 x1, x2, x3;

    // Linear + constant polynomial terms
    x1.r = dot(cAr,normal);
    x1.g = dot(cAg,normal);
    x1.b = dot(cAb,normal);

    // 4 of the quadratic polynomials
    highp vec4 vB = normal.xyzz * normal.yzzx;
    x2.r = dot(cBr,vB);
    x2.g = dot(cBg,vB);
    x2.b = dot(cBb,vB);

    // Final quadratic polynomial
    highp float vC = normal.x*normal.x - normal.y*normal.y;
    x3 = cC.rgb * vC;

    return x1 + x2 + x3;
}



void main()
{
    gl_Position = WorldViewProjectionMatrix * Vertex;
#ifdef DIFFUSE
    vCoordDiffuse = TexCoord0;
#endif
#ifdef LIGHTMAP
    vCoordLightMap = TexCoord1;
#endif
#ifdef REFLECTIONMAP
    vCoordReflectionMap = TexCoord2;
#endif

    vec3 normal = (matWorldIT * vec4(Normal,0)).xyz;

    // Eye direction
    highp vec4 eyePosition = matworldi * matviewi[3];
    highp vec3 eyeDirection = normalize(eyePosition.xyz - Vertex.xyz);
    eyeDirection = (matWorldIT * vec4(eyeDirection,0)).xyz;

    // Reflection
    vReflection = normalize(reflect(eyeDirection, normal));

    // Amount
    float fresnel = 1.0-clamp(-dot(vReflection, normal),0.0,1.0);
    fresnel = pow(fresnel, fresnelPower);
    vec4 reflectionAmount = mix(minReflectionAmount, maxReflectionAmount, fresnel);
#ifdef IRRADIANCE
    reflectionAmount.a *= clamp(dot(irradiance(normal),vec3(0.3,0.6,0.1)),minIrradiance,1.0);
#endif
    vReflectionAmount = reflectionAmount.rgb * reflectionAmount.a;

#ifdef CG_FOG
    float depth = dot(WorldViewT[2], Vertex);
    fogFactor = clamp((depth - fogstartend[0])/(fogstartend[1] - fogstartend[0]), 0.0, 1.0);
#endif

}
