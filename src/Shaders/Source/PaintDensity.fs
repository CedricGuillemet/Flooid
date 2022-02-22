$input v_texcoord0

#include "bgfx_shader.sh"
#include "CommonFS.shader"
#include "Common.shader"

uniform vec4 brush; // position.xy, radius, strength
uniform vec4 brushColor; //

void main()
{
    float dist = 1. - distance(v_texcoord0, brush.xy) / brush.z;
    gl_FragColor = vec4(brushColor.xyz, 1.) * max(dist * brush.w, 0.);
}
