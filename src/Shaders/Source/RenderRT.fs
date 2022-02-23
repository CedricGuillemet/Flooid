$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texVelocity,  1);
void main()
{
    vec4 density = texture2D(s_texColor, v_texcoord0.xy);
    vec4 velocity = texture2D(s_texVelocity, v_texcoord0.xy);
    gl_FragColor = vec4(density.x, velocity.xy, 1.0);
}
