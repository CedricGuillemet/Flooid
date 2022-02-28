static const uint8_t Jacobi_fs_mtl[1357] =
{
	0x46, 0x53, 0x48, 0x0b, 0x6f, 0x1e, 0x3e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x14, 0x73, // FSH.o.><.......s
	0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, 0x53, 0x61, 0x6d, 0x70, // _texVelocitySamp
	0x6c, 0x65, 0x72, 0x11, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x73, 0x5f, // ler...........s_
	0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, 0x54, 0x65, 0x78, 0x74, 0x75, // texVelocityTextu
	0x72, 0x65, 0x11, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x73, 0x5f, 0x74, // re...........s_t
	0x65, 0x78, 0x44, 0x65, 0x6e, 0x73, 0x69, 0x74, 0x79, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, // exDensitySampler
	0x11, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x73, 0x5f, 0x74, 0x65, 0x78, // ...........s_tex
	0x44, 0x65, 0x6e, 0x73, 0x69, 0x74, 0x79, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x11, 0x01, // DensityTexture..
	0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x6a, 0x61, 0x63, 0x6f, 0x62, 0x69, 0x50, // .........jacobiP
	0x61, 0x72, 0x61, 0x6d, 0x65, 0x74, 0x65, 0x72, 0x73, 0x12, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, // arameters.......
	0x00, 0x00, 0x00, 0x0d, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, // ....s_texVelocit
	0x79, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x73, 0x5f, 0x74, 0x65, // y...........s_te
	0x78, 0x44, 0x65, 0x6e, 0x73, 0x69, 0x74, 0x79, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // xDensity........
	0x00, 0x00, 0x73, 0x04, 0x00, 0x00, 0x23, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x3c, // ..s...#include <
	0x6d, 0x65, 0x74, 0x61, 0x6c, 0x5f, 0x73, 0x74, 0x64, 0x6c, 0x69, 0x62, 0x3e, 0x0a, 0x23, 0x69, // metal_stdlib>.#i
	0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x3c, 0x73, 0x69, 0x6d, 0x64, 0x2f, 0x73, 0x69, 0x6d, // nclude <simd/sim
	0x64, 0x2e, 0x68, 0x3e, 0x0a, 0x0a, 0x75, 0x73, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x61, 0x6d, 0x65, // d.h>..using name
	0x73, 0x70, 0x61, 0x63, 0x65, 0x20, 0x6d, 0x65, 0x74, 0x61, 0x6c, 0x3b, 0x0a, 0x0a, 0x73, 0x74, // space metal;..st
	0x72, 0x75, 0x63, 0x74, 0x20, 0x5f, 0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x0a, 0x7b, 0x0a, 0x20, // ruct _Global.{. 
	0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x6a, 0x61, 0x63, 0x6f, 0x62, 0x69, //    float4 jacobi
	0x50, 0x61, 0x72, 0x61, 0x6d, 0x65, 0x74, 0x65, 0x72, 0x73, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, 0x0a, // Parameters;.};..
	0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, // struct xlatMtlMa
	0x69, 0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, // in_out.{.    flo
	0x61, 0x74, 0x34, 0x20, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, 0x74, // at4 bgfx_FragDat
	0x61, 0x30, 0x20, 0x5b, 0x5b, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x3b, // a0 [[color(0)]];
	0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, // .};..struct xlat
	0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x69, 0x6e, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, // MtlMain_in.{.   
	0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, //  float2 v_texcoo
	0x72, 0x64, 0x30, 0x20, 0x5b, 0x5b, 0x75, 0x73, 0x65, 0x72, 0x28, 0x6c, 0x6f, 0x63, 0x6e, 0x30, // rd0 [[user(locn0
	0x29, 0x5d, 0x5d, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x66, 0x72, 0x61, 0x67, 0x6d, 0x65, 0x6e, // )]];.};..fragmen
	0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x6f, 0x75, // t xlatMtlMain_ou
	0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x28, 0x78, 0x6c, // t xlatMtlMain(xl
	0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x69, 0x6e, 0x20, 0x69, 0x6e, 0x20, // atMtlMain_in in 
	0x5b, 0x5b, 0x73, 0x74, 0x61, 0x67, 0x65, 0x5f, 0x69, 0x6e, 0x5d, 0x5d, 0x2c, 0x20, 0x63, 0x6f, // [[stage_in]], co
	0x6e, 0x73, 0x74, 0x61, 0x6e, 0x74, 0x20, 0x5f, 0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x26, 0x20, // nstant _Global& 
	0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x20, 0x5b, 0x5b, 0x62, 0x75, 0x66, 0x66, 0x65, 0x72, 0x28, // _mtl_u [[buffer(
	0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x64, 0x3c, // 0)]], texture2d<
	0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3e, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, // float> s_texVelo
	0x63, 0x69, 0x74, 0x79, 0x20, 0x5b, 0x5b, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x28, 0x30, // city [[texture(0
	0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x64, 0x3c, 0x66, // )]], texture2d<f
	0x6c, 0x6f, 0x61, 0x74, 0x3e, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x44, 0x65, 0x6e, 0x73, 0x69, // loat> s_texDensi
	0x74, 0x79, 0x20, 0x5b, 0x5b, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x28, 0x31, 0x29, 0x5d, // ty [[texture(1)]
	0x5d, 0x2c, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, // ], sampler s_tex
	0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, // VelocitySampler 
	0x5b, 0x5b, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, // [[sampler(0)]], 
	0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x44, 0x65, 0x6e, // sampler s_texDen
	0x73, 0x69, 0x74, 0x79, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x5b, 0x5b, 0x73, 0x61, // sitySampler [[sa
	0x6d, 0x70, 0x6c, 0x65, 0x72, 0x28, 0x31, 0x29, 0x5d, 0x5d, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, // mpler(1)]]).{.  
	0x20, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x6f, 0x75, //   xlatMtlMain_ou
	0x74, 0x20, 0x6f, 0x75, 0x74, 0x20, 0x3d, 0x20, 0x7b, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, // t out = {};.    
	0x6f, 0x75, 0x74, 0x2e, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, 0x74, // out.bgfx_FragDat
	0x61, 0x30, 0x20, 0x3d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x28, 0x28, 0x28, 0x28, 0x28, // a0 = float4(((((
	0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, 0x2e, 0x73, 0x61, // s_texVelocity.sa
	0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, // mple(s_texVeloci
	0x74, 0x79, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x28, 0x69, 0x6e, 0x2e, 0x76, // tySampler, (in.v
	0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2d, 0x20, 0x66, 0x6c, 0x6f, // _texcoord0 - flo
	0x61, 0x74, 0x32, 0x28, 0x30, 0x2e, 0x30, 0x30, 0x33, 0x39, 0x30, 0x36, 0x32, 0x35, 0x2c, 0x20, // at2(0.00390625, 
	0x30, 0x2e, 0x30, 0x29, 0x29, 0x29, 0x2e, 0x78, 0x20, 0x2b, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, // 0.0))).x + s_tex
	0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, // Velocity.sample(
	0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, 0x53, 0x61, 0x6d, // s_texVelocitySam
	0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x28, 0x69, 0x6e, 0x2e, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, // pler, (in.v_texc
	0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x30, // oord0 + float2(0
	0x2e, 0x30, 0x30, 0x33, 0x39, 0x30, 0x36, 0x32, 0x35, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x29, 0x29, // .00390625, 0.0))
	0x29, 0x2e, 0x78, 0x29, 0x20, 0x2b, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, // ).x) + s_texVelo
	0x63, 0x69, 0x74, 0x79, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x74, 0x65, // city.sample(s_te
	0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, // xVelocitySampler
	0x2c, 0x20, 0x28, 0x69, 0x6e, 0x2e, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, // , (in.v_texcoord
	0x30, 0x20, 0x2d, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x30, 0x2e, 0x30, 0x2c, 0x20, // 0 - float2(0.0, 
	0x30, 0x2e, 0x30, 0x30, 0x33, 0x39, 0x30, 0x36, 0x32, 0x35, 0x29, 0x29, 0x29, 0x2e, 0x78, 0x29, // 0.00390625))).x)
	0x20, 0x2b, 0x20, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, //  + s_texVelocity
	0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, // .sample(s_texVel
	0x6f, 0x63, 0x69, 0x74, 0x79, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x28, 0x69, // ocitySampler, (i
	0x6e, 0x2e, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x2b, 0x20, // n.v_texcoord0 + 
	0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x28, 0x30, 0x2e, 0x30, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x30, // float2(0.0, 0.00
	0x33, 0x39, 0x30, 0x36, 0x32, 0x35, 0x29, 0x29, 0x29, 0x2e, 0x78, 0x29, 0x20, 0x2b, 0x20, 0x28, // 390625))).x) + (
	0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, 0x6a, 0x61, 0x63, 0x6f, 0x62, 0x69, 0x50, 0x61, 0x72, // _mtl_u.jacobiPar
	0x61, 0x6d, 0x65, 0x74, 0x65, 0x72, 0x73, 0x2e, 0x78, 0x20, 0x2a, 0x20, 0x73, 0x5f, 0x74, 0x65, // ameters.x * s_te
	0x78, 0x44, 0x65, 0x6e, 0x73, 0x69, 0x74, 0x79, 0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, // xDensity.sample(
	0x73, 0x5f, 0x74, 0x65, 0x78, 0x44, 0x65, 0x6e, 0x73, 0x69, 0x74, 0x79, 0x53, 0x61, 0x6d, 0x70, // s_texDensitySamp
	0x6c, 0x65, 0x72, 0x2c, 0x20, 0x69, 0x6e, 0x2e, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, // ler, in.v_texcoo
	0x72, 0x64, 0x30, 0x29, 0x2e, 0x78, 0x29, 0x29, 0x20, 0x2f, 0x20, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, // rd0).x)) / _mtl_
	0x75, 0x2e, 0x6a, 0x61, 0x63, 0x6f, 0x62, 0x69, 0x50, 0x61, 0x72, 0x61, 0x6d, 0x65, 0x74, 0x65, // u.jacobiParamete
	0x72, 0x73, 0x2e, 0x79, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x2c, 0x20, // rs.y, 0.0, 0.0, 
	0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, // 1.0);.    return
	0x20, 0x6f, 0x75, 0x74, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00, 0x00, 0x50, 0x00,                   //  out;.}....P.
};
