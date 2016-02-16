static std::string fragmentShaderSource = MAKE_STRING(
uniform vec4 uAtlasInfo;\n
uniform vec2 uGradientTexturePos;\n
uniform sampler2D  sSvgAtlas;\n
varying vec4 v_glyph;\n
varying vec4 v_fill_color;\n
varying vec4 v_stroke_color;\n
varying float v_stroke_width;\n
varying vec3 v_texture_coord;\n
varying float v_gradient_type;\n
\n
vec4 source_over (const vec4 src, const vec4 dst)\n
{\n
  // http://dev.w3.org/fxtf/compositing-1/#porterduffcompositingoperators_srcover\n
  float alpha = mix(dst.a, 1., src.a);\n
  return vec4( mix(dst.rgb * dst.a, src.rgb,  src.a) / alpha, alpha );\n
}\n
\n
void main()\n
{\n
  vec2 p = v_glyph.xy;\n
  // decode glyph info
  ivec2 nominal_size = (ivec2 (mod (v_glyph.zw, 256.)) + 2) / 4;\n
  ivec2 atlas_pos = ivec2 (v_glyph.zw) / 256;\n
\n
  // isotropic antialiasing \n
  vec2 dpdx = dFdx (p);\n
  vec2 dpdy = dFdy (p);\n
  float m = length (vec2 (length (dpdx), length (dpdy))) * 0.7071068;\n // #define SQRT2_2 0.7071068  = 1 / sqrt(2.) \n"
  float sdist = glyphy_sdf (p, nominal_size, sSvgAtlas, uAtlasInfo, atlas_pos) / m;\n
\n
  vec4 fill_color = vec4(0.0);\n
  if (sdist < 1.){\n
    float alpha = smoothstep (-.75, +.75, -sdist);\n
    if(v_gradient_type < 0.5 ){\n   //solid color
      fill_color = v_fill_color;
      fill_color.a *=  alpha;}\n
    else {\n // gradient color
      fill_color = texture2D(sSvgAtlas, uGradientTexturePos
          +vec2(clamp(length(v_texture_coord.xy), 0.0,uAtlasInfo.z/uAtlasInfo.x), v_texture_coord.z) ); \n
      fill_color.a *=  v_fill_color.a * alpha;}\n
   }\n
   gl_FragColor = fill_color;\n
   \n
   sdist = abs (sdist) - v_stroke_width * .5;\n
   if( !glyphy_iszero( v_stroke_color.a ) && sdist < 1. )\n
   {\n
      float alpha = smoothstep (-.75, +.75, -sdist);\n
      vec4 stroke_color = vec4 (v_stroke_color.rgb, v_stroke_color.a * alpha);
      gl_FragColor = source_over( stroke_color, fill_color);
   }\n
\n
}\n
);
