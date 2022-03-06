$input v_texcoord0, v_normal, v_positionWorld

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

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

    gl_FragColor = vec4(g, g, g, 1.0);
    //gl_FragColor = vec4(v_normal *0.5 + 0.5, 1.0);
}
