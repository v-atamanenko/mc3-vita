#define GLITCH_OPENGLES_2

uniform lowp sampler2D DiffuseSampler;
varying	mediump vec2 vCoord0;
varying lowp   vec4 vColor;

void main()
{
	lowp vec4 diffuseMap = texture2D(DiffuseSampler, vCoord0);
	gl_FragColor = diffuseMap * vColor;
}
