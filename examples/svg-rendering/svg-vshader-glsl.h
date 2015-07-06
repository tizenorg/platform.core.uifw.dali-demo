static std::string vertexShaderSource = MAKE_STRING(
attribute vec2 aPosition;\n
attribute vec2 aGlyphyInfo;\n
attribute vec4 aFillColor;\n
attribute vec4 aStrokeColot;\n
attribute vec3 aGradientParamX;\n
attribute vec3 aGradientParamY;\n
attribute float aStrokeWidth;\n
attribute float aGradientIndex;\n

uniform mat4 uMvpMatrix;\n
uniform float uStrokeScale;\n
uniform vec4 uAtlasInfo;\n
uniform vec2 uCenter;\n

varying vec4 v_glyph;\n
varying vec4 v_fill_color;\n
varying vec4 v_stroke_color;\n
varying float v_stroke_width;\n
varying vec3 v_texture_coord;\n
varying float v_gradient_type;\n
\n
vec4 glyph_vertex_transcode (vec2 v)\n
{\n
  ivec2 g = ivec2 (v);\n
  ivec2 corner = ivec2 (mod (v, 2.));\n
  g /= 2;\n
  ivec2 nominal_size = ivec2 (mod (vec2(g), 64.));\n
  return vec4 (corner * nominal_size, g * 4);\n
}\n
\n
void main()\n
{\n
  gl_Position = uMvpMatrix * vec4 (aPosition-uCenter, 0.0, 1.0);\n
  v_glyph = glyph_vertex_transcode (aGlyphyInfo);\n
\n
  v_fill_color = aFillColor;\n
  v_stroke_color = aStrokeColot;\n
  v_stroke_width = aStrokeWidth*uStrokeScale;\n
\n
  v_gradient_type = 0.;\n
  if( aGradientIndex > 0.5 ){ // gradient color
    vec3 pos = vec3(aPosition, 1.0);\n
    v_texture_coord.y = dot(aGradientParamY, pos);\n
    v_texture_coord.x = dot(aGradientParamX, pos);\n
    v_gradient_type = 1.;\n
    v_texture_coord.xy *= uAtlasInfo.z/uAtlasInfo.x;\n
    v_texture_coord.xy -= vec2(1.,1.)/uAtlasInfo.x;\n
    v_texture_coord.z = ( aGradientIndex ) / uAtlasInfo.y;\n
  }\n
}\n
);
