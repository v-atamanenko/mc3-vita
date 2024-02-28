#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;

uniform   highp   mat4 WorldViewProjectionMatrix;

varying mediump vec2 vCoord0;

void main(void)
{
	mediump vec2 uv = TexCoord0;
	vCoord0 = uv;
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
