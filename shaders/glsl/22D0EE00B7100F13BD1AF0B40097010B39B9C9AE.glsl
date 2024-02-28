#define GLITCH_OPENGLES_2

uniform lowp sampler2D texture0;

varying mediump vec2 vCoord0;

void main()
{
	gl_FragColor = texture2D(texture0, vCoord0);
}
