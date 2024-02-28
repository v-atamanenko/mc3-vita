#define GLITCH_OPENGLES_2

uniform lowp    sampler2D texture;

varying mediump vec2 vCoord0;
varying	lowp    vec4 vColor0;

void main()
{
    lowp vec4 texColor = texture2D(texture, vCoord0);
    gl_FragColor = vec4(texColor.rgb *  vColor0.rgb, texColor.a);
}
