// generated by CMake
#include <stdint.h>
#include <bgfx/embedded_shader.h>
#ifdef WIN32
#include "Default_vs_dx9.h"
#include "Default_vs_dx11.h"
#endif
#include "Default_vs_glsl.h"
#ifdef __APPLE__
#include "Default_vs_mtl.h"
#endif
#include "Default_vs_spv.h"
#include "Default_vs_essl.h"
#ifdef WIN32
#include "Ground_vs_dx9.h"
#include "Ground_vs_dx11.h"
#endif
#include "Ground_vs_glsl.h"
#ifdef __APPLE__
#include "Ground_vs_mtl.h"
#endif
#include "Ground_vs_spv.h"
#include "Ground_vs_essl.h"
#ifdef WIN32
#include "Quad_vs_dx9.h"
#include "Quad_vs_dx11.h"
#endif
#include "Quad_vs_glsl.h"
#ifdef __APPLE__
#include "Quad_vs_mtl.h"
#endif
#include "Quad_vs_spv.h"
#include "Quad_vs_essl.h"
#ifdef WIN32
#include "Render_vs_dx9.h"
#include "Render_vs_dx11.h"
#endif
#include "Render_vs_glsl.h"
#ifdef __APPLE__
#include "Render_vs_mtl.h"
#endif
#include "Render_vs_spv.h"
#include "Render_vs_essl.h"
#ifdef WIN32
#include "RenderVolume_vs_dx9.h"
#include "RenderVolume_vs_dx11.h"
#endif
#include "RenderVolume_vs_glsl.h"
#ifdef __APPLE__
#include "RenderVolume_vs_mtl.h"
#endif
#include "RenderVolume_vs_spv.h"
#include "RenderVolume_vs_essl.h"
#ifdef WIN32
#include "Advect_cs_dx9.h"
#include "Advect_cs_dx11.h"
#endif
#include "Advect_cs_glsl.h"
#ifdef __APPLE__
#include "Advect_cs_mtl.h"
#endif
#include "Advect_cs_spv.h"
#include "Advect_cs_essl.h"
#ifdef WIN32
#include "AdvectPaged_cs_dx9.h"
#include "AdvectPaged_cs_dx11.h"
#endif
#include "AdvectPaged_cs_glsl.h"
#ifdef __APPLE__
#include "AdvectPaged_cs_mtl.h"
#endif
#include "AdvectPaged_cs_spv.h"
#include "AdvectPaged_cs_essl.h"
#ifdef WIN32
#include "AllocatePages_cs_dx9.h"
#include "AllocatePages_cs_dx11.h"
#endif
#include "AllocatePages_cs_glsl.h"
#ifdef __APPLE__
#include "AllocatePages_cs_mtl.h"
#endif
#include "AllocatePages_cs_spv.h"
#include "AllocatePages_cs_essl.h"
#ifdef WIN32
#include "AllocateSubPages_cs_dx9.h"
#include "AllocateSubPages_cs_dx11.h"
#endif
#include "AllocateSubPages_cs_glsl.h"
#ifdef __APPLE__
#include "AllocateSubPages_cs_mtl.h"
#endif
#include "AllocateSubPages_cs_spv.h"
#include "AllocateSubPages_cs_essl.h"
#ifdef WIN32
#include "Clear_cs_dx9.h"
#include "Clear_cs_dx11.h"
#endif
#include "Clear_cs_glsl.h"
#ifdef __APPLE__
#include "Clear_cs_mtl.h"
#endif
#include "Clear_cs_spv.h"
#include "Clear_cs_essl.h"
#ifdef WIN32
#include "CommitFreePages_cs_dx9.h"
#include "CommitFreePages_cs_dx11.h"
#endif
#include "CommitFreePages_cs_glsl.h"
#ifdef __APPLE__
#include "CommitFreePages_cs_mtl.h"
#endif
#include "CommitFreePages_cs_spv.h"
#include "CommitFreePages_cs_essl.h"
#ifdef WIN32
#include "DensityGen_cs_dx9.h"
#include "DensityGen_cs_dx11.h"
#endif
#include "DensityGen_cs_glsl.h"
#ifdef __APPLE__
#include "DensityGen_cs_mtl.h"
#endif
#include "DensityGen_cs_spv.h"
#include "DensityGen_cs_essl.h"
#ifdef WIN32
#include "DensityGenPaged_cs_dx9.h"
#include "DensityGenPaged_cs_dx11.h"
#endif
#include "DensityGenPaged_cs_glsl.h"
#ifdef __APPLE__
#include "DensityGenPaged_cs_mtl.h"
#endif
#include "DensityGenPaged_cs_spv.h"
#include "DensityGenPaged_cs_essl.h"
#ifdef WIN32
#include "DilatePages_cs_dx9.h"
#include "DilatePages_cs_dx11.h"
#endif
#include "DilatePages_cs_glsl.h"
#ifdef __APPLE__
#include "DilatePages_cs_mtl.h"
#endif
#include "DilatePages_cs_spv.h"
#include "DilatePages_cs_essl.h"
#ifdef WIN32
#include "DispatchIndirect_cs_dx9.h"
#include "DispatchIndirect_cs_dx11.h"
#endif
#include "DispatchIndirect_cs_glsl.h"
#ifdef __APPLE__
#include "DispatchIndirect_cs_mtl.h"
#endif
#include "DispatchIndirect_cs_spv.h"
#include "DispatchIndirect_cs_essl.h"
#ifdef WIN32
#include "Divergence_cs_dx9.h"
#include "Divergence_cs_dx11.h"
#endif
#include "Divergence_cs_glsl.h"
#ifdef __APPLE__
#include "Divergence_cs_mtl.h"
#endif
#include "Divergence_cs_spv.h"
#include "Divergence_cs_essl.h"
#ifdef WIN32
#include "DivergencePaged_cs_dx9.h"
#include "DivergencePaged_cs_dx11.h"
#endif
#include "DivergencePaged_cs_glsl.h"
#ifdef __APPLE__
#include "DivergencePaged_cs_mtl.h"
#endif
#include "DivergencePaged_cs_spv.h"
#include "DivergencePaged_cs_essl.h"
#ifdef WIN32
#include "Downscale_cs_dx9.h"
#include "Downscale_cs_dx11.h"
#endif
#include "Downscale_cs_glsl.h"
#ifdef __APPLE__
#include "Downscale_cs_mtl.h"
#endif
#include "Downscale_cs_spv.h"
#include "Downscale_cs_essl.h"
#ifdef WIN32
#include "DownscalePaged_cs_dx9.h"
#include "DownscalePaged_cs_dx11.h"
#endif
#include "DownscalePaged_cs_glsl.h"
#ifdef __APPLE__
#include "DownscalePaged_cs_mtl.h"
#endif
#include "DownscalePaged_cs_spv.h"
#include "DownscalePaged_cs_essl.h"
#ifdef WIN32
#include "FrameInit_cs_dx9.h"
#include "FrameInit_cs_dx11.h"
#endif
#include "FrameInit_cs_glsl.h"
#ifdef __APPLE__
#include "FrameInit_cs_mtl.h"
#endif
#include "FrameInit_cs_spv.h"
#include "FrameInit_cs_essl.h"
#ifdef WIN32
#include "FreePages_cs_dx9.h"
#include "FreePages_cs_dx11.h"
#endif
#include "FreePages_cs_glsl.h"
#ifdef __APPLE__
#include "FreePages_cs_mtl.h"
#endif
#include "FreePages_cs_spv.h"
#include "FreePages_cs_essl.h"
#ifdef WIN32
#include "Gradient_cs_dx9.h"
#include "Gradient_cs_dx11.h"
#endif
#include "Gradient_cs_glsl.h"
#ifdef __APPLE__
#include "Gradient_cs_mtl.h"
#endif
#include "Gradient_cs_spv.h"
#include "Gradient_cs_essl.h"
#ifdef WIN32
#include "GradientPaged_cs_dx9.h"
#include "GradientPaged_cs_dx11.h"
#endif
#include "GradientPaged_cs_glsl.h"
#ifdef __APPLE__
#include "GradientPaged_cs_mtl.h"
#endif
#include "GradientPaged_cs_spv.h"
#include "GradientPaged_cs_essl.h"
#ifdef WIN32
#include "InitPages_cs_dx9.h"
#include "InitPages_cs_dx11.h"
#endif
#include "InitPages_cs_glsl.h"
#ifdef __APPLE__
#include "InitPages_cs_mtl.h"
#endif
#include "InitPages_cs_spv.h"
#include "InitPages_cs_essl.h"
#ifdef WIN32
#include "Jacobi_cs_dx9.h"
#include "Jacobi_cs_dx11.h"
#endif
#include "Jacobi_cs_glsl.h"
#ifdef __APPLE__
#include "Jacobi_cs_mtl.h"
#endif
#include "Jacobi_cs_spv.h"
#include "Jacobi_cs_essl.h"
#ifdef WIN32
#include "JacobiPaged_cs_dx9.h"
#include "JacobiPaged_cs_dx11.h"
#endif
#include "JacobiPaged_cs_glsl.h"
#ifdef __APPLE__
#include "JacobiPaged_cs_mtl.h"
#endif
#include "JacobiPaged_cs_spv.h"
#include "JacobiPaged_cs_essl.h"
#ifdef WIN32
#include "Residual_cs_dx9.h"
#include "Residual_cs_dx11.h"
#endif
#include "Residual_cs_glsl.h"
#ifdef __APPLE__
#include "Residual_cs_mtl.h"
#endif
#include "Residual_cs_spv.h"
#include "Residual_cs_essl.h"
#ifdef WIN32
#include "ResidualPaged_cs_dx9.h"
#include "ResidualPaged_cs_dx11.h"
#endif
#include "ResidualPaged_cs_glsl.h"
#ifdef __APPLE__
#include "ResidualPaged_cs_mtl.h"
#endif
#include "ResidualPaged_cs_spv.h"
#include "ResidualPaged_cs_essl.h"
#ifdef WIN32
#include "Upscale_cs_dx9.h"
#include "Upscale_cs_dx11.h"
#endif
#include "Upscale_cs_glsl.h"
#ifdef __APPLE__
#include "Upscale_cs_mtl.h"
#endif
#include "Upscale_cs_spv.h"
#include "Upscale_cs_essl.h"
#ifdef WIN32
#include "VelocityGen_cs_dx9.h"
#include "VelocityGen_cs_dx11.h"
#endif
#include "VelocityGen_cs_glsl.h"
#ifdef __APPLE__
#include "VelocityGen_cs_mtl.h"
#endif
#include "VelocityGen_cs_spv.h"
#include "VelocityGen_cs_essl.h"
#ifdef WIN32
#include "VelocityGenPaged_cs_dx9.h"
#include "VelocityGenPaged_cs_dx11.h"
#endif
#include "VelocityGenPaged_cs_glsl.h"
#ifdef __APPLE__
#include "VelocityGenPaged_cs_mtl.h"
#endif
#include "VelocityGenPaged_cs_spv.h"
#include "VelocityGenPaged_cs_essl.h"
#ifdef WIN32
#include "Vorticity_cs_dx9.h"
#include "Vorticity_cs_dx11.h"
#endif
#include "Vorticity_cs_glsl.h"
#ifdef __APPLE__
#include "Vorticity_cs_mtl.h"
#endif
#include "Vorticity_cs_spv.h"
#include "Vorticity_cs_essl.h"
#ifdef WIN32
#include "VorticityForce_cs_dx9.h"
#include "VorticityForce_cs_dx11.h"
#endif
#include "VorticityForce_cs_glsl.h"
#ifdef __APPLE__
#include "VorticityForce_cs_mtl.h"
#endif
#include "VorticityForce_cs_spv.h"
#include "VorticityForce_cs_essl.h"
#ifdef WIN32
#include "Default_fs_dx9.h"
#include "Default_fs_dx11.h"
#endif
#include "Default_fs_glsl.h"
#ifdef __APPLE__
#include "Default_fs_mtl.h"
#endif
#include "Default_fs_spv.h"
#include "Default_fs_essl.h"
#ifdef WIN32
#include "Ground_fs_dx9.h"
#include "Ground_fs_dx11.h"
#endif
#include "Ground_fs_glsl.h"
#ifdef __APPLE__
#include "Ground_fs_mtl.h"
#endif
#include "Ground_fs_spv.h"
#include "Ground_fs_essl.h"
#ifdef WIN32
#include "Render_fs_dx9.h"
#include "Render_fs_dx11.h"
#endif
#include "Render_fs_glsl.h"
#ifdef __APPLE__
#include "Render_fs_mtl.h"
#endif
#include "Render_fs_spv.h"
#include "Render_fs_essl.h"
#ifdef WIN32
#include "RenderRT_fs_dx9.h"
#include "RenderRT_fs_dx11.h"
#endif
#include "RenderRT_fs_glsl.h"
#ifdef __APPLE__
#include "RenderRT_fs_mtl.h"
#endif
#include "RenderRT_fs_spv.h"
#include "RenderRT_fs_essl.h"
#ifdef WIN32
#include "RenderVolume_fs_dx9.h"
#include "RenderVolume_fs_dx11.h"
#endif
#include "RenderVolume_fs_glsl.h"
#ifdef __APPLE__
#include "RenderVolume_fs_mtl.h"
#endif
#include "RenderVolume_fs_spv.h"
#include "RenderVolume_fs_essl.h"
