$input a_texcoord0, a_position, a_normal
$output v_texcoord0, v_normal, v_positionWorld

#include "bgfx_shader.sh"
#include "Common.shader"

void main()
{
    v_texcoord0 = a_texcoord0;
    v_normal = a_normal;
    v_positionWorld = a_position;
    gl_Position = mul(u_viewProjection, vec4(a_position.xyz, 1.0));
}

