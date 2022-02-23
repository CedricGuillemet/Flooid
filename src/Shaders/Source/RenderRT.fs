$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER2D(s_texColor,  0);
void main()
{
    gl_FragColor = texture2D(s_texColor, v_texcoord0.xy);
}
