#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;
attribute highp   vec4 color;
uniform   highp   mat4 WorldViewProjectionMatrix;
varying   mediump vec2 vCoord0;
varying   lowp   vec4 vColor;
uniform   highp  mat4 texturemat0;

void main()
{
	gl_Position    = WorldViewProjectionMatrix * Vertex;
	vCoord0        = TexCoord0;
	vColor         = color;
}
