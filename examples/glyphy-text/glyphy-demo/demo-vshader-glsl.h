static const char *demo_vshader_glsl =
"varying vec4 v_glyph;\n"
"varying float positionY;\n "// only for comparison in demo, draw up-half screen
"\n"
"vec4\n"
"glyph_vertex_transcode (vec2 v)\n"
"{\n"
"  ivec2 g = ivec2 (v);\n"
"  ivec2 corner = ivec2 (mod (v, 2.));\n"
"  g /= 2;\n"
"  ivec2 nominal_size = ivec2 (mod (vec2(g), 64.));\n"
"  return vec4 (corner * nominal_size, g * 4);\n"
"}\n"
"\n"
"void\n"
"main()\n"
"{\n"
"  gl_Position = uMvpMatrix * vec4 (aPosition, 1.0);\n"
"  v_glyph = glyph_vertex_transcode (aTexCoord);\n"

"  positionY = gl_Position.y;\n" // only for comparison in demo, draw up-half screen
"}\n"
;