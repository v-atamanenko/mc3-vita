#define GLITCH_OPENGLES_2

attribute highp   vec4 Vertex;
uniform   mediump mat4 WorldViewProjectionMatrix;
void main(void)
{
    gl_Position = WorldViewProjectionMatrix * Vertex;
}
