#define GLITCH_OPENGLES_2

attribute highp vec4 Vertex;
uniform   lowp  vec4 DiffuseColor;
uniform   highp mat4 WorldViewProjectionMatrix;
varying	  lowp  vec4 vColor0;

void main(void)
{
	vColor0 = DiffuseColor;
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
