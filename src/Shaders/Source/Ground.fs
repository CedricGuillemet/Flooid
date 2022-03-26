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
    
    int MAX_STEPS = 5;

    
    vec3 rayOrigin = v_positionWorld.xyz;
    vec3 rayDir = -directional.xyz;
    
    
    vec2 boxIntersection = intersectAABB(rayOrigin, rayDir, vec3(0., 0., 0.), vec3(1., 1., 1.));
    
    if (abs(boxIntersection.y) > abs(boxIntersection.x))
    {
        float jitterScale = hash(vec4(rayOrigin.xy+vec2(MAX_STEPS), rayOrigin.yx * float(MAX_STEPS))) * 0.2;
        
        
        float accum = 0.;
        float absorption = 0.1;

        float step = (boxIntersection.y - boxIntersection.x) / float(MAX_STEPS);
        for(int i = 0; i < MAX_STEPS; i ++)
        {
            vec3 rayPos = rayOrigin + rayDir * (step * (float(i) + 0.5) + jitterScale + boxIntersection.x);
            accum += texture3D(texDensity, rayPos, 0).x;
        }

        g *= exp(-accum * absorption);
    }
    
    gl_FragColor = vec4(g, g, g, 1.0);
    //gl_FragColor = vec4(v_normal *0.5 + 0.5, 1.0);
}
