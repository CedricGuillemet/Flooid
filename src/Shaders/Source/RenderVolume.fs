$input v_texcoord0, v_positionWorld

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER3D(texDensity, 0);

uniform vec4 eyePosition;

void main()
{
    vec3 rayOrigin = v_positionWorld.xyz;
    vec3 rayDir = normalize(rayOrigin - eyePosition.xyz);
    vec3 rayPos = rayOrigin;
    float accum = 0.;
    vec2 boxIntersection = intersectAABB(rayOrigin, rayDir, vec3(0., 0., 0.), vec3(1., 1., 1.));
    if (abs(boxIntersection.y) > abs(boxIntersection.x))
    {
        
        int MAX_STEPS = 10;
        float step = (boxIntersection.y - boxIntersection.x) / float(MAX_STEPS);
        rayPos += rayDir * boxIntersection.x;
        for(int i = 0; i < MAX_STEPS; i ++)
        {
            float density = texture3D(texDensity, rayPos, 0).x;
            accum += density;
            rayPos += rayDir * step;
        }
    }
    gl_FragColor = vec4(1., 0., 0., accum);
}
