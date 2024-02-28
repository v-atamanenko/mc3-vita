#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
attribute highp   vec2 Texcoord0;
varying mediump vec2 vCoord0;

void main(void)
{
	vCoord0 = Texcoord0;
	gl_Position = Vertex;
}
