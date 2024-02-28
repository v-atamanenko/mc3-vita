#define GLITCH_OPENGLES_2

uniform lowp sampler2D texture1;
varying mediump vec2 vCoordDecimated;
void main()
{
	gl_FragColor = texture2D(texture1, vCoordDecimated); 
}
