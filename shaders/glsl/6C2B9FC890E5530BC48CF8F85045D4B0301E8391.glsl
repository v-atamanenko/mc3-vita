#define GLITCH_OPENGLES_2

uniform lowp sampler2D texture0;
uniform lowp sampler2D texture1;
uniform lowp float interpolation;

varying mediump vec2 vCoord0;

void main()
{
    lowp vec4 texColor0 = texture2D(texture0, vCoord0);
    lowp vec4 texColor1 = texture2D(texture1, vCoord0);
    
    gl_FragColor = mix(texColor0, texColor1, interpolation);
}
