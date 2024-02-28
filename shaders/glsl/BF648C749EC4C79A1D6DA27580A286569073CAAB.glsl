#define GLITCH_OPENGLES_2

uniform lowp    sampler2D texture0;
uniform lowp    float scale;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;
varying mediump vec2 vCoord2;
varying mediump vec2 vCoord3;
varying mediump vec2 vCoord4;

void main()
{
	lowp vec4 color = scale * texture2D(texture0, vCoord0);
	color += scale * texture2D(texture0, vCoord1);
	color += scale * texture2D(texture0, vCoord2);
	color += scale * texture2D(texture0, vCoord3);
	color += scale * texture2D(texture0, vCoord4);
	gl_FragColor = color;
}
