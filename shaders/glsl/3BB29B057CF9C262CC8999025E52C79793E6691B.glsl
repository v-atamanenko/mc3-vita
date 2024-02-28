#define GLITCH_OPENGLES_2

uniform lowp sampler2D texture0;
uniform lowp sampler2D texture1;

uniform lowp float scale;

varying mediump vec2 vCoord0;

void main()
{
	mediump vec2 normal = texture2D(texture1, vCoord0).xy + vec2(-0.498,-0.498); // NOTE: using 0.498 instead of 0.5 to make sure a value of 0x7F will return 0
	mediump vec2 uv = vCoord0 + normal * vec2(scale,scale);
	gl_FragColor = texture2D(texture0, uv);
}
