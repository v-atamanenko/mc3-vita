#ifdef NORMALMAP
    #define SPECULAR
#endif


attribute highp   vec4 Vertex;
attribute highp vec2 TexCoord0;
attribute highp   vec3 Normal;
attribute highp   vec3 Tangent;
attribute highp   vec3 Binormal;

uniform highp vec3 light0position;
uniform highp mat4 WorldViewProjectionMatrix;
uniform highp mat4 matWorld;
uniform highp mat4 matWorldIT;
uniform highp mat4 matWorldViewIT;
uniform highp mat4 matWorldViewI;
uniform highp mat4 TextureMatrix0;
uniform highp mat4 matWorldT;
uniform highp mat4 matViewI;
uniform highp mat4 matWorldI;
uniform highp mat4 WorldViewT;

varying highp vec2 vCoord0;
varying lowp vec3 vDiffuse;
varying highp vec3 vNormal;

// Irradiance
uniform highp vec4 cAr;
uniform highp vec4 cAg;
uniform highp vec4 cAb;
uniform highp vec4 cBr;
uniform highp vec4 cBg;
uniform highp vec4 cBb;
uniform highp vec3 cC;

// Fog
varying lowp float fogFactor;
uniform highp vec2 fogstartend;

// Lights
uniform highp vec3 pointLightPosition;
uniform highp vec3 pointLightIntensity;

// Specular
uniform highp vec3 specDirection;
varying lowp vec3 vHalf;
varying mediump vec3 vSpecIntensity;
uniform mediump vec3 specIntensity;
uniform mediump float specAmount;

varying highp vec2 vRimCoord;

// Reflection
varying highp vec3 vEyeDirection;
varying highp vec3 vReflection;
uniform mediump float reflectionAmount;
varying mediump float vReflectionAmount;

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

highp vec3 pointLight(highp vec3 position, highp vec3 normal)
{
    highp vec3 delta = pointLightPosition - position;
    highp float distance2 = dot(delta, delta) * 0.0001;
    highp vec3 intensity = pointLightIntensity / distance2;
    return max(dot(normal, normalize(delta)),0.0) * intensity;
}

void main()
{

    gl_Position = WorldViewProjectionMatrix * Vertex;
    highp vec3 position = (matWorld * Vertex).xyz;
    vCoord0 = (TextureMatrix0 * vec4(TexCoord0.xy, 1.0, 0.0)).xy;
    highp vec3 normal = (matWorldIT * vec4(Normal,0.0)).xyz;
    vDiffuse = irradiance(normal);
#ifdef POINTLIGHT
    vDiffuse += pointLight(position, normal);
#endif

#ifdef NORMALMAP
    //mediump mat3 objectToTangent = mat3(0, 0, 0, 0, 0, 0, 0, 0, 0);
    highp mat3 objectToTangent = mat3(Tangent.x, Binormal.x, Normal.x, Tangent.y, Binormal.y, Normal.y, Tangent.z, Binormal.z, Normal.z);
#else
    vNormal = normal;
#endif

#if defined(SPECULAR) || defined(CUBE)
    highp vec4 eyePosition = matWorldI * matViewI[3];
    highp vec3 eyeDirection = normalize(eyePosition.xyz - Vertex.xyz);
    #if defined(CUBE)
        vEyeDirection = eyeDirection;
        vReflectionAmount = dot(vDiffuse,vec3(0.3,0.6,0.1)) * reflectionAmount;
        #ifndef NORMALMAP
            vReflection = reflect(vEyeDirection, normal);
        #endif
    #else
        vEyeDirection = objectToTangent * vEyeDirection;
    #endif


#endif

#ifdef SPECULAR

    highp vec3 spec = (matWorldT * vec4(specDirection,0.0)).xyz;
    //eyeDirection = (matWorld * vec4(eyeDirection,1.0)).xyz;
    vHalf = normalize(eyeDirection + spec);

#ifdef NORMALMAP
    vHalf = objectToTangent * vHalf;
    //vHalf = normalize(vHalf);
#endif

    vSpecIntensity = specIntensity * specAmount;
#endif


#ifdef CG_FOG
    float depth = dot(WorldViewT[2], Vertex);
    fogFactor = clamp((depth - fogstartend[0])/(fogstartend[1] - fogstartend[0]), 0.0, 1.0);
#endif


#ifdef RIM
    vec2 rimNormal = (matWorldViewIT * vec4(Normal, 0)).xy;
    vRimCoord.x = rimNormal.x * 0.5 + 0.5;
    vRimCoord.y = 0.5 - rimNormal.y * 0.5;
#endif
}
