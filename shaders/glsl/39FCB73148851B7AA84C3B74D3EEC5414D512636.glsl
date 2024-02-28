#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute lowp    vec4 Color0;

uniform   highp   mat4 WorldViewProjectionMatrix;

varying	  lowp    vec4 vColor0;

void main(void)
{
	vColor0 = Color0;
	gl_Position = WorldViewProjectionMatrix * Vertex;
}
