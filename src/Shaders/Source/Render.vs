$input a_texcoord0, a_position
$output v_texcoord0, v_positionWorld

#include "bgfx_shader.sh"
#include "Common.shader"

void main()
{
    v_texcoord0 = a_texcoord0;
    v_positionWorld = a_position;
    //gl_Position = mul(u_viewProjection, vec4(a_position.xyz, 1.0));
    gl_Position = vec4(a_texcoord0.xy * 2. - 1., 0., 1.);
}

