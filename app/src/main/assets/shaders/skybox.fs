#version 310 es
// skybox.fs
precision lowp float;
//in vec3 TexCoords;
in vec3 localPos;
out vec4 color;

layout(location=5) uniform samplerCube skybox;
void main()
{
//color = texture(skybox, TexCoords);
//color = vec4(TexCoords, 1.0);
    vec3 coord = normalize(localPos);
    color = texture(skybox, coord);
}