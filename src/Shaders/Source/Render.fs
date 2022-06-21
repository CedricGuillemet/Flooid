$input v_texcoord0, v_positionWorld

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER3D(s_texTiles,  0);
SAMPLER3D(s_texWorldToTile,  1);

vec4 SamplePage(vec3 worldTexCoord, float scale)
{
    vec4 page = texture3D(s_texWorldToTile, worldTexCoord.xyz, 0);
    vec3 localCoord = mod(worldTexCoord.xyz / scale, 1./16.);
    vec3 pageCoord = (page.xyz * 255.) * 1./16.;
    return texture3D(s_texTiles, pageCoord + localCoord);
}

SAMPLER3D(s_worldToTileTags, 5);

uniform vec4 eyePosition;
uniform vec4 debugDisplay; // grid, page allocation, texture type, debug level
uniform vec4 debugDisplay2; // Z slice

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
    float scaling = pow(2., debugDisplay.w);
    
    float slice = debugDisplay2.x;
    vec3 texCoord = vec3(v_texcoord0.xy, slice);
    vec4 tag = texture3D(s_worldToTileTags, texCoord.xyz / scaling, 0);
    vec2 localCoord = mod(texCoord.xy, 1./16.);
    localCoord = mod(texCoord.xy / scaling, 1./16.);
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
    // residual/jacobi
    if (debugDisplay.z >= 4. && debugDisplay.z <= 5.)
    {
        float value = SamplePage(texCoord.xyz, scaling).x;
        vec3 logColor = logspace_color_map(value, 1.);
        gl_FragColor = vec4(logColor.xyz, 1.);
    }
else
    if (tag.x > 0.)
    {
        // density
        if (abs(debugDisplay.z - 0.) < 0.001)
        {
            vec4 color = SamplePage(texCoord.xyz, 1.);
            color.a = 1.;
            gl_FragColor = color;
        }
        // velocity
        else if (abs(debugDisplay.z - 1.) < 0.001)
        {
            vec4 direction = SamplePage(texCoord.xyz, 1.);
            vec4 color = direction * 0.5 + 0.5;
            gl_FragColor = vec4(color.xy, 0., 1.);
        }
        // divergence
        else if (abs(debugDisplay.z - 3.) < 0.001)
        {
            float pressure = SamplePage(texCoord.xyz, 1.).x;// * 10. + 0.5;
            vec3 logColor = logspace_color_map(pressure, 1.);
            gl_FragColor = vec4(logColor.xyz, 1.);
        }
        // gradient
        else if (abs(debugDisplay.z - 6.) < 0.001)
        {
            vec2 gradient = SamplePage(texCoord.xyz, 1.).xy + 0.5;
            gl_FragColor = vec4(gradient, 0., 1.);
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
