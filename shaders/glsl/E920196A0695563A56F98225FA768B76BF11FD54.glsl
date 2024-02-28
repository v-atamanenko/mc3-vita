#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;

uniform   highp   mat4 WorldViewProjectionMatrix;

uniform   mediump vec2 center;
uniform   mediump float scale;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;
varying mediump vec2 vCoord2;
varying mediump vec2 vCoord3;
varying mediump vec2 vCoord4;

varying mediump vec2 v0c;

void main(void)
{
	vCoord0 = TexCoord0;
	v0c = (vCoord0 - center);
	vCoord1 = (1.0 + scale) * v0c + center;
	vCoord2 = (1.0 + 2.0*scale) * v0c + center;
	vCoord3 = (1.0 + 3.0*scale) * v0c + center;
	vCoord4 = (1.0 + 4.0*scale) * v0c + center;
	
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
