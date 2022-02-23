static const uint8_t RenderRT_fs_spv[1115] =
{
	0x46, 0x53, 0x48, 0x0b, 0x6f, 0x1e, 0x3e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x0a, 0x73, // FSH.o.><.......s
	0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, // _texColor0......
	0x02, 0x1a, 0x00, 0x0d, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, // ....s_texVelocit
	0x79, 0x30, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x02, 0x1a, 0x00, 0x18, 0x04, 0x00, 0x00, 0x03, // y0..............
	0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x00, 0x08, 0x00, 0xb9, 0x00, 0x00, 0x00, 0x00, // .#..............
	0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x01, // ................
	0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, 0x30, 0x00, // ...GLSL.std.450.
	0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, // ................
	0x00, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, // ...........main.
	0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x04, // ...d...o........
	0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x00, 0x00, 0xf4, // ................
	0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, // ...........main.
	0x00, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00, 0x22, 0x00, 0x00, 0x00, 0x73, 0x5f, 0x74, 0x65, 0x78, // ......."...s_tex
	0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x00, 0x00, 0x00, 0x05, // ColorSampler....
	0x00, 0x07, 0x00, 0x25, 0x00, 0x00, 0x00, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x43, 0x6f, 0x6c, 0x6f, // ...%...s_texColo
	0x72, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08, 0x00, 0x29, // rTexture.......)
	0x00, 0x00, 0x00, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, // ...s_texVelocity
	0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x08, 0x00, 0x2b, // Sampler........+
	0x00, 0x00, 0x00, 0x73, 0x5f, 0x74, 0x65, 0x78, 0x56, 0x65, 0x6c, 0x6f, 0x63, 0x69, 0x74, 0x79, // ...s_texVelocity
	0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x64, // Texture........d
	0x00, 0x00, 0x00, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x00, 0x05, // ...v_texcoord0..
	0x00, 0x06, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, // ...o...bgfx_Frag
	0x44, 0x61, 0x74, 0x61, 0x30, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x22, // Data0..G..."..."
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x21, // .......G..."...!
	0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x22, // .......G...%..."
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x21, // .......G...%...!
	0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x29, 0x00, 0x00, 0x00, 0x22, // .......G...)..."
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x29, 0x00, 0x00, 0x00, 0x21, // .......G...)...!
	0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x22, // .......G...+..."
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x21, // .......G...+...!
	0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x64, 0x00, 0x00, 0x00, 0x1e, // .......G...d....
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x1e, // .......G...o....
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, // ...............!
	0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x02, 0x00, 0x06, // ................
	0x00, 0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x19, // ........... ....
	0x00, 0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x02, // ................
	0x00, 0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x04, // ................
	0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, // ... ...!........
	0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, // ...;...!..."....
	0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, // ... ...$........
	0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x24, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, // ...;...$...%....
	0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, // ...;...!...)....
	0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x24, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x00, // ...;...$...+....
	0x00, 0x00, 0x00, 0x1b, 0x00, 0x03, 0x00, 0x37, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x2b, // .......7.......+
	0x00, 0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x5a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x20, // .......Z......? 
	0x00, 0x04, 0x00, 0x63, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x3b, // ...c...........;
	0x00, 0x04, 0x00, 0x63, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, // ...c...d....... 
	0x00, 0x04, 0x00, 0x6e, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3b, // ...n...........;
	0x00, 0x04, 0x00, 0x6e, 0x00, 0x00, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x36, // ...n...o.......6
	0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, // ................
	0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x06, // ...........=....
	0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, // ...#..."...=....
	0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x06, // ...&...%...=....
	0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, // ...*...)...=....
	0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x0b, // ...,...+...=....
	0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x56, 0x00, 0x05, 0x00, 0x37, // ...e...d...V...7
	0x00, 0x00, 0x00, 0xad, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x57, // .......&...#...W
	0x00, 0x05, 0x00, 0x0d, 0x00, 0x00, 0x00, 0xaf, 0x00, 0x00, 0x00, 0xad, 0x00, 0x00, 0x00, 0x65, // ...............e
	0x00, 0x00, 0x00, 0x56, 0x00, 0x05, 0x00, 0x37, 0x00, 0x00, 0x00, 0xb6, 0x00, 0x00, 0x00, 0x2c, // ...V...7.......,
	0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x57, 0x00, 0x05, 0x00, 0x0d, 0x00, 0x00, 0x00, 0xb8, // ...*...W........
	0x00, 0x00, 0x00, 0xb6, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x07, // .......e...Q....
	0x00, 0x00, 0x00, 0x9a, 0x00, 0x00, 0x00, 0xaf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, // ...............Q
	0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00, 0x9d, 0x00, 0x00, 0x00, 0xb8, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00, 0x9e, 0x00, 0x00, 0x00, 0xb8, // ...Q............
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x9f, // .......P........
	0x00, 0x00, 0x00, 0x9a, 0x00, 0x00, 0x00, 0x9d, 0x00, 0x00, 0x00, 0x9e, 0x00, 0x00, 0x00, 0x5a, // ...............Z
	0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x6f, 0x00, 0x00, 0x00, 0x9f, 0x00, 0x00, 0x00, 0xfd, // ...>...o........
	0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,                               // ...8.......
};
