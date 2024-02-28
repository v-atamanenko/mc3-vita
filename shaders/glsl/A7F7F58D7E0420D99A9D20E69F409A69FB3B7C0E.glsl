#define GLITCH_OPENGLES_2

uniform lowp    sampler2D texture0;

varying mediump vec2 vCoord00;
varying mediump vec2 vCoord01;
varying mediump vec2 vCoord02;
varying mediump vec2 vCoord10;
varying mediump vec2 vCoord11;
varying mediump vec2 vCoord12;
varying mediump vec2 vCoord20;
varying mediump vec2 vCoord21;
varying mediump vec2 vCoord22;

uniform   lowp float offset;
uniform   lowp vec3 weight0;
uniform   lowp vec3 weight1;
uniform   lowp vec3 weight2;

void main()
{
	lowp vec4 color = weight0.x * texture2D(texture0, vCoord00);
	color += weight0.y * texture2D(texture0, vCoord01);
	color += weight0.z * texture2D(texture0, vCoord02);
	
	color += weight1.x * texture2D(texture0, vCoord10);
	color += weight1.y * texture2D(texture0, vCoord11);
	color += weight1.z * texture2D(texture0, vCoord12);
	
	color += weight2.x * texture2D(texture0, vCoord20);
	color += weight2.y * texture2D(texture0, vCoord21);
	color += weight2.z * texture2D(texture0, vCoord22);

	gl_FragColor = color + offset;
}

