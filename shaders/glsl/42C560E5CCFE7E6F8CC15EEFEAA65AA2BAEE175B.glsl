#define GLITCH_OPENGLES_2

uniform lowp    sampler2D texture0;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;
varying mediump vec2 vCoord2;
varying mediump vec2 vCoord3;
varying mediump vec2 vCoord4;

void main()
{
	lowp vec4 color = 0.2 * texture2D(texture0, vCoord0);
	color += 0.2 * texture2D(texture0, vCoord1);
	color += 0.2 * texture2D(texture0, vCoord2);
	color += 0.2 * texture2D(texture0, vCoord3);
	color += 0.2 * texture2D(texture0, vCoord4);

	gl_FragColor = color;
}

