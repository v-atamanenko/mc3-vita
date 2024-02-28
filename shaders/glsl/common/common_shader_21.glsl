
attribute highp   vec4 Vertex;

#ifdef CUBE
attribute highp   vec3 Normal;
#endif

attribute mediump vec2 TexCoord0;
attribute mediump vec2 TexCoord1;

uniform highp mat4 WorldViewProjectionMatrix;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;
varying highp vec3 vReflection;

uniform highp mat4 matWorldT;
uniform highp mat4 matWorldI;
uniform highp mat4 matWorldIT;
uniform highp mat4 matViewI;

uniform highp mat4 WorldViewT;
varying lowp float fogFactor;
uniform mediump vec2 fogstartend;

void main()
{
    gl_Position = WorldViewProjectionMatrix * Vertex;
    vCoord0 = TexCoord0;
    vCoord1 = TexCoord1;

#ifdef CUBE
    // Eye direction
    highp vec4 eyePosition = matWorldI * matViewI[3];
    highp vec3 eyeDirection = normalize(eyePosition.xyz - Vertex.xyz);
    eyeDirection = (matWorldIT * vec4(eyeDirection,0)).xyz;

    // Reflection
    vReflection = normalize(reflect(eyeDirection, Normal));
#endif


#ifdef FOG
    float depth = dot(WorldViewT[2], Vertex);
    fogFactor = clamp((depth - fogstartend[0])/(fogstartend[1] - fogstartend[0]), 0.0, 1.0);
#endif

}
