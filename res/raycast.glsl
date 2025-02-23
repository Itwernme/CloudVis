#version 330

#define PI 3.14159265359

uniform sampler3D voxelData;
uniform int voxelRes;

uniform vec3 cameraPos;
uniform mat4 vpi;
uniform vec2 renderRes;

uniform int nSteps;
uniform float dist;
uniform float density;
uniform bool hardEdges;
uniform bool hardCubes;

out vec4 finalColor;

vec3 betterStep(vec3 a, vec3 b) {
    // bvec3 c;
    // c.x = (a.x <= b.x) || isinf(a.x);
    // c.y = (a.y <= b.y) || isinf(a.y);
    // c.z = (a.z <= b.z) || isinf(a.z);
    // return vec3(c);
    return min(vec3(lessThanEqual(a, b)) + vec3(isinf(a)), vec3(1));
}

vec2 intersectAABB(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax) {
    vec3 tMin = (boxMin - rayOrigin) / rayDir;
    vec3 tMax = (boxMax - rayOrigin) / rayDir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
}

float random(in vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float ray_cast(in vec3 ro, in vec3 rd, in vec2 bounds)
{
    if (bounds.x > bounds.y) return 0.0;

    vec3 si = sign(rd),
    d = 1.0 / abs(rd),
    v = ro + rd * bounds.x,
    s = (si * (0.5 - fract(v)) + 0.5) * d,
    mask;

    v = floor(v) + 0.5;

    float value = 0.0,
    u = 0.0,
    t = 0.0;

    for (int i = 0; i < nSteps && t < bounds.y - bounds.x; ++i)
    {
        mask = betterStep(s.xyz, min(s.yzx, s.zxy));
        u = min(s.x, min(s.y, s.z));
        value += texture(voxelData, v / voxelRes).r * density * (hardCubes ? 1.0 : (u - t));
        t = u;
        s += mask * d;
        v += mask * si;
    }
    return value;
}

void main()
{
    vec2 uv = (gl_FragCoord.xy - renderRes.xy / 2) / renderRes.xy * 2;

    vec4 near = vpi * vec4(uv, -1.0, 1.0);
    vec4 far = vpi * vec4(uv, 1.0, 1.0);

    vec3 ro = (near.xyz / near.w) * voxelRes / 10.0;
    vec3 rd = normalize((far.xyz / far.w) - (near.xyz / near.w));

    vec2 bounds = intersectAABB(ro, rd, vec3(0), vec3(voxelRes));
    bounds.x = max(bounds.x, 0.0);

    float cloud = ray_cast(ro, rd, bounds);
    vec3 cloudCol = mix(vec3(0.4, 0.0, 0.4), vec3(1.0, 0.9, 0.3), cloud);

    finalColor = vec4(cloudCol, (hardEdges ? (cloud > 0 ? 1 : 0) : cloud * 2));
}
