uniform lowp sampler2D texture0;
uniform lowp sampler2D texture1;
uniform lowp float threshold;
uniform lowp float bloomFactor;


varying mediump vec2 vCoordSource;
//varying mediump vec2 vCoordDecimated;

varying mediump vec2 vCoordBlur0;
varying mediump vec2 vCoordBlur1;
varying mediump vec2 vCoordBlur2;
varying mediump vec2 vCoordBlur3;

uniform mediump vec2 texelSize;
uniform mediump vec2 invert;
//uniform mediump float decimatedTexelSize;

uniform mediump float random;

uniform lowp float effectFactor;
varying lowp float vBlurFactor;
//varying lowp float vFlashFactor;
varying lowp float vGammaFactor;
//varying lowp float vInvFlashFactor;
varying lowp vec4 vFlashColor;

lowp vec4 screen(lowp vec4 a, lowp vec4 b)
{
    return b + (a * (1.0-b));
}

void main()
{
    lowp vec4 color = texture2D(texture0, vCoordSource);
#ifdef BLUR
    lowp vec4 blur = vec4(0.0);
    blur += 0.25 * texture2D(texture1, vCoordBlur0);
    blur += 0.25 * texture2D(texture1, vCoordBlur1);
    blur += 0.25 * texture2D(texture1, vCoordBlur2);
    blur += 0.25 * texture2D(texture1, vCoordBlur3);

    // Initial level
    color = mix(color, blur, vBlurFactor);
#endif

    lowp vec4 highColor = color*color;
    highColor *= highColor;
    highColor *= highColor;

    color = vFlashColor + mix(color, highColor, vGammaFactor);



    gl_FragColor = color;//mix(color, highColor, effectFactor);
}
