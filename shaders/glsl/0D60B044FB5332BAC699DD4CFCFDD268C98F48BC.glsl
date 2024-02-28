#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;

uniform highp   mat4 WorldViewProjectionMatrix;
uniform mediump float texelSize;
uniform mediump float resizedTexelSize;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;

void main(void)
{
	vCoord0 = TexCoord0 + vec2(0.5 * texelSize,0.5 * texelSize);
	vCoord1 = TexCoord0;// + vec2(0.5 * resizedTexelSize,0.5 * resizedTexelSize);

	gl_Position = WorldViewProjectionMatrix * Vertex;
}
