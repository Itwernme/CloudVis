#version 330

#define PI 3.14159265359

uniform sampler3D voxelData;
uniform vec3 voxelRes;
uniform float voxelScale;

uniform vec3 cameraPos;
uniform mat4 vpi;
uniform vec2 renderRes;

uniform int nSteps;
uniform float density;
uniform int diminish;
uniform bool hardEdges;
uniform bool hardCubes;

uniform float minValue;
uniform float maxValue;

uniform bool grid = true;
uniform bool axis = true;
uniform float axisThickness;
uniform float gridThickness;
uniform float gridScale = 0.1;

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

vec4 ray_cast(in vec3 ro, in vec3 rd, in vec2 bounds)
{
    if (bounds.x > bounds.y) return vec4(0.0);

    vec3 si = sign(rd),
    d = 1.0 / abs(rd),
    v = ro + rd * bounds.x,
    s = (si * (0.5 - fract(v)) + 0.5) * d,
    mask;

    vec4 col = vec4(0.0);

    float u = 0.0,
    t = 0.0,
    value = 0.0;

    vec3 p = (ro + rd * bounds.x) / voxelRes;
    if (axis) {
        bvec3 axis = lessThanEqual(p.yxx + p.zzy, vec3(axisThickness));
        if (any(axis)) return vec4(axis, 1.0);
    }
    if (grid) {
        bvec2 grid = lessThanEqual(mod(p.zx + gridThickness, vec2(gridScale)) + p.y, vec2(gridThickness));
        if (any(grid)) return vec4(grid.x, 0, grid.y, 1.0);
    }

    v = floor(v) + 0.5;

    for (int i = 0; i < nSteps && t < bounds.y - bounds.x; ++i)
    {
        mask = betterStep(s.xyz, min(s.yzx, s.zxy));
        u = min(s.x, min(s.y, s.z));
        value = texture(voxelData, v / voxelRes).r;
        col.a += value * density * (hardCubes ? 1.0 : (u - t)) * max(1.0 - col.a * diminish, 0.0) * float(minValue <= value && value <= maxValue);
        t = u;
        s += mask * d;
        v += mask * si;
    }

    col = vec4(mix(vec3(0.4, 0.0, 0.4), vec3(1.0, 0.9, 0.3), col.a), (hardEdges ? (col.a > 0 ? 1 : 0) : col.a));

    p = (ro + rd * bounds.y) / voxelRes;
    if (axis) {
        bvec3 axis = lessThanEqual(p.yxx + p.zzy, vec3(axisThickness));
        col = mix(vec4(axis, any(axis)), col, col.a);
    }
    if (grid) {
        bvec2 grid = lessThanEqual(mod(p.zx + gridThickness, vec2(gridScale)) + p.y, vec2(gridThickness));
        col = mix(vec4(grid.x, 0, grid.y, any(grid)), col, col.a);
    }

    return col;
}

void main()
{
    vec2 uv = (gl_FragCoord.xy - renderRes.xy / 2) / renderRes.xy * 2;

    vec4 near = vpi * vec4(uv, -1.0, 1.0);
    vec4 far = vpi * vec4(uv, 1.0, 1.0);

    vec3 ro = (near.xyz / near.w) * voxelScale;
    vec3 rd = normalize((far.xyz / far.w) - (near.xyz / near.w));

    vec2 bounds = intersectAABB(ro, rd, vec3(0), voxelRes);
    bounds.x = max(bounds.x, 0.0);

    //vec3 cloudCol = mix(vec3(0.4, 0.0, 0.4), vec3(1.0, 0.9, 0.3), cloud);

    finalColor = ray_cast(ro, rd, bounds);
}
