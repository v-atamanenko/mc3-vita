#define GLITCH_OPENGLES_2

uniform lowp sampler2D texture0;
uniform lowp sampler2D texture1;
uniform lowp float threshold;
uniform lowp float bloomFactor;


varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;
uniform mediump float texelSize;
uniform mediump float resizedTexelSize;

lowp vec4 screen(lowp vec4 a, lowp vec4 b)
{
	return b + (a * (1.0-b));
}

void main()
{

	
	lowp vec4 color = texture2D(texture0, vCoord0);
	lowp vec4 bloom = texture2D(texture1, vCoord1);
	//bloom *= bloom;
	//bloom *= bloom;
	//bloom *= bloom;
	
	color = screen(color, bloom*1.1);
	
	gl_FragColor = color;
}
