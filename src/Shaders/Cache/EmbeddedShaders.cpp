#include "EmbeddedShaders.h"
#include "bgfx_utils.h"
static const bgfx::EmbeddedShader embeddedShaders[] =
{
BGFX_EMBEDDED_SHADER(Default_vs),
BGFX_EMBEDDED_SHADER(Ground_vs),
BGFX_EMBEDDED_SHADER(Quad_vs),
BGFX_EMBEDDED_SHADER(Render_vs),
BGFX_EMBEDDED_SHADER(RenderVolume_vs),
BGFX_EMBEDDED_SHADER(Advect_cs),
BGFX_EMBEDDED_SHADER(AllocatePages_cs),
BGFX_EMBEDDED_SHADER(Clear_cs),
BGFX_EMBEDDED_SHADER(DensityGen_cs),
BGFX_EMBEDDED_SHADER(DensityGenPaged_cs),
BGFX_EMBEDDED_SHADER(DilatePages_cs),
BGFX_EMBEDDED_SHADER(Divergence_cs),
BGFX_EMBEDDED_SHADER(DivergencePaged_cs),
BGFX_EMBEDDED_SHADER(Downscale_cs),
BGFX_EMBEDDED_SHADER(FreePages_cs),
BGFX_EMBEDDED_SHADER(Gradient_cs),
BGFX_EMBEDDED_SHADER(GradientPaged_cs),
BGFX_EMBEDDED_SHADER(InitPages_cs),
BGFX_EMBEDDED_SHADER(Jacobi_cs),
BGFX_EMBEDDED_SHADER(JacobiPaged_cs),
BGFX_EMBEDDED_SHADER(Residual_cs),
BGFX_EMBEDDED_SHADER(Upscale_cs),
BGFX_EMBEDDED_SHADER(VelocityGen_cs),
BGFX_EMBEDDED_SHADER(VelocityGenPaged_cs),
BGFX_EMBEDDED_SHADER(Vorticity_cs),
BGFX_EMBEDDED_SHADER(VorticityForce_cs),
BGFX_EMBEDDED_SHADER(Default_fs),
BGFX_EMBEDDED_SHADER(Ground_fs),
BGFX_EMBEDDED_SHADER(Render_fs),
BGFX_EMBEDDED_SHADER(RenderRT_fs),
BGFX_EMBEDDED_SHADER(RenderVolume_fs),
BGFX_EMBEDDED_SHADER_END()
};
#include "ShaderHelper.h"
