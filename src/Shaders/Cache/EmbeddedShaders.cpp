#include "EmbeddedShaders.h"
#include "bgfx_utils.h"
static const bgfx::EmbeddedShader embeddedShaders[] =
{
BGFX_EMBEDDED_SHADER(Default_vs),
BGFX_EMBEDDED_SHADER(Ground_vs),
BGFX_EMBEDDED_SHADER(Quad_vs),
BGFX_EMBEDDED_SHADER(Render_vs),
BGFX_EMBEDDED_SHADER(Advect_cs),
BGFX_EMBEDDED_SHADER(Clear_cs),
BGFX_EMBEDDED_SHADER(DensityGen_cs),
BGFX_EMBEDDED_SHADER(Divergence_cs),
BGFX_EMBEDDED_SHADER(Gradient_cs),
BGFX_EMBEDDED_SHADER(Jacobi_cs),
BGFX_EMBEDDED_SHADER(VelocityGen_cs),
BGFX_EMBEDDED_SHADER(Vorticity_cs),
BGFX_EMBEDDED_SHADER(VorticityForce_cs),
BGFX_EMBEDDED_SHADER(Default_fs),
BGFX_EMBEDDED_SHADER(Ground_fs),
BGFX_EMBEDDED_SHADER(Render_fs),
BGFX_EMBEDDED_SHADER(RenderRT_fs),
BGFX_EMBEDDED_SHADER_END()
};
#include "ShaderHelper.h"
