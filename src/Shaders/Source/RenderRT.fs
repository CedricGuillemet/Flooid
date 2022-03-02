$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER2D(s_texColor,  0);
void main()
{
    vec4 density = texture2D(s_texColor, v_texcoord0.xy);
    //float value = density.x * 10. + 0.5;
    float value = density.x;
    
    gl_FragColor = vec4(value, value, value, 1.0);
}
