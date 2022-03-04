$input a_texcoord0, a_position
$output v_texcoord0, v_positionWorld

#include "bgfx_shader.sh"
#include "Common.shader"

void main()
{
    v_texcoord0 = a_texcoord0;
    v_positionWorld = a_position.xyz * 1000.;
    gl_Position = mul(u_viewProjection, vec4(v_positionWorld.xyz, 1.0));
}

