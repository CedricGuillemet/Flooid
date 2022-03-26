$input v_texcoord0, v_positionWorld

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER3D(texDensity, 0);

uniform vec4 eyePosition;
uniform vec4 directional;



float GetAccum(vec3 rayOrigin, vec3 rayDir, int steps, float jitter)
{
    float accum = 0.;
    float jitterScale = hash(vec4(rayOrigin.xy+vec2(steps), rayOrigin.yx * float(steps))) * jitter;
    //vec2 boxIntersection = intersectAABB(rayOrigin, rayDir, vec3(0., 0., 0.), vec3(1., 1., 1.));
    //if (abs(boxIntersection.y) > abs(boxIntersection.x))
    {
        float step = 1.73/*(boxIntersection.y - boxIntersection.x)*/ / float(steps);
        
        for(int i = 0; i < steps; i ++)
        {
            vec3 rayPos = rayOrigin + rayDir * (/*boxIntersection.x + */step * (float(i) + 0.5 + jitterScale));
            float density = texture3D(texDensity, rayPos, 0).x;
            
            accum += density;
        }
    }
    return accum;
}

vec2 March(vec3 rayOrigin, vec3 rayDir, int steps, float absorption)
{
    float lightenergy = 0.;
    float transmittance = 1.;
    float jitterScale = hash(vec4(rayOrigin.xy+vec2(steps), rayOrigin.yx * float(steps))) * 0.8;
    vec2 boxIntersection = intersectAABB(rayOrigin, rayDir, vec3(0., 0., 0.), vec3(1., 1., 1.));
    if (abs(boxIntersection.y) > abs(boxIntersection.x))
    {
        float step = (boxIntersection.y - boxIntersection.x) / float(steps);
        
        for(int i = 0; i < steps; i ++)
        {
            vec3 rayPos = rayOrigin + rayDir * (boxIntersection.x + step * (float(i) + 0.5 + jitterScale));
            float density = texture3D(texDensity, rayPos, 0).x;
            if (density > 0.001)
            {
                float curDensity = density * absorption;
                transmittance *= 1. - curDensity;
                
                // light
                float shadowDist = GetAccum(rayPos, -directional.xyz, steps/2, 0.8);
                float shadowterm = exp(-shadowDist * absorption);
                float absorbedlight = shadowterm * curDensity;
                lightenergy += absorbedlight * transmittance;
                if (lightenergy > 0.999)
                {
                    break;
                }
            }
            if (transmittance < 0.001)
            {
                break;
            }
        }
    }
    return vec2(transmittance, lightenergy);
}

void main()
{
    vec3 rayOrigin = v_positionWorld.xyz;
    vec3 rayDir = normalize(rayOrigin - eyePosition.xyz);
    vec3 rayPos = rayOrigin;
    vec3 result = vec3(0., 0., 0.);
    vec3 dir = vec3(0., 1., 0.);
    vec3 lightColor = vec3(1., 0.6, 0.5) * 2.;
    
    float absorption = 0.05;
    int MAX_STEPS = 40;
    
    // with shadow term
    
    vec2 transmitanceEnergy = March(rayOrigin, rayDir, MAX_STEPS, absorption);
    result = lightColor * transmitanceEnergy.y;
    gl_FragColor = vec4(result, 1. - transmitanceEnergy.x);
     /*
    float transmitance = GetAccum(rayOrigin, rayDir, MAX_STEPS);
    gl_FragColor = vec4(1., 1., 1., 1. - exp(-transmitance * absorption));*/
}
