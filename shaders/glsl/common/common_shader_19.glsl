uniform samplerCube CubeMapSampler;
uniform sampler2D GeneratedWaterNormals;


varying mediump vec2 vCoord;
varying mediump vec4 vDebug;
varying highp vec3 vEyeDirection;

uniform lowp vec4 fogcolor;
varying lowp float vFogFactor;

uniform lowp vec3 sunFactorA;

uniform lowp vec3 downColor;
uniform lowp vec3 horizonColor;
varying highp vec4 vPosition;
varying highp vec4 vEyePosition;
varying lowp vec4 vBaseColor;

void main()
{
    highp vec3 eyeDirection = vEyeDirection;//normalize(vEyePosition.xyz - vPosition.xyz);

    lowp vec4 color = vBaseColor;

    //normal.z=-normal.z;
    #ifdef LOW_WATER
    mediump vec3 normal = texture2D(GeneratedWaterNormals, vCoord*1.6).xzy * vec3(2.0,2.0,-2.0) - vec3(1.0,1.0,-1.0);

        mediump vec3 reflection = eyeDirection - normal;

        mediump float reflectionAmount = normal.x + normal.y + normal.z;
        mediump float d = max(0.7,normal.x + normal.y + normal.z);
        lowp vec4 reflectedColor = vec4(vec3(d*0.8,d*0.9,d),1.0);

        color *= reflectedColor;
    #else
    mediump vec3 normal = texture2D(GeneratedWaterNormals, vCoord).xzy * vec3(2.0,2.0,-2.0) - vec3(1.0,1.0,-1.0);
        mediump vec3 reflection = reflect(eyeDirection, normal);

        mediump float reflectionAmount = min(1.0 - dot(eyeDirection, normal), 0.90);
        reflectionAmount *= reflectionAmount;
        reflectionAmount *= reflectionAmount;
        lowp vec4 reflectedColor = textureCube(CubeMapSampler, reflection);
        reflectedColor *= reflectionAmount;
    color += reflectedColor;
    #endif







#ifdef FOG
    color.xyz = mix(color.xyz, fogcolor.xyz, vFogFactor);
#endif

    color.rgb += sunFactorA;

    gl_FragColor = color;
}
