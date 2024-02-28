#define GLITCH_OPENGLES_2


attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;
attribute highp   vec3 Normal;
attribute lowp   vec4 color;

uniform highp   mat4 WorldViewProjectionMatrix;
uniform mediump vec2 uvOffset;

varying	mediump vec2 vCoord0;
varying	lowp vec4 vColor;

void main()
{
	vec4 pos = WorldViewProjectionMatrix * Vertex;
	vCoord0 = TexCoord0.xy + uvOffset;
	vColor = color;
	gl_Position = pos;
}
