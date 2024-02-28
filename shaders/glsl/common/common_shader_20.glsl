// Input
attribute highp   vec4 Vertex;
attribute highp   vec3 Normal;
attribute highp   vec3 Tangent;
attribute highp   vec3 Binormal;
attribute mediump vec2 TexCoord0;
#ifdef ALPHA
attribute lowp   vec4 color;
#endif

// Matrices
uniform highp mat4 WorldViewProjectionMatrix;
uniform highp mat4 matWorldView;
uniform highp mat4 WorldViewT;

// Texture coords
varying mediump vec2 vCoord;


// Reflection
uniform highp mat4 matWorldIT;
uniform highp mat4 matViewI;
uniform highp mat4 matWorld;
uniform highp mat4 matWorldI;
uniform highp mat4 matWorldT;
uniform mediump float fresnelPower;
uniform mediump vec4 minReflectionAmount;
uniform mediump vec4 maxReflectionAmount;

varying mediump vec4 vDebug;
varying highp vec3 vEyeDirection;
varying highp vec4 vEyePosition;

// Fog
varying lowp float vFogFactor;
uniform mediump vec2 fogstartend;
varying highp vec4 vPosition;

uniform lowp vec3 downColor;
uniform lowp vec3 horizonColor;
uniform lowp float downAlpha;
uniform lowp float horizonAlpha;
varying lowp vec4 vBaseColor;
uniform   mediump vec2 uvOffset;


void main()
{
    vPosition = matWorld * Vertex;

    gl_Position = WorldViewProjectionMatrix * Vertex;

    vCoord = TexCoord0.xy + uvOffset;

    // Eye position
    //highp vec4 eyePosition = matWorldI * matViewI[3];
    vEyeDirection = normalize(matViewI[3].xyz - vPosition.xyz);
    //vEyePosition = matViewI[3];
    vEyeDirection.y = abs(vEyeDirection.y);
    mediump float baseFactor = vEyeDirection.y;
    baseFactor = pow(baseFactor,0.25);
    vBaseColor.rgb = mix(horizonColor, downColor, baseFactor);

#ifdef ALPHA
    vBaseColor.a = mix(horizonAlpha, downAlpha, baseFactor);
    vBaseColor.a *= color.a;
#else
    vBaseColor.a = 1.0;
#endif
    // Base color


    // Tangent space conversions
//  highp mat3 objectToTangent = mat3(Tangent.x, Binormal.x, Normal.x, Tangent.y, Binormal.y, Normal.y, Tangent.z, Binormal.z, Normal.z);
    //yeDirection = eyeDirection.xzy;//normalize(objectToTangent * eyeDirection);

#ifdef FOG
    float depth = dot(WorldViewT[2], Vertex);
    vFogFactor = clamp((-depth - fogstartend[0])/(fogstartend[1] - fogstartend[0]), 0.0, 1.0);
#endif

}
