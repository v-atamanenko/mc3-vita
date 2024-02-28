#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;

uniform   highp   mat4 WorldViewProjectionMatrix;

varying mediump vec2 vCoord0;

void main(void)
{
	vCoord0 = TexCoord0;
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
