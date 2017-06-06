#version 310 es
// skybox.fs
precision lowp float;
in vec3 TexCoords;
out vec4 color;
layout(location=5) uniform samplerCube skybox;
void main()
{
color = texture(skybox, TexCoords);
//color = vec4(TexCoords, 1.0);
}