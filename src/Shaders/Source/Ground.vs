$input a_texcoord0, a_position
$output v_texcoord0

#include "bgfx_shader.sh"
#include "Common.shader"

void main()
{
    v_texcoord0 = a_texcoord0;
    gl_Position = mul(u_viewProjection, vec4(a_position.xyz, 1.0));
}

