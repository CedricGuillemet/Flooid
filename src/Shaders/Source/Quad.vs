$input a_texcoord0
$output v_texcoord0

#include "bgfx_shader.sh"
#include "Common.shader"

void main()
{
    v_texcoord0 = a_texcoord0;
    v_texcoord0.y = 1. - v_texcoord0.y;
	gl_Position = vec4(a_texcoord0.xy * 2.0 - 1.0, 0.5, 1.0);
}
