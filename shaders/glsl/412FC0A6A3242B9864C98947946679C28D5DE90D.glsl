#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;

uniform   highp   mat4 WorldViewProjectionMatrix;
uniform   highp float offsetX;
uniform   highp float offsetY;
uniform   highp float uvScale;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;

void main(void)
{
	mediump vec2 uv = TexCoord0;
	vCoord0 = uv;
	vCoord1 = (uv + vec2(-0.5,-0.5) + vec2(offsetX,offsetY))*uvScale + vec2(0.5,0.5);
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
