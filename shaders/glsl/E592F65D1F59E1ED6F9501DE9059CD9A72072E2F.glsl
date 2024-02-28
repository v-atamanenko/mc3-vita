#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;
attribute mediump vec2 TexCoord1;

uniform highp   mat4 WorldViewProjectionMatrix;
uniform mediump vec2 texelSize;
uniform mediump vec2 decimatedTexelSize;

varying mediump vec2 vCoordSource;
varying mediump vec2 vCoordDecimated;
varying mediump vec2 vCoordBlur0;
varying mediump vec2 vCoordBlur1;
varying mediump vec2 vCoordBlur2;
varying mediump vec2 vCoordBlur3;

void main(void)
{
	vCoordSource = TexCoord0 + vec2(0.5 * texelSize.x,0.5 * texelSize.x);
	mediump vec2 uv = TexCoord1;
	vCoordDecimated = uv;
	mediump vec2 offset = 0.5 * decimatedTexelSize;
	vCoordBlur0 = uv + vec2( offset.x,  offset.y);
	vCoordBlur1 = uv + vec2(-offset.x,  offset.y);
	vCoordBlur2 = uv + vec2( offset.x, -offset.y);
	vCoordBlur3 = uv + vec2(-offset.x, -offset.y);

	gl_Position = WorldViewProjectionMatrix * Vertex;
}
