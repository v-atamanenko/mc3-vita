#define GLITCH_OPENGLES_2

attribute highp vec4 Position;
attribute lowp vec4 Color0;

attribute highp vec2 TexCoord0;
varying highp vec2 vTexCoord0;

uniform highp mat4 WorldViewProjectionMatrix;

//uniform mediump vec4 DiffuseColor;

varying lowp vec4 vColor0;

void main(void)
{
	gl_Position = WorldViewProjectionMatrix * Position;

	vColor0 = Color0;
	
	//vColor0 *= DiffuseColor;
	vTexCoord0 = TexCoord0;
}
