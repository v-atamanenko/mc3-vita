#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute mediump vec2 TexCoord0;

uniform   highp   mat4 WorldViewProjectionMatrix;

uniform   highp   float offsetX;
uniform   highp   float offsetY;

varying mediump vec2 vCoord00;
varying mediump vec2 vCoord01;
varying mediump vec2 vCoord02;
varying mediump vec2 vCoord10;
varying mediump vec2 vCoord11;
varying mediump vec2 vCoord12;
varying mediump vec2 vCoord20;
varying mediump vec2 vCoord21;
varying mediump vec2 vCoord22;

void main(void)
{
	vCoord11 = TexCoord0;
	
	vCoord00 = vCoord11 + vec2(-offsetX,-offsetY);
	vCoord01 = vCoord11 + vec2(0,-offsetY);
	vCoord02 = vCoord11 + vec2(offsetX,-offsetY);

	vCoord10 = vCoord11 + vec2(-offsetX,0);
	//vCoord11 = vCoord11 + vec2(0,0);
	vCoord12 = vCoord11 + vec2(offsetX,0);

	vCoord20 = vCoord11 + vec2(-offsetX,offsetY);
	vCoord21 = vCoord11 + vec2(0,offsetY);
	vCoord22 = vCoord11 + vec2(offsetX,offsetY);
	
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
