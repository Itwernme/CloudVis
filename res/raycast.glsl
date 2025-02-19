#version 330

#define PI 3.14159265359

in vec2 fragTexCoord;
in vec4 fragColor;

//in vec4 nearPos;
//in vec4 farPos;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform sampler3D voxelData;

uniform vec3 cameraPos;
uniform mat4 vpi;

uniform vec2 renderResolution;
uniform int nSteps = 1024;
uniform float dist = 30.0;
uniform float density = 25.0;
uniform bool hard = true;

out vec4 finalColor;

vec2 intersectAABB(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax) {
    vec3 tMin = (boxMin - rayOrigin) / rayDir;
    vec3 tMax = (boxMax - rayOrigin) / rayDir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
};

float random(in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float ray_cast(in vec3 ro, in vec3 rd, in vec2 bounds)
{
    if (bounds.x > bounds.y) return 0.0;

    float stepSize = dist / nSteps;
    vec3 delta = rd * stepSize;

    float t = bounds.x;
    vec3 pos = ro + rd * t;
    float value = 0.0;

    for (int i = 0; i < nSteps && t < bounds.y; ++i)
    {
        vec3 mapPos = (pos / 10);
        vec3 mapPosInv = mapPos * vec3(1, -1, 1) + vec3(0, 1.0 / 401, 0);
        float voxVal = mapPos.y > 0 ? texture(voxelData, mapPos).r : texture(voxelData, mapPosInv).r;
        voxVal *= stepSize * density;
        value += voxVal;
        pos += delta;
        t += stepSize;
    }
    return value;
}

void main()
{
    vec2 uv = (gl_FragCoord.xy - renderResolution.xy / 2) / renderResolution.xy * 2;

    vec4 near = vpi * vec4(uv, -1.0, 1.0);
    vec4 far = vpi * vec4(uv, 1.0, 1.0);

    vec3 ro = near.xyz / near.w;
    vec3 rd = normalize((far.xyz / far.w) - (near.xyz / near.w));

    vec2 bounds = intersectAABB(ro, rd, vec3(0, 0, 0), vec3(10, 10, 10));

    float cloud = ray_cast(ro, rd, bounds);
    vec3 cloudCol = mix(vec3(0.4, 0.0, 0.4), vec3(1.0, 0.9, 0.3), cloud);

    finalColor = vec4(cloudCol, (hard ? (cloud > 0 ? 1 : 0) : cloud * 2));
}
