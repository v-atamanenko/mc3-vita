uniform lowp sampler2D DiffuseMapSampler;
uniform lowp sampler2D GeneratedWaterNormals;
uniform lowp vec4 DiffuseColor;

varying mediump vec2 vCoord0;
varying lowp vec4 vColor;

void main()
{
#ifdef WATER
    lowp vec4 color = texture2D(GeneratedWaterNormals, vCoord0) * DiffuseColor;
    color.a = vColor.a;
    gl_FragColor = color;
#else
    lowp vec4 color = texture2D(DiffuseMapSampler, vCoord0);
    color.a = vColor.a;
    gl_FragColor = color;
#endif
}
