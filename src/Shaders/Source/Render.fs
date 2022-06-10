$input v_texcoord0, v_positionWorld


#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"
#include "SamplingPaged.sh"
//#include "Arrows.glsl"

SAMPLER2D(s_worldToPageTags, 5);

uniform vec4 eyePosition;
uniform vec4 debugDisplay; // grid, page allocation, texture type

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

/*
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

/*
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
 */

/*
float rand(vec2 c){
    return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
    float unit = screenWidth/freq;
    vec2 ij = floor(p/unit);
    vec2 xy = mod(p,unit)/unit;
    //xy = 3.*xy*xy-2.*xy*xy*xy;
    xy = .5*(1.-cos(PI*xy));
    float a = rand((ij+vec2(0.,0.)));
    float b = rand((ij+vec2(1.,0.)));
    float c = rand((ij+vec2(0.,1.)));
    float d = rand((ij+vec2(1.,1.)));
    float x1 = mix(a, b, xy.x);
    float x2 = mix(c, d, xy.x);
    return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
    float persistance = .5;
    float n = 0.;
    float normK = 0.;
    float f = 4.;
    float amp = 1.;
    int iCount = 0;
    for (int i = 0; i<50; i++){
        n+=amp*noise(p, f);
        f*=2.;
        normK+=amp;
        amp*=persistance;
        if (iCount == res) break;
        iCount++;
    }
    float nf = n/normK;
    return nf*nf*nf*nf;
}
//#define M_PI 3.14159265358979323846

float rand(vec2 co){return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);}
float rand (vec2 co, float l) {return rand(vec2(rand(co), l));}
float rand (vec2 co, float l, float t) {return rand(vec2(rand(co, l), t));}

float perlin(vec2 p, float dim, float time) {
    vec2 pos = floor(p * dim);
    vec2 posx = pos + vec2(1.0, 0.0);
    vec2 posy = pos + vec2(0.0, 1.0);
    vec2 posxy = pos + vec2(1.0);
    
    float c = rand(pos, dim, time);
    float cx = rand(posx, dim, time);
    float cy = rand(posy, dim, time);
    float cxy = rand(posxy, dim, time);
    
    vec2 d = fract(p * dim);
    d = -0.5 * cos(d * M_PI) + 0.5;
    
    float ccx = mix(c, cx, d.x);
    float cycxy = mix(cy, cxy, d.x);
    float center = mix(ccx, cycxy, d.y);
    
    return center * 2.0 - 1.0;
}

// p must be normalized!
float perlin(vec2 p, float dim) {
    
    /*vec2 pos = floor(p * dim);
    vec2 posx = pos + vec2(1.0, 0.0);
    vec2 posy = pos + vec2(0.0, 1.0);
    vec2 posxy = pos + vec2(1.0);
    
    // For exclusively black/white noise
    /*float c = step(rand(pos, dim), 0.5);
    float cx = step(rand(posx, dim), 0.5);
    float cy = step(rand(posy, dim), 0.5);
    float cxy = step(rand(posxy, dim), 0.5);*/
    
    /*float c = rand(pos, dim);
    float cx = rand(posx, dim);
    float cy = rand(posy, dim);
    float cxy = rand(posxy, dim);
    
    vec2 d = fract(p * dim);
    d = -0.5 * cos(d * M_PI) + 0.5;
    
    float ccx = mix(c, cx, d.x);
    float cycxy = mix(cy, cxy, d.x);
    float center = mix(ccx, cycxy, d.y);
    
    return center * 2.0 - 1.0;
    return perlin(p, dim, 0.0);
}
*/
// 4 bands per hue gives a decent look
#define LOGSPACE_QUANTIZATION_PER_HUE 4.0

vec3 logspace_color_map(float v, float scale/* = 1.0*/) {
    // A unique hue is mapped to each order of magnitude base 10,
    // i.e. when moving from one hue to the next the value reduces by 10
    float logv = log(scale * abs(v)) / log(10.0);
    
    // Bias by 7.0 to make 1.0e-7 map to 0.0, for simplicity
    float f = floor(logv + 7.0);
    float i = floor(LOGSPACE_QUANTIZATION_PER_HUE * ((logv + 7.0) - f));

    // Mixes with white to give it a bit more punch
    if (f < 0.0) return vec3(0.0, 0.0, 0.0);                                                              // Black:  |v| <  1.0e-7
    if (f < 1.0) return mix(vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0), i / LOGSPACE_QUANTIZATION_PER_HUE); // Red:    |v| <  1.0e-6
    if (f < 2.0) return mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0), i / LOGSPACE_QUANTIZATION_PER_HUE); // Green:  |v| <  1.0e-5
    if (f < 3.0) return mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), i / LOGSPACE_QUANTIZATION_PER_HUE); // Blue:   |v| <  1.0e-4
    if (f < 4.0) return mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0), i / LOGSPACE_QUANTIZATION_PER_HUE); // Yellow: |v| <  1.0e-3
    if (f < 5.0) return mix(vec3(1.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), i / LOGSPACE_QUANTIZATION_PER_HUE); // Purple: |v| <  1.0e-2
    if (f < 6.0) return mix(vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0), i / LOGSPACE_QUANTIZATION_PER_HUE); // Cyan:   |v| <  1.0e-1
    if (f < 7.0) return mix(vec3(1.0, 0.5, 0.0), vec3(1.0, 1.0, 1.0), i / LOGSPACE_QUANTIZATION_PER_HUE); // Orange: |v| <  1.0e-0
    return vec3(1.0, 1.0, 1.0);                                                                           // White:  |v| >= 1.0e+0
}
 
void main()
{
    /*
    vec3 lightDir = normalize(vec3(-1., 1., -1.));
    
    float step = 0.04;
    int stepCount = 10;
    int shadowStepCount = 10;
    vec3 dir = normalize(v_positionWorld - eyePosition.xyz) * step;
    vec3 march = v_positionWorld;
    
    float accum = 0.;
    vec3 diffuse = vec3(1., 1., 1.);
    float shadow = 0.;
    float prevHeight = 0.;
    float prevZ = 0.;
    for(int i = 0; i< stepCount; i ++)
    {
        float height = texture2D(s_texDensity, march.xy, 0).x;
        float opacity = max(height - abs(march.z), 0.) * step;
        if (opacity > 0.001)
        {
            vec3 lpos = march;
            for(int j = 0;j < shadowStepCount;j ++)
            {
                lpos += lightDir * step;
                float dShadow = texture2D(s_texDensity, lpos.xy, 0).x;
                float dOpa = max(dShadow - abs(lpos.z), 0.) * step;
                shadow += dOpa;
            }
            
            //opacity += (prevHeight - prevZ) / (abs(march.z) - prevZ - height + prevHeight) * step;
        }
        accum += opacity;
        march += dir;
        prevZ = abs(march.z);
        prevHeight = height;
    }
    accum *= 10.;
    shadow *= 0.36;
    diffuse -= vec3(shadow, shadow, shadow);
    //accum = texture2D(s_texDensity, v_positionWorld.xy, 0).x;
    //gl_FragColor = vec4(accum, accum, accum, 1.);
    gl_FragColor = vec4(diffuse, accum);
    */

    
    //float accum = texture2D(s_texDensity, v_positionWorld.xy, 0).x;// * 0.2 + 0.5;
    //gl_FragColor = vec4(accum, accum, accum, 1.);
    /*
    vec2 accum = texture2D(s_texDensity, v_positionWorld.xy, 0).xy * 1. + 0.5;
    gl_FragColor = vec4(accum, 0., 1.);
    */

    //vec4 page = texture2D(s_texWorldToPage, v_texcoord0.xy, 0);
    vec4 tag = texture2D(s_worldToPageTags, v_texcoord0.xy, 0);
    vec2 localCoord = mod(v_texcoord0.xy, 1./16.);
    //vec2 pageCoord = (page.xy * 255.) * 1./16.;
    vec4 tagColor = vec4(0., 0., 0., 1.);
    if (tag.x == 1./255.)
        tagColor = vec4(1.,0.5,0.1,10.) * 0.4;
    else if (tag.x != 0.)
        tagColor = vec4(1., 1., 1., 10.) * 0.5;
    if (abs(debugDisplay.z - 2.) < 0.001)
    {
        gl_FragColor = vec4(tag.x * 127.5,0.,0.,1.);
        tagColor = vec4(1.,1.,1.,1.);
    }
    if (abs(debugDisplay.z - 7.) < 0.001)
    {
        float residual = SamplePage(v_texcoord0.xy * 0.5).x;
        vec3 logColor = logspace_color_map(residual, 1.);
        gl_FragColor = vec4(logColor.xyz, 1.);
        
        localCoord = mod(v_texcoord0.xy * 0.5, 1./16.);
    } else if (abs(debugDisplay.z - 8.) < 0.001)
    {
        float jacobi = SamplePage(v_texcoord0.xy * 0.5).x;// + 0.5;
        vec3 logColor = logspace_color_map(jacobi, 1.);
        gl_FragColor = vec4(logColor.xyz, 1.);
        localCoord = mod(v_texcoord0.xy * 0.5, 1./16.);
    } else if (abs(debugDisplay.z - 9.) < 0.001)
    {
        float jacobi = SamplePage(v_texcoord0.xy * 0.5).x;// + 0.5;
        vec3 logColor = logspace_color_map(jacobi, 1.);
        gl_FragColor = vec4(logColor.xyz, 1.);

        localCoord = mod(v_texcoord0.xy * 0.5, 1./16.);
    }
else
    if (tag.x > 0.)
    {
        // density
        if (abs(debugDisplay.z - 0.) < 0.001)
        {
            vec4 color = SamplePage(v_texcoord0.xy);
            color.a = 1.;
            gl_FragColor = color;
        }
        // velocity
        else if (abs(debugDisplay.z - 1.) < 0.001)
        {
            vec4 direction = SamplePage(v_texcoord0.xy);
            vec4 color = direction * 0.5 + 0.5;
            gl_FragColor = vec4(color.xy, 0., 1.);
            //gl_FragColor = vec4(arrow, arrow, arrow, 1.);
        }
        // divergence
        else if (abs(debugDisplay.z - 3.) < 0.001)
        {
            float pressure = SamplePage(v_texcoord0.xy).x;// * 10. + 0.5;
            vec3 logColor = logspace_color_map(pressure, 1.);
            gl_FragColor = vec4(logColor.xyz, 1.);
        }
        // jacobi
        else if (abs(debugDisplay.z - 4.) < 0.001)
        {
            float jacobi = SamplePage(v_texcoord0.xy).x;// + 0.5;
            vec3 logColor = logspace_color_map(jacobi, 1.);
            gl_FragColor = vec4(logColor.xyz, 1.);
        }
        // gradient
        else if (abs(debugDisplay.z - 5.) < 0.001)
        {
            vec2 gradient = SamplePage(v_texcoord0.xy).xy + 0.5;
            gl_FragColor = vec4(gradient, 0., 1.);
        }
        // residual mip 0
        else if (abs(debugDisplay.z - 6.) < 0.001)
        {
            float residual = SamplePage(v_texcoord0.xy).x;
            vec3 logColor = logspace_color_map(residual, 1.);
            gl_FragColor = vec4(logColor.xyz, 1.);
        }
    } 
    else
    {
        gl_FragColor = vec4(0.,0.,0.,1.);
    }
    //gl_FragColor *= tagColor;

    float mx = max(step(localCoord.x, 1./255.), step(localCoord.y, 1./255.));
    mx = max(mx, 1. - step(localCoord.x, 15./255.));
    mx = max(mx, 1. - step(localCoord.y, 15./255.));
    gl_FragColor = mix(gl_FragColor, vec4(1.,1.,1.,1.) * tagColor, mx * debugDisplay.x);
}
