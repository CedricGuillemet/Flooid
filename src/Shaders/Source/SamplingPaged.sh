SAMPLER2D(s_texPages,  0);
SAMPLER2D(s_texWorldToPage,  1);

vec4 SamplePage(vec2 worldTexCoord)
{
    vec4 page = texture2D(s_texWorldToPage, worldTexCoord.xy, 0);
    vec2 localCoord = mod(worldTexCoord.xy, 1./16.);
    vec2 pageCoord = (page.xy * 255.) * 1./16.;
    return texture2D(s_texPages, pageCoord + localCoord, 0);
}
