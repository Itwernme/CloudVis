#version 330

#define PI 3.14159265359

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 cameraPos;
uniform vec3 targetPos;
uniform vec2 resolution;
uniform sampler3D voxelData;
uniform mat4 vpi;

out vec4 finalColor;

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
    const int NUMBER_OF_STEPS = 1024;
    const float MAX_DISTANCE = 40.0;
    float stepSize = MAX_DISTANCE / NUMBER_OF_STEPS;
    vec3 delta = rd * stepSize;

    vec3 pos = ro;
    float value = 0.0;

    for (int i = 0; i < NUMBER_OF_STEPS; ++i)
    {
        vec3 mapPos = pos / 15;
        float voxVal = texture(voxelData, mapPos).r; //texture(voxelData, mapPos.xy + vec2(mod(mapPos.z, 40) * 400, floor(mapPos.z / 40) * 400));
        float oob = float(length(vec3(lessThan(mapPos, vec3(0.0))) + vec3(greaterThan(mapPos, vec3(1.0)))) == 0);
        voxVal *= 0.05 * oob;
        value += voxVal;
        pos += delta;
    }
    return value;
}

mat3 setCamera(in vec3 ro, in vec3 ta, float cr)
{
    vec3 cw = normalize(ta - ro);
    vec3 cp = vec3(sin(cr), cos(cr), 0.0);
    vec3 cu = normalize(cross(cw, cp));
    vec3 cv = (cross(cu, cw));
    return mat3(cu, cv, cw);
}

void main()
{
    const float fov = 1 / atan(PI / 2.0);

    vec2 uv = (gl_FragCoord.xy - resolution.xy / 2) / resolution.y;

    vec3 ro = cameraPos;
    vec3 ta = targetPos;

    // camera-to-world transformation
    mat3 ca = setCamera(ro, ta, 0.0);
    // ray direction
    vec3 rd = ca * normalize(vec3(uv, fov));

    //vec4 near = vec4(uv * 2, 0.0, 1.0) * vpi;
    vec4 far = vec4(uv * 2, 1.0, 1.0) * vpi;

    rd = normalize((far.xyz / far.w) - cameraPos);

    float cloud = ray_cast(ro, rd);
    vec3 cloudCol = mix(vec3(0.2, 0.0, 0.7), vec3(1.0, 0.9, 0.3), cloud / 1.5);

    float s = (-ro.y) / rd.y;
    vec3 p = ro + rd * s;
    vec2 planePos = (ro + rd * s).xz;
    vec3 plane = vec3(checkerboard(planePos, 1.0) + 0.9);
    plane += centre(planePos, 0.1);
    plane *= (s > 0 ? 1.0 : 0.0);
    //plane = vec3(0.0);

    finalColor = vec4(mix(plane, cloudCol, cloud * 2), 1.0); //ray_cast(ro, rd)
}
