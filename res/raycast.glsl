#version 330

#define PI 3.14159265359

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform sampler3D voxelData;

uniform vec3 cameraPos;
uniform mat4 vpi;

uniform vec2 resolution;
uniform float time;
uniform int nSteps = 1024;
uniform float dist = 25.0;
uniform float density = 25.0;
uniform int hard = 0;

out vec4 finalColor;

float random(in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float checkerboard(vec2 p, float size) {
    p /= size;
    vec2 f = fract(p.xy) - 0.5;
    return float(f.x * f.y > 0.0 ? 1.0 : 0.0);
}

float centre(vec2 p, float size) {
    p /= size;
    return (length(p) < 1.0 ? 1.0 : 0.0);
}

float ray_cast(in vec3 ro, in vec3 rd)
{
    float stepSize = dist / nSteps;
    vec3 delta = rd * stepSize;

    vec3 pos = ro;
    float value = 0.0;

    for (int i = 0; i < nSteps; ++i)
    {
        vec3 mapPos = (pos / 10); // + vec3(0.5, 0, 0.5);
        //mapPos = round(mapPos * 401) / 401;
        float voxVal = texture(voxelData, mapPos).r;
        float oob = float(length(vec3(lessThan(mapPos, vec3(0.0))) + vec3(greaterThan(mapPos, vec3(1.0)))) == 0);
        voxVal *= stepSize * density * oob;
        value += voxVal;
        pos += delta;
    }
    return value;
}

void main()
{
    const float fov = 1 / atan(PI / 2.0);

    vec2 uv = (gl_FragCoord.xy - resolution.xy / 2) / resolution.xy * 2;
    //uv.x += random(uv + time + 1) * 0.005;
    //uv.y += random(uv + time) * 0.005;

    vec4 near = vpi * vec4(uv, -1.0, 1.0);
    vec4 far = vpi * vec4(uv, 1.0, 1.0);

    vec3 ro = near.xyz / near.w;

    vec3 rd = normalize((far.xyz / far.w) - (near.xyz / near.w));

    float cloud = ray_cast(ro, rd);
    vec3 cloudCol = mix(vec3(0.4, 0.0, 0.4), vec3(1.0, 0.9, 0.3), cloud);

    // float s = (-ro.y) / rd.y;
    // vec3 p = ro + rd * s;
    // vec2 planePos = (ro + rd * s).xz;
    // vec3 plane = vec3(checkerboard(planePos, 0.1) * 0.1);
    // plane += centre(planePos, 0.1);
    // plane *= (s > 0 ? 1.0 : 0.0);
    //plane = vec3(0.0);

    finalColor = vec4(cloudCol, (hard == 1 ? (cloud > 0 ? 1 : 0) : cloud * 2));
}
