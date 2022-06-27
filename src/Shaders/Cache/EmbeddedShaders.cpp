#include "EmbeddedShaders.h"
#include "bgfx_utils.h"
static const bgfx::EmbeddedShader embeddedShaders[] =
{
BGFX_EMBEDDED_SHADER(Default_vs),
BGFX_EMBEDDED_SHADER(Ground_vs),
BGFX_EMBEDDED_SHADER(Quad_vs),
BGFX_EMBEDDED_SHADER(Render_vs),
BGFX_EMBEDDED_SHADER(RenderVolume_vs),
BGFX_EMBEDDED_SHADER(AdvectTile_cs),
BGFX_EMBEDDED_SHADER(AllocateCoarserTiles_cs),
BGFX_EMBEDDED_SHADER(AllocateTiles_cs),
BGFX_EMBEDDED_SHADER(Clear_cs),
BGFX_EMBEDDED_SHADER(ClearTiles_cs),
BGFX_EMBEDDED_SHADER(CommitFreeTiles_cs),
BGFX_EMBEDDED_SHADER(CopyTile_cs),
BGFX_EMBEDDED_SHADER(DensityGenTile_cs),
BGFX_EMBEDDED_SHADER(DilateTiles_cs),
BGFX_EMBEDDED_SHADER(DispatchIndirect_cs),
BGFX_EMBEDDED_SHADER(DivergenceTile_cs),
BGFX_EMBEDDED_SHADER(DownscaleTile_cs),
BGFX_EMBEDDED_SHADER(FreeTiles_cs),
BGFX_EMBEDDED_SHADER(GradientTile_cs),
BGFX_EMBEDDED_SHADER(InitTiles_cs),
BGFX_EMBEDDED_SHADER(JacobiTile_cs),
BGFX_EMBEDDED_SHADER(ResidualTile_cs),
BGFX_EMBEDDED_SHADER(UpscaleTile_cs),
BGFX_EMBEDDED_SHADER(VelocityGenTile_cs),
BGFX_EMBEDDED_SHADER(Vorticity_cs),
BGFX_EMBEDDED_SHADER(VorticityForce_cs),
BGFX_EMBEDDED_SHADER(buoyancyTile_cs),
BGFX_EMBEDDED_SHADER(Default_fs),
BGFX_EMBEDDED_SHADER(Ground_fs),
BGFX_EMBEDDED_SHADER(Render_fs),
BGFX_EMBEDDED_SHADER(RenderRT_fs),
BGFX_EMBEDDED_SHADER(RenderVolume_fs),
BGFX_EMBEDDED_SHADER_END()
};
#include "ShaderHelper.h"
