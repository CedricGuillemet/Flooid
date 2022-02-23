$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

uniform float timestep;
uniform float dissipation;

SAMPLER2D(s_texVelocity, 0);
SAMPLER2D(s_texAdvect, 1);

void main()
{
    vec2 uvAdvected = v_texcoord0 - timestep * texture2D(s_texVelocity, v_texcoord0).xy;
    uvAdvected = v_texcoord0 + vec2(-0.001,0.);
    gl_FragColor = vec4(dissipation * texture2D(s_texAdvect, uvAdvected).xy, 0.0, 1.0);
}
