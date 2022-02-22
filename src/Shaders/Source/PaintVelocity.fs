$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

uniform vec4 brush; // position.xy, radius, strength
uniform vec4 brushDirection; // xy direction

void main()
{
    float dist = 1. - distance(vUV, brush.xy) / brush.z;
    gl_FragColor = vec4(brushDirection.xy * 100., 0., 1.) * max(dist * brush.w, 0.);
}
