#define GLITCH_OPENGLES_2

uniform lowp    sampler2D texture0;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;
varying mediump vec2 vCoord2;
varying mediump vec2 vCoord3;
varying mediump vec2 vCoord4;

uniform lowp vec3 additiveColor;
uniform lowp vec3 multiplyColor;

uniform mediump float desaturation;

void main()
{
	// radial blur
	lowp vec4 color = 0.2 * texture2D(texture0, vCoord0);
	color += 0.2 * texture2D(texture0, vCoord1);
	color += 0.2 * texture2D(texture0, vCoord2);
	color += 0.2 * texture2D(texture0, vCoord3);
	color += 0.2 * texture2D(texture0, vCoord4);
	
	// desaturate
	mediump float average = 0.33 * (color.r + color.g + color.b);
	color.rgb = mix(color.rgb, vec3(average,average,average), desaturation);
	
	// adjust colors
	color.rgb *= multiplyColor;
	color.rgb += additiveColor;
	
	//
	gl_FragColor = color;
}

