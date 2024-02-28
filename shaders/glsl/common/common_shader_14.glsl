attribute highp   vec4 Vertex;
attribute mediump vec3 Color;
attribute mediump vec2 TexCoord0;
attribute mediump vec2 TexCoord1;

uniform highp vec3 light0position;
uniform highp mat4 WorldViewProjectionMatrix;
uniform highp mat4 matWorldIT;
uniform highp mat4 matworldviewit;
uniform highp mat4 WorldViewT;


varying mediump vec2 vCoord0;
varying mediump vec2 vBlendEdges;
uniform mediump float threshWidth;
varying mediump float vBlend;

varying mediump vec2 vCoord1;

uniform highp mat4 matWorldT;
varying lowp float fogFactor;
uniform mediump vec2 fogstartend;

void main()
{
    gl_Position = WorldViewProjectionMatrix * Vertex;
    vCoord0 = TexCoord0;

#ifdef LIGHTMAP
    vCoord1 = TexCoord1;
#endif

#ifdef BLENDMAP
    vBlendEdges.x = Color.r - threshWidth*0.5;
    vBlendEdges.y = 1.0 / threshWidth;
#else
    vBlend = Color.r;
#endif

#ifdef CG_FOG
    float depth = dot(WorldViewT[2], Vertex);
    fogFactor = clamp((-depth - fogstartend[0])/(fogstartend[1] - fogstartend[0]), 0.0, 1.0);
#endif
}
