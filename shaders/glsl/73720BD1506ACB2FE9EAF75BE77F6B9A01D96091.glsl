#define GLITCH_OPENGLES_2

uniform lowp    sampler2D texture0;
uniform lowp    sampler2D depthTexture;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;
varying mediump vec2 vCoord2;
varying mediump vec2 vCoord3;
varying mediump vec2 vCoord4;

uniform   mediump float focus;
uniform   mediump float scale;

void main()
{
	mediump float depth = texture2D(depthTexture, vCoord0).r;
	mediump float z = abs(depth - focus) * scale;

	mediump float w1 = 0.2 * z;
	mediump float w0 = (1.0 - 4.0 * w1);
	
	mediump vec4 color = w0 * texture2D(texture0, vCoord0);
	color += w1 * texture2D(texture0, vCoord1);
	color += w1 * texture2D(texture0, vCoord2);
	color += w1 * texture2D(texture0, vCoord3);
	color += w1 * texture2D(texture0, vCoord4);
	
	gl_FragColor = color;
}
