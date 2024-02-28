#define GLITCH_OPENGLES_2

uniform lowp    sampler2D texture0;

uniform mediump float scale;
varying mediump vec2 vCoord;

void main()
{
	mediump vec4 color = texture2D(texture0, vCoord);
	mediump float average = 0.33 * (color.r + color.g + color.b);
	
	gl_FragColor = mix(color, vec4(average,average,average,average), scale); 
}
