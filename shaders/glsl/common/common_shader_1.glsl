uniform lowp sampler2D texture0;
uniform lowp sampler2D texture1;
uniform lowp float threshold;
uniform lowp float bloomFactor;


varying mediump vec2 vCoordSource;
varying mediump vec2 vCoordDecimated;

varying mediump vec2 vCoordBlur0;
varying mediump vec2 vCoordBlur1;
varying mediump vec2 vCoordBlur2;
varying mediump vec2 vCoordBlur3;

uniform mediump vec2 texelSize;
uniform lowp vec2 invert;
uniform mediump float infraLinesFactor;
//uniform mediump float decimatedTexelSize;

uniform mediump float random;

lowp vec4 screen(lowp vec4 a, lowp vec4 b)
{
    return b + (a * (1.0-b));
}



void main()
{
    lowp float color = texture2D(texture0, vCoordSource).r;

    lowp float blur = 0.0;
#ifdef BLUR
    lowp vec4 b = vec4(texture2D(texture1, vCoordBlur0).g,
                          texture2D(texture1, vCoordBlur1).g,
                          texture2D(texture1, vCoordBlur2).g,
                          texture2D(texture1, vCoordBlur3).g);

    blur = dot(vec4(0.25),b);

    lowp float line = 0.5;
#else
    lowp float line = 1.0;
#endif



    // Horizontal lines in screen

    #ifdef LINES
        line = min(0.5,mod(gl_FragCoord.x,2.0)*0.5+infraLinesFactor);
    #endif

    // Initial level
    lowp float level = blur + color * line;

    // Noise
    #ifdef NOISE
        highp vec2 co = vCoordSource;
        highp float r = random;
        highp vec2 seed = co+r;
        highp vec2 d = vec2(12.9898,78.233);
        highp float rand = fract(sin(dot(seed ,d)) * 43758.5453);
        mediump float noise = rand * 0.14 - 0.07;
        level += noise;
    #endif

    // Negative
    //level = invert.x + invert.y * clamp(level, 0.0,1.1);
    #ifdef INVERTED
    level = 1.0 - level;
    #endif

    #ifdef TINT
        lowp vec4 tint = vec4(0.90, 1.00, 0.96,1.0); // Tint

        gl_FragColor = level * tint;
    #else
        gl_FragColor = vec4(level);
    #endif
}
