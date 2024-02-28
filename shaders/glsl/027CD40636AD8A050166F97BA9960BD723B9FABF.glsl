#define GLITCH_OPENGLES_2

uniform lowp sampler2D texture0;
varying mediump vec2 vCoord0;
uniform lowp float threshold;

void main()
{
	lowp vec4 color = texture2D(texture0, vCoord0);
	
	lowp float average = 0.33 * (color.r + color.g + color.b) - threshold;
	
	color.r = max(0.0, average);
	color.g = color.r;
	color.b = color.r;
	
	gl_FragColor = color;
}
