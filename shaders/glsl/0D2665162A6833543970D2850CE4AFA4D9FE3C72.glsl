#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;

uniform   highp   mat4 WorldViewProjectionMatrix;
uniform   highp   float offsetX;
uniform   highp   float offsetY;

varying mediump vec2 vCoord1;
varying mediump vec2 vCoord2;
varying mediump vec2 vCoord3;

void main(void)
{
	mediump vec2 uv = TexCoord0;
	vCoord1 = uv + vec2(offsetX, 0);
	vCoord2 = uv + vec2(-offsetX, 0);
	vCoord3 = uv + vec2(0, offsetY);
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
