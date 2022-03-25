$input v_texcoord0, v_normal, v_positionWorld

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER3D(texDensity, 0);

uniform vec4 directional;

float grid(vec2 st, float res)
{
    vec2 grid = fract((st + 0.5) * res);
    return (step(res, grid.x) * step(res, grid.y));
}

void main()
{
    vec3 lightDir = vec3(0.2, -0.6, 0.5);
    //vec3 lightToPos = lightPos - v_positionWorld;
    float illum = dot(normalize(-lightDir), normalize(v_normal)) * 0.5 + 0.8;
    
    float g = grid(v_texcoord0.xy * 8., 0.05) * 0.4 + 0.6;
    g = min(g, grid(v_texcoord0.xy * 40. + 0.5, 0.1) * 0.2 + 0.8);
    g *= 0.3;
    g -= 0.05;
    
    g *= illum;

    
    vec3 rayOrigin = v_positionWorld.xyz;
    vec3 rayDir = -directional.xyz;
    
    
    vec2 boxIntersection = intersectAABB(rayOrigin, rayDir, vec3(0., 0., 0.), vec3(1., 1., 1.));
    if (abs(boxIntersection.y) > abs(boxIntersection.x))
    {
        vec3 rayPos = rayOrigin + rayDir * boxIntersection.x;
        
        float accum = 0.;
        int MAX_STEPS = 20;
        float step = (boxIntersection.y - boxIntersection.x) / float(MAX_STEPS);
        for(int i = 0; i < MAX_STEPS; i ++)
        {
            float density = texture3D(texDensity, rayPos, 0).x;
            accum += density;
            rayPos += rayDir * step;
        }
        accum = min(accum, 1.);
        g *= mix(1., 0.5, accum);
    }
    
    gl_FragColor = vec4(g, g, g, 1.0);
    //gl_FragColor = vec4(v_normal *0.5 + 0.5, 1.0);
}
