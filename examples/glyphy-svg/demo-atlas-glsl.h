static const char *demo_atlas_glsl =
"uniform vec4 u_atlas_info;\n"
"\n"
"vec4\n"
"glyphy_texture1D_func (int offset, sampler2D _tex, ivec4 _atlas_info, ivec2 _atlas_pos)\n"
"{\n"
"  ivec2 item_geom = _atlas_info.zw;\n"
"  vec2 pos = (vec2 (_atlas_pos.xy * item_geom +\n"
"		   ivec2 (mod (float (offset), float (item_geom.x)), offset / item_geom.x)) +\n"
"	      + vec2 (.5, .5)) / vec2(_atlas_info.xy);\n"
"  return texture2D (_tex, pos);\n"
"}\n"
;
