#version 330

#define PI 3.14159265359

in vec2 vertexTexCoord;

// uniform vec3 cameraPos;
// uniform mat4 vpi;
// uniform vec2 resolution;

out vec4 nearPos;
out vec4 farPos;

void main()
{
    vec2 uv = (gl_.xy - resolution.xy / 2) / resolution.xy * 2;

    nearPos = vpi * vec4(uv, -1.0, 1.0);
    farPos = vpi * vec4(uv, 1.0, 1.0);
}
