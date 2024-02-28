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
	mediump vec4 c0 = texture2D(texture0, vCoord0);
	mediump vec4 c1 = texture2D(texture0, vCoord1);
	mediump vec4 c2 = texture2D(texture0, vCoord2);
	mediump vec4 c3 = texture2D(texture0, vCoord3);
	mediump vec4 c4 = texture2D(texture0, vCoord4);
	
	mediump float ca = (c0.a + c1.a + c2.a + c3.a + c4.a);
	mediump float invca = 1.0 / ca;
	
	mediump vec4 color;
	color.xyz = c0.a * c0.xyz;
	color.xyz += c1.a * c1.xyz;
	color.xyz += c2.a * c2.xyz;
	color.xyz += c3.a * c3.xyz;
	color.xyz += c4.a * c4.xyz;
	
	color.xyz *= invca;
	
	color.a = 0.2 * ca;
	
	gl_FragColor = color;
}
