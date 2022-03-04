$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

SAMPLER2D(s_texDensity,  0);

void main()
{
    vec4 density = texture2D(s_texDensity, v_texcoord0);
    gl_FragColor = vec4(density.x, density.x, density.x, density.x *2.);
}
