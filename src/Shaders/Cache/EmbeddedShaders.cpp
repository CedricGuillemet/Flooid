#include "EmbeddedShaders.h"
#include "bgfx_utils.h"
static const bgfx::EmbeddedShader embeddedShaders[] =
{
BGFX_EMBEDDED_SHADER(Default_vs),
BGFX_EMBEDDED_SHADER(Quad_vs),
BGFX_EMBEDDED_SHADER(TestCompute_cs),
BGFX_EMBEDDED_SHADER(Advect_fs),
BGFX_EMBEDDED_SHADER(Default_fs),
BGFX_EMBEDDED_SHADER(Divergence_fs),
BGFX_EMBEDDED_SHADER(Gradient_fs),
BGFX_EMBEDDED_SHADER(Jacobi_fs),
BGFX_EMBEDDED_SHADER(PaintDensity_fs),
BGFX_EMBEDDED_SHADER(PaintVelocity_fs),
BGFX_EMBEDDED_SHADER(RenderRT_fs),
BGFX_EMBEDDED_SHADER_END()
};
#include "ShaderHelper.h"
