static const char *demo_fshader_glsl =
"\n"
"varying vec4 v_glyph;\n"
"varying vec4 v_fill_color;\n"
"varying vec4 v_stroke_color;\n"
"varying float v_stroke_width;\n"
"varying vec2 v_position;\n"
"varying float v_gradient_index;\n"
"varying vec3 v_gradient_info_x;\n"
"varying vec3 v_gradient_info_y;\n"
"\n"
"\n"
"#define SQRT2_2 0.7071068 /* 1 / sqrt(2.) */\n"
"#define SQRT2   1.4142136\n"
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
"  ivec4 uu_atlas_info = ivec4( u_atlas_info );"
"  float gsdist = glyphy_sdf (p, gi.nominal_size GLYPHY_DEMO_EXTRA_ARGS);\n"
"  float sdist = gsdist / m;\n"
"\n"
"\n"
"  vec4 fill_color = vec4(0.0);\n"
"  if (sdist < 1.){\n"
"    float alpha = antialias (-sdist);\n"
"    if(v_gradient_index < 0. ){\n"
"      fill_color = vec4 (v_fill_color.rgb, v_fill_color.a * alpha);}\n"
"    else{\n"
"      float _y = v_gradient_info_y[0]*v_position[0]-v_gradient_info_y[1]*v_position[1]+v_gradient_info_y[2];\n"
"      float lookup_x;"
"      if( abs(v_gradient_info_x[0])+abs(v_gradient_info_x[1])+ abs(v_gradient_info_x[2])< GLYPHY_EPSILON )\n"
"      { lookup_x = clamp(_y, 0.0, 1.0);}\n "
"      else{\n"
"        float _x = v_gradient_info_x[0]*v_position[0]-v_gradient_info_x[1]*v_position[1]+v_gradient_info_x[2];\n"
"        lookup_x = clamp(sqrt(_x*_x + _y*_y), 0.0,1.0) ;}\n "
"      float lookup_y = (v_gradient_index*6.0+3.0) / float(GRADIENT_INFO_COUNT);\n"
"      fill_color = texture2D(sEffect, vec2(lookup_x, lookup_y));\n"
"      fill_color.a *=  v_fill_color.a * alpha;}\n"
"   }\n"
"\n"
"  vec4 stroke_color = vec4(0.0);\n"
"  sdist = abs (sdist) - v_stroke_width * .5;\n"
"  if (sdist < 1.){\n"
"    float alpha = antialias (-sdist);\n"
"    stroke_color = vec4 (v_stroke_color.rgb, v_stroke_color.a * alpha);}\n"
"\n"
"  gl_FragColor = source_over( stroke_color, fill_color);\n"
"}\n"
;
