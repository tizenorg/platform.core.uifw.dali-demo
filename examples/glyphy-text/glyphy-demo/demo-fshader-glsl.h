static const char *demo_fshader_glsl =
"uniform float u_contrast;\n"
"uniform float u_gamma_adjust;\n"
"uniform float u_outline_thickness;\n"
"uniform float u_outline;\n"
"uniform float u_boldness;\n"
"\n"
"varying vec4 v_glyph;\n"
"varying float positionY;\n "// only for comparison in demo, draw up-half screen
"\n"
"\n"
//"#define SQRT2_2 0.70710678118654757 /* 1 / sqrt(2.) */\n"
//"#define SQRT2   1.4142135623730951\n"
"#define SQRT2_2 0.70711 /* 1 / sqrt(2.) */\n"
"#define SQRT2   1.41421\n"
"\n"
"struct glyph_info_t {\n"
"  ivec2 nominal_size;\n"
"  ivec2 atlas_pos;\n"
"};\n"
"\n"
"glyph_info_t\n"
"glyph_info_decode (vec4 v)\n"
"{\n"
"  glyph_info_t gi;\n"
"  gi.nominal_size = (ivec2 (mod (v.zw, 256.)) + 2) / 4;\n"
"  gi.atlas_pos = ivec2 (v_glyph.zw) / 256;\n"
"  return gi;\n"
"}\n"
"\n"
"\n"
"float\n"
"antialias (float d)\n"
"{\n"
"  return smoothstep (-.75, +.75, d);\n"
"}\n"
"\n"
"vec4\n"
"source_over (const vec4 src, const vec4 dst)\n"
"{\n"
"  // http://dev.w3.org/fxtf/compositing-1/#porterduffcompositingoperators_srcover\n"
"  float alpha = src.a + (dst.a * (1. - src.a));\n"
"  return vec4 (((src.rgb * src.a) + (dst.rgb * dst.a * (1. - src.a))) / alpha, alpha);\n"
"}\n"
"\n"
"void\n"
"main()\n"
"{\n"
"  vec2 p = v_glyph.xy;\n"
"  glyph_info_t gi = glyph_info_decode (v_glyph);\n"
"\n"
"  /* isotropic antialiasing */\n"
"  vec2 dpdx = dFdx (p);\n"
"  vec2 dpdy = dFdy (p);\n"
"  float m = length (vec2 (length (dpdx), length (dpdy))) * SQRT2_2;\n"
"\n"
"  vec4 color = uColor;\n"
"\n"
"  ivec4 uu_atlas_info = ivec4( u_atlas_info );"
"  float gsdist = glyphy_sdf (p, gi.nominal_size GLYPHY_DEMO_EXTRA_ARGS);\n"
"  float sdist = gsdist / m * u_contrast;\n"
"\n"
/*  codes for debug!
"    float gudist = abs (gsdist);\n"
"    float debug_color = 0.4;\n"
"    // Color the distance field red inside and green outside\n"
"    if (!glyphy_isinf (gudist))\n"
"      color = source_over (vec4 (debug_color * smoothstep (1., -1., sdist), debug_color * smoothstep (-1., 1., sdist), 0, 1. - gudist), color);\n"
"\n"
//"    glyphy_arc_list_t arc_list = glyphy_arc_list (p, gi.nominal_size GLYPHY_DEMO_EXTRA_ARGS);\n"
"    // Color the number of endpoints per cell blue\n"
//"    color = source_over (vec4 (0, 0, debug_color, float(arc_list.num_endpoints) / float(GLYPHY_MAX_NUM_ENDPOINTS)), color);\n"
"\n"
"    float pdist = glyphy_point_dist (p, gi.nominal_size GLYPHY_DEMO_EXTRA_ARGS);\n"
"    // Color points yellow\n"
"    color = source_over (vec4 (1, 1, 0, smoothstep (0.1, .05, pdist)), color);\n"*/

"    sdist -= u_boldness * 10.;\n"
"    if ( glyphy_iszero( u_outline ) )\n"
"      sdist = abs (sdist) - u_outline_thickness * .5;\n"
"    if (sdist > 1.)\n"
"      discard;\n"
"    float alpha = antialias (-sdist);\n"
"    if (u_gamma_adjust != 1.)\n"
"      alpha = pow (alpha, 1./u_gamma_adjust);\n"
"    color = vec4 (color.rgb,color.a * alpha);\n"
"\n"
"  gl_FragColor = color;\n"
// debug
/*"  int cell_offset = glyphy_arc_list_offset(p, gi.nominal_size);\n"
"  vec4 arc_list_data = glyphy_texture1D_func(cell_offset GLYPHY_DEMO_EXTRA_ARGS);\n"
"  gl_FragColor = arc_list_data * 50.0;\n"*/
		"  if( sign(positionY) < 0.0)  gl_FragColor = vec4(0.0);\n"		// only for comparison in demo, draw up-half screen
"}\n"
;
