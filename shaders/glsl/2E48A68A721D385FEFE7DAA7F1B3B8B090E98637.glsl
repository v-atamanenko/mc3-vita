#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;
attribute mediump vec2 TexCoord1;

uniform highp   mat4 WorldViewProjectionMatrix;
uniform mediump vec2 texelSize;

varying mediump vec2 vCoordSource;
varying mediump vec2 vCoordDecimated;

void main(void)
{
	vCoordDecimated = TexCoord1;// + vec2(0.5 * texelSize.x,0.5 * texelSize.y);
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
