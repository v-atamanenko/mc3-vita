#define GLITCH_OPENGLES_2

uniform lowp sampler2D TextureSampler;
uniform lowp sampler2D TextureSampler_alpha;
uniform lowp vec4 DiffuseColor;

varying highp vec2 vTexCoord0;
varying lowp vec4 vColor0;

void main()
{
	lowp vec4 color;
	
	color.a = texture2D(TextureSampler_alpha, vTexCoord0).g * vColor0.a;
	color.rgb = texture2D(TextureSampler, vTexCoord0).rgb * vColor0.rgb * color.a;
	
	gl_FragColor = color;
}