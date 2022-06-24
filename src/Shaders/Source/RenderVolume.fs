$input v_texcoord0, v_positionWorld

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER3D(s_texTiles,  0);
SAMPLER3D(s_texWorldToTile,  1);

uniform vec4 eyePosition;
uniform vec4 directional;

vec4 SampleTile(vec3 worldTexCoord, float scale)
{
    vec4 page = texture3D(s_texWorldToTile, worldTexCoord.xyz);
    vec3 localCoord = mod(worldTexCoord.xyz / scale, 1./16.);
    vec3 tileCoord = (page.xyz * 255.) * 1./16.;
    return texture3D(s_texTiles, tileCoord + localCoord);
}

float GetAccum(vec3 rayOrigin, vec3 rayDir, int steps, float jitter)
{
    float accum = 0.;
    float jitterScale = hash(vec4(rayOrigin.xy+vec2(steps, steps), rayOrigin.yx * float(steps))) * jitter;
    vec2 boxIntersection = intersectAABB(rayOrigin, rayDir, vec3(0., 0., 0.), vec3(1., 1., 1.));
    if (abs(boxIntersection.y) > abs(boxIntersection.x))
    {
        float step = (boxIntersection.y - boxIntersection.x) / float(steps);
        
        for(int i = 0; i < steps; i ++)
        {
            vec3 rayPos = rayOrigin + rayDir * (boxIntersection.x + step * (float(i) + 0.5 + jitterScale));
            //float density = texture3D(texDensity, rayPos, 0).x;
            float density = SampleTile(rayPos, 1.).x;
            
            accum += density;
        }
    }
    return accum;
}

vec2 March(vec3 rayOrigin, vec3 rayDir, int steps, float absorption)
{
    float lightenergy = 0.;
    float transmittance = 1.;
    float jitterScale = hash(vec4(rayOrigin.xy+vec2(steps, steps), rayOrigin.yx * float(steps))) * 0.3;
    vec2 boxIntersection = intersectAABB(rayOrigin, rayDir, vec3(0., 0., 0.), vec3(1., 1., 1.));
    if (abs(boxIntersection.y) > abs(boxIntersection.x))
    {
        float step = (boxIntersection.y - boxIntersection.x) / float(steps);
        
        for(int i = 0; i < steps; i ++)
        {
            vec3 rayPos = rayOrigin + rayDir * (boxIntersection.x + step * (float(i) + 0.5 + jitterScale));
            //float density = texture3D(texDensity, rayPos, 0).x;
            float density = SampleTile(rayPos, 1.).x;
            if (density > 0.001)
            {
                float curDensity = density * absorption;
                transmittance *= 1. - curDensity;
                
                // light
                float shadowDist = GetAccum(rayPos, -directional.xyz, 3, 0.8);
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
/*

float GetAccumEstShadow(vec3 rayOrigin, vec3 rayDir, int steps, float jitterScale)
{
    float accum = 0.;
    float theshold = 3.5;
    float step = 2. / float(steps);

    for(int i = 0; i < steps; i ++)
    {
        vec3 rayPos = rayOrigin + rayDir * (step * (float(i) + 1.5 + jitterScale));
        float density = texture3D(texDensity, rayPos, 0).x;
        
        accum += density;
        
        if (accum >= theshold)
        {
            return accum * (float(steps) / float(i));
        }
    }
    return accum;
}

vec2 GetAccumEst(vec3 rayOrigin, vec3 rayDir, int steps, float jitter)
{
    float accum = 0.;
    float theshold = 3.5;
    float jitterScale = hash(vec4(rayOrigin.xy+vec2(steps, steps), rayOrigin.yx * float(steps))) * jitter;
    vec2 boxIntersection = intersectAABB(rayOrigin, rayDir, vec3(0., 0., 0.), vec3(1., 1., 1.));
    if (abs(boxIntersection.y) > abs(boxIntersection.x))
    {
        float step = (boxIntersection.y - boxIntersection.x) / float(steps);

        for(int i = 0; i < steps; i ++)
        {
            vec3 rayPos = rayOrigin + rayDir * (boxIntersection.x + step * (float(i) + 0.5 + jitterScale));
            float density = texture3D(texDensity, rayPos, 0).x;
            
            accum += density;
            
            if (accum >= theshold)
            {
                float lgt = GetAccumEstShadow(rayPos, -directional.xyz, steps/2, jitterScale * 1.9);
                return vec2(accum * (float(steps) / float(i)), lgt);
            }
        }
        return vec2(accum, 0.);
    }
    return vec2(accum, 0.);
}
*/

void main()
{
    vec3 rayOrigin = v_positionWorld.xyz;
    vec3 rayDir = normalize(rayOrigin - eyePosition.xyz);
    vec3 rayPos = rayOrigin;
    vec3 result = vec3(0., 0., 0.);
    vec3 dir = vec3(0., 1., 0.);
    vec3 lightColor = vec3(0.9, 0.8, 0.7) * 2.;
    
    float absorption = 0.5;
    int MAX_STEPS = 50;
    
    // with shadow term
    
    vec2 transmitanceEnergy = March(rayOrigin, rayDir, MAX_STEPS, absorption);
    result = lightColor * transmitanceEnergy.y;
    gl_FragColor = vec4(result, 1. - transmitanceEnergy.x);
    /*
    
     
    float transmitance = GetAccum(rayOrigin, rayDir, MAX_STEPS, 1.);
    gl_FragColor = vec4(1., 1., 1., max(1. - exp(-transmitance * absorption), 0.01));
     */
     /*
     vec2 estimate = GetAccumEst(rayOrigin, rayDir, MAX_STEPS, 0.8);
     float transparency = 1. - exp(-estimate.x * absorption);
     float l = mix(0.8, 1., max((exp(-estimate.y)), 0.)) * transparency;
     gl_FragColor = vec4(l, l, l, transparency);
     */
     //gl_FragColor = vec4(1.0,0.0,1.0,1.0);

     /*
     // complexity
     float transmitance = GetAccumEst(rayOrigin, rayDir, MAX_STEPS, 0.6);
     gl_FragColor = vec4(transmitance/255., 0., 0., 1.);
     */
}
