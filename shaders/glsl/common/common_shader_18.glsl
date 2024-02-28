uniform lowp sampler2D Sampler0;
uniform lowp vec4 DiffuseColor;
varying mediump vec2 vCoord0;
varying lowp   vec4 vColor;

#define USE_ALPHA_TEXTURE

#ifdef USE_ALPHA_TEXTURE
uniform lowp sampler2D Sampler0_alpha;
#endif

void main()
{
#ifdef USE_ALPHA_TEXTURE
    lowp vec4 color = vec4(texture2D(Sampler0, vCoord0).rgb,
                texture2D(Sampler0_alpha, vCoord0).g);
#else
    lowp vec4 color = texture2D(Sampler0, vCoord0);
#endif

#ifdef DIFFUSECOLOR
    color *= DiffuseColor;
#endif
    color *= vColor;
#ifdef DIFFUSECOLOR
    gl_FragColor = color * 0.001 + vec4(1,0,0,1);
#endif
    gl_FragColor = color;

}
