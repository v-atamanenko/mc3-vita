#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;

uniform   highp   mat4 WorldViewProjectionMatrix;
uniform   highp   float blurOffsetX;
uniform   highp   float blurOffsetY;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;
varying mediump vec2 vCoord2;
varying mediump vec2 vCoord3;
varying mediump vec2 vCoord4;

void main(void)
{
	mediump vec2 uv = TexCoord0;
	vCoord0 = uv;
	vCoord1 = uv + 1.0 * vec2(blurOffsetX, blurOffsetY);
	vCoord2 = uv + 2.0 * vec2(blurOffsetX, blurOffsetY);
	vCoord3 = uv + 3.0 * vec2(blurOffsetX, blurOffsetY);
	vCoord4 = uv + 4.0 * vec2(blurOffsetX, blurOffsetY);
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
