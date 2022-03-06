$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER2D(s_texDensity,  0);
/*
float hash(vec4 p)  // replace this by something better
{
    p  = fract(p * 0.3183099 + vec4(0.1));
    p *= 17.0;
    return fract( p.x*p.y*p.z*p.w*(p.x+p.y+p.z+p.w) );
}

float getAccum(vec2 UV)
{
    vec3 TraceVec = vec3(1., 0., 1.);
    float startZ = 0.5;
    TraceVec = TraceVec * 0.001;
    float jitterScale = 0.1;
    float bias = 0.01;
    float StepSize = 0.1;
    
    
    float NumSteps = 10.;
    vec2 TimeLerp = vec2(1.0, 1.0);
    // float2 DepthDiff = float2(0.0f, 0.0f);
    // float2 LastDiff = float2(-bias, -bias);
    // new diff is just combining the above 2.
    vec4 diff = vec4(0.0, 0.0, -bias , -bias);

    //We scale Z by 2 since the heightmap represents two halves of as symmetrical volume texture, split along Z where texture = 0
    vec3 RayStepUVz = vec3(TraceVec.x, TraceVec.y, TraceVec.z * 2.);
    vec2 accum = vec2(0.0, 0.0);
    vec3 RayUVz = vec3(UV, startZ);
    jitterScale *= hash(vec4(UV.xy+vec2(NumSteps), UV.yx * NumSteps));
    for(int i = 0; i < int(NumSteps); i++)
    {
        RayUVz += RayStepUVz;
        RayUVz.xy = saturate(RayUVz.xy+jitterScale);
        vec2 SampleDepth = texture2D(s_texDensity, RayUVz.xy, 0).xy;
        diff.xy = vec2(abs(RayUVz.z)) - SampleDepth;
        float df = (diff < 0.001) ? 1. : 0.;
        vec4 DiffComposite = vec4(df, df, df, df);
        TimeLerp = saturate( diff.zw / (diff.zw - diff.xy)).xx;
        accum += StepSize * ( TimeLerp * ( DiffComposite.zw - DiffComposite.xy ) + DiffComposite.xy );
        diff.zw = diff.xy;
    }
    return accum.x;
}
*/


float GetAccum(vec3 TraceVec, vec2 UV)
{
    TraceVec = TraceVec * 0.005;
    vec4 channel = vec4(1., 0., 0., 0.);
    float threshold = 0.01;
    float bias = 0.05;
    float startZ = 0.5;
    int NumSteps = 20;
    float StepSize = 0.1;
    
    if(startZ<=threshold) return 0.;
    float TimeLerp = 1.;
    float DepthDiff = 0.;
    float LastDiff = -bias;

    //scale Z by 2 since the heightmap represents two halves of as symmetrical volume texture, split along Z where texture = 0
    vec3 RayStepUVz = vec3(TraceVec.x, TraceVec.y, TraceVec.z * 2.);

    float accum = 0.;
    vec3 RayUVz = vec3(UV, (startZ - bias) );
    int i = 0;
    while (i < NumSteps)
    {
        RayUVz += RayStepUVz;

        float SampleDepth = dot( channel, texture2D(s_texDensity, RayUVz.xy,0.) );
        DepthDiff = abs(RayUVz.z) - abs(SampleDepth);

        if (DepthDiff <= 0.)
        {

            if(LastDiff > 0.)
            {
                TimeLerp = saturate( LastDiff / (LastDiff - DepthDiff));
                accum += StepSize * (1. - TimeLerp);
            }
            else
            {
                accum+=StepSize;
            }
        }
        else
        if(LastDiff <= 0.)
        {
            TimeLerp = saturate( LastDiff / (LastDiff - DepthDiff));
            accum += StepSize * (TimeLerp);
        }

        LastDiff = DepthDiff;

        i++;
    }

    return accum;
}

/*
float GetAccum(vec3 TraceVec, vec2 UV)
{
    float startZ = 0.5;
    TraceVec = vec3(1., 0., 1.) * 0.01;
    vec3 RayUVz = vec3(UV, startZ);
    int i = 0;
    while (i < NumSteps)
    {
        RayUVz += RayStepUVz;

        float SampleDepth = texture2D(s_texDensity, RayUVz.xy,0.).x;
        DepthDiff = abs(RayUVz.z) - abs(SampleDepth);

        if (DepthDiff <= 0.)
        {

            if(LastDiff > 0.)
            {
                TimeLerp = saturate( LastDiff / (LastDiff - DepthDiff));
                accum += StepSize * (1. - TimeLerp);
            }
            else
            {
                accum+=StepSize;
            }
        }
        else
        if(LastDiff <= 0.)
        {
            TimeLerp = saturate( LastDiff / (LastDiff - DepthDiff));
            accum += StepSize * (TimeLerp);
        }

    LastDiff = DepthDiff;

    i++;
    }

    return accum;
}
 */
void main()
{
    vec4 tex = texture2D(s_texDensity, v_texcoord0);
    float height = tex.x;
    //gl_FragColor = vec4(density.x, density.x, density.x, density.x *2.);
    vec2 n = -vec2(dFdx(height), dFdy(height));
    vec3 norm = vec3(n * 10., 1.);
    vec3 nnorm = normalize(norm);
    
    vec3 lightDir = normalize(vec3(1, 0.3, 0.1));
    float illum = dot(nnorm, lightDir) * 0.5 + 0.5;
    
    float accum = GetAccum(nnorm, v_texcoord0);
    //illum *= 1. - accum;
    
    vec3 color = mix(vec3(1., 0.5, 0.2) * illum, vec3(0.1, 0.05, 0.02) * illum * 2., 1.-accum);
    //gl_FragColor = vec4(tex.x, density, density, 1.);
    gl_FragColor = vec4(color, tex.x*10.);
    //gl_FragColor = vec4(n, tex.x);
    gl_FragColor = vec4(tex.x, tex.x, tex.x, tex.x * 10.);
}
