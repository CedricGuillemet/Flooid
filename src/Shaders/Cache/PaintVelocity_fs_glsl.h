static const uint8_t PaintVelocity_fs_glsl[470] =
{
	0x46, 0x53, 0x48, 0x0b, 0x6f, 0x1e, 0x3e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x05, 0x62, // FSH.o.><.......b
	0x72, 0x75, 0x73, 0x68, 0x02, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x62, // rush...........b
	0x72, 0x75, 0x73, 0x68, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x02, 0x01, 0x00, // rushDirection...
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9a, 0x01, 0x00, 0x00, 0x76, 0x61, 0x72, 0x79, 0x69, // ...........varyi
	0x6e, 0x67, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, // ng vec2 v_texcoo
	0x72, 0x64, 0x30, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x76, 0x65, 0x63, // rd0;.uniform vec
	0x34, 0x20, 0x62, 0x72, 0x75, 0x73, 0x68, 0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, // 4 brush;.uniform
	0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x62, 0x72, 0x75, 0x73, 0x68, 0x44, 0x69, 0x72, 0x65, 0x63, //  vec4 brushDirec
	0x74, 0x69, 0x6f, 0x6e, 0x3b, 0x0a, 0x76, 0x6f, 0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, // tion;.void main 
	0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x32, 0x20, 0x74, 0x6d, 0x70, 0x76, // ().{.  vec2 tmpv
	0x61, 0x72, 0x5f, 0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, // ar_1;.  tmpvar_1
	0x20, 0x3d, 0x20, 0x28, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, //  = (v_texcoord0 
	0x2d, 0x20, 0x62, 0x72, 0x75, 0x73, 0x68, 0x2e, 0x78, 0x79, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x66, // - brush.xy);.  f
	0x6c, 0x6f, 0x61, 0x74, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x3b, 0x0a, 0x20, // loat tmpvar_2;. 
	0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x20, 0x3d, 0x20, 0x28, 0x31, 0x2e, 0x30, //  tmpvar_2 = (1.0
	0x20, 0x2d, 0x20, 0x28, 0x73, 0x71, 0x72, 0x74, 0x28, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x64, 0x6f, //  - (sqrt(.    do
	0x74, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2c, 0x20, 0x74, 0x6d, 0x70, // t (tmpvar_1, tmp
	0x76, 0x61, 0x72, 0x5f, 0x31, 0x29, 0x0a, 0x20, 0x20, 0x29, 0x20, 0x2f, 0x20, 0x62, 0x72, 0x75, // var_1).  ) / bru
	0x73, 0x68, 0x2e, 0x7a, 0x29, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, // sh.z));.  vec4 t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // mpvar_3;.  tmpva
	0x72, 0x5f, 0x33, 0x2e, 0x7a, 0x20, 0x3d, 0x20, 0x30, 0x2e, 0x30, 0x3b, 0x0a, 0x20, 0x20, 0x74, // r_3.z = 0.0;.  t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x2e, 0x78, 0x79, 0x20, 0x3d, 0x20, 0x28, 0x62, 0x72, // mpvar_3.xy = (br
	0x75, 0x73, 0x68, 0x44, 0x69, 0x72, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x78, 0x79, 0x20, // ushDirection.xy 
	0x2a, 0x20, 0x31, 0x30, 0x30, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, // * 100.0);.  tmpv
	0x61, 0x72, 0x5f, 0x33, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // ar_3.w = tmpvar_
	0x32, 0x3b, 0x0a, 0x20, 0x20, 0x67, 0x6c, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, // 2;.  gl_FragColo
	0x72, 0x20, 0x3d, 0x20, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x33, 0x20, 0x2a, 0x20, // r = (tmpvar_3 * 
	0x6d, 0x61, 0x78, 0x20, 0x28, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x32, 0x20, 0x2a, // max ((tmpvar_2 *
	0x20, 0x62, 0x72, 0x75, 0x73, 0x68, 0x2e, 0x77, 0x29, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x29, 0x29, //  brush.w), 0.0))
	0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,                                                             // ;.}...
};
