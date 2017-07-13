#version 310 es
precision lowp float;

//layout (location = 0) in vec3 position;
//out vec3 TexCoords;

out vec3 localPos;

uniform mat4 projectionInverse;
uniform mat4 viewInverse;

void main()
{
//    gl_Position =   projection * view * vec4(position, 1.0);
//    TexCoords = position;

// A different approach
    const vec4 UNIT_QUAD[4] = vec4[4](
        vec4(-1.0, -1.0, 0.0, 1.0),
        vec4(1.0, -1.0, 0.0, 1.0),
        vec4(-1.0, 1.0, 0.0, 1.0),
        vec4(1.0, 1.0, 0.0, 1.0)
    );
    vec4 inPosition = UNIT_QUAD[gl_VertexID]; // clip space
    // vec3 clipDir = inPosition.xyz;
    vec3 eyeDir = vec3(projectionInverse * inPosition);

    localPos = vec3(viewInverse * vec4(eyeDir.xyz, 0.0));

    gl_Position = inPosition;

    // clipPos = proj * view * localPos;
//    invProj * clipPos = view * localPos;
//    invView * invProj * clipPos = localPos;

// TRANSLATES FROM CLIP SPACE TO LOCAL SPACE. THEN IT TAKES THAT LOCAL SPACE AS A DIRECTION TO SAMPLE THE CUBEMAP

}