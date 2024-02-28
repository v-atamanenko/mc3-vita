#define GLITCH_OPENGLES_2

uniform lowp    sampler2D texture1;
uniform lowp    sampler2D texture2;

varying mediump vec2 vCoord0;
varying	lowp vec4 vColor0;

void main()
{
    gl_FragColor = texture2D(texture1, vCoord0) * texture2D(texture2, vCoord0) * vColor0;
}
