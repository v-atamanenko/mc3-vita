#define GLITCH_OPENGLES_2

uniform lowp sampler2D texture0;
uniform lowp sampler2D texture1;

uniform lowp float scale;

varying mediump vec2 vCoord0;
varying mediump vec2 vCoord1;

void main()
{
	mediump vec2 normal = texture2D(texture1, vCoord1).xy + vec2(-0.5,-0.5);
	mediump vec2 uv = vCoord0 + normal * vec2(scale,scale);
	gl_FragColor = texture2D(texture0, uv);
}
