#define GLITCH_OPENGLES_2

attribute highp vec4 Position;
attribute lowp vec4 Color0;
attribute mediump vec2 TexCoord0;

uniform mediump mat4 TextureMatrix0;
uniform highp mat4 WorldViewProjectionMatrix;
uniform mediump vec4 DiffuseColor;

#ifdef CG_FOG
uniform highp mat4 WorldViewT;
varying lowp float fogFactor;
uniform mediump vec2 fogstartend;
#endif //CG_FOG

varying mediump vec2 vTexCoord0;
varying lowp vec4 vColor0;

void main(void)
{
	gl_Position = WorldViewProjectionMatrix * Position;
	vTexCoord0 = (TextureMatrix0 * vec4(TexCoord0, 1, 0)).xy;
	vColor0 = Color0;

#ifdef CG_FOG
	float depth = dot(WorldViewT[2], Position);
	fogFactor = clamp((-depth - fogstartend[0])/(fogstartend[1] - fogstartend[0]), 0.0, 1.0);
#endif //CG_FOG

}
