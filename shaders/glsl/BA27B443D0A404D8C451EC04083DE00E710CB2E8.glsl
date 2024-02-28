#define GLITCH_OPENGLES_2

uniform lowp    sampler2D texture0;

varying mediump vec2 vCoord1;
varying mediump vec2 vCoord2;
varying mediump vec2 vCoord3;

void main()
{
	mediump vec4 b0 = texture2D(texture0, vCoord1);
	mediump vec4 b1 = texture2D(texture0, vCoord2);
	mediump vec4 b2 = texture2D(texture0, vCoord3);

	gl_FragColor.r = b0.r;
	gl_FragColor.g = b1.g;
	gl_FragColor.b = b2.b;
}
