static std::string fragmentShaderSource = MAKE_STRING(
uniform vec4 uAtlasInfo;\n
uniform sampler2D  sGlyphyAtlas;\n
uniform sampler2D  sGradientLookup;\n
varying vec4 v_glyph;\n
varying vec4 v_fill_color;\n
varying vec4 v_stroke_color;\n
varying float v_stroke_width;\n
varying vec3 v_texture_coord;\n
varying float v_gradient_type;\n
\n
\n
struct glyph_info_t {\n
  ivec2 nominal_size;\n
  ivec2 atlas_pos;\n
};\n
\n
glyph_info_t glyph_info_decode (vec4 v)\n
{\n
  glyph_info_t gi;\n
  gi.nominal_size = (ivec2 (mod (v.zw, 256.)) + 2) / 4;\n
  gi.atlas_pos = ivec2 (v_glyph.zw) / 256;\n
  return gi;\n
}\n
\n
float antialias (float d)\n
{\n
  return smoothstep (-.75, +.75, d);\n
}\n
\n
vec4 source_over (const vec4 src, const vec4 dst)\n
{\n
  // http://dev.w3.org/fxtf/compositing-1/#porterduffcompositingoperators_srcover\n
  float alpha = src.a + (dst.a * (1. - src.a));\n
  return vec4 (((src.rgb * src.a) + (dst.rgb * dst.a * (1. - src.a))) / alpha, alpha);\n
}\n
\n
void main()\n
{\n
  vec2 p = v_glyph.xy;\n
  glyph_info_t gi = glyph_info_decode (v_glyph);\n
\n
  // isotropic antialiasing \n
  vec2 dpdx = dFdx (p);\n
  vec2 dpdy = dFdy (p);\n
  float m = length (vec2 (length (dpdx), length (dpdy))) * 0.7071068;\n // #define SQRT2_2 0.7071068  = 1 / sqrt(2.) \n"
\n
  ivec4 uu_atlas_info = ivec4( uAtlasInfo );
  float gsdist = glyphy_sdf (p, gi.nominal_size, sGlyphyAtlas, uu_atlas_info, gi.atlas_pos);\n
  float sdist = gsdist / m;\n
\n
  vec4 fill_color = vec4(0.0);\n
  if (sdist < 1.){\n
    float alpha = antialias (-sdist);\n
    if(v_gradient_type < 0.5 ){\n   //solid color
      fill_color = v_fill_color;}\n
    else if(v_gradient_type < 1.5) {\n  // linear gradient color
      fill_color = texture2D(sGradientLookup, vec2(clamp(v_texture_coord.y, 0.0,1.0),v_texture_coord.z) ); }\n
    else{\n // radial gradient color
      fill_color = texture2D(sGradientLookup, vec2(clamp(sqrt(v_texture_coord.x*v_texture_coord.x + v_texture_coord.y*v_texture_coord.y), 0.0,1.0), v_texture_coord.z) ); }\n
    fill_color.a *=  v_fill_color.a * alpha;\n
   }\n
\n
  vec4 stroke_color = vec4(0.0);\n
  sdist = abs (sdist) - v_stroke_width * .5;\n
  if (sdist < 1.){\n
    float alpha = antialias (-sdist);\n
    stroke_color = vec4 (v_stroke_color.rgb, v_stroke_color.a * alpha);}\n
\n
  gl_FragColor = source_over( stroke_color, fill_color);\n
}\n
);
