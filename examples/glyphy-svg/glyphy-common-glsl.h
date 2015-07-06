static std::string glyphyCommonShaderSource = MAKE_STRING(
/*\n
 * Copyright 2012 Google, Inc. All Rights Reserved.\n
 *\n
 * Licensed under the Apache License, Version 2.0 (the \"License\");\n
 * you may not use this file except in compliance with the License.\n
 * You may obtain a copy of the License at\n
 *\n
 *     http://www.apache.org/licenses/LICENSE-2.0\n
 *\n
 * Unless required by applicable law or agreed to in writing, software\n
 * distributed under the License is distributed on an \"AS IS\" BASIS,\n
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n
 * See the License for the specific language governing permissions and\n
 * limitations under the License.\n
 *\n
 * Google Author(s): Behdad Esfahbod, Maysum Panju\n
 */
\n
struct glyphy_arc_t {\n
  vec2  p0;\n
  vec2  p1;\n
  float d;\n
};\n
\n
struct glyphy_arc_endpoint_t {\n
  /* Second arc endpoint */\n
  vec2  p;\n
  /* Infinity if this endpoint does not form an arc with the previous\n
   * endpoint.  Ie. a \"move_to\".  Test with glyphy_isinf().\n
   * Arc depth otherwise.  */\n
  float d;\n
};\n
\n
struct glyphy_arc_list_t {\n
  /* Number of endpoints in the list.\n
   * Will be zero if we're far away inside or outside, in which case side is set.\n
   * Will be -1 if this arc-list encodes a single line, in which case line_* are set. */\n
  int num_endpoints;\n
\n
  /* If num_endpoints is zero, this specifies whether we are inside (-1)\n
   * or outside (+1).  Otherwise we're unsure (0). */\n
  int side;\n
  /* Offset to the arc-endpoints from the beginning of the glyph blob */\n
  int offset;\n
  int pivot;\n
\n
};\n
\n
bool glyphy_isinf (const float v)\n
{\n
  return abs (v) >= GLYPHY_INFINITY * .5;\n
}\n
\n
bool glyphy_iszero (const float v)\n
{\n
  return abs (v) <= GLYPHY_EPSILON * 2.;\n
}\n
\n
vec2 glyphy_ortho (const vec2 v)\n
{\n
  return vec2 (-v.y, v.x);\n
}\n
\n
int glyphy_float_to_byte (const float v)\n
{\n
  return int (v * (256. - GLYPHY_EPSILON));\n
}\n
\n
ivec4 glyphy_vec4_to_bytes (const vec4 v)\n
{\n
  return ivec4 (v * (256. - GLYPHY_EPSILON));\n
}\n
\n
ivec2 glyphy_float_to_two_nimbles (const float v)\n
{\n
  int f = glyphy_float_to_byte (v);\n
  return ivec2 (f / 16, int(mod (float(f), 16.)));\n
}\n
\n
/* returns tan (2 * atan (d)) */\n
float glyphy_tan2atan (const float d)\n
{\n
  return 2. * d / (1. - d * d);\n
}\n
\n
glyphy_arc_endpoint_t glyphy_arc_endpoint_decode (const vec4 v, const ivec2 nominal_size)\n
{\n
  vec2 p = (vec2 (glyphy_float_to_two_nimbles (v.a)) + v.gb) / 16.;\n
  float d = v.r;\n
  if (d < GLYPHY_EPSILON)\n
    d = GLYPHY_INFINITY;\n
  else\n
    d = float(glyphy_float_to_byte (d) - 128) * GLYPHY_MAX_D / 127.;\n
  return glyphy_arc_endpoint_t (p * vec2(nominal_size), d);\n
}\n
\n
vec2 glyphy_arc_center (const glyphy_arc_t a)\n
{\n
  return mix (a.p0, a.p1, .5) + glyphy_ortho (a.p1 - a.p0) / (2. * glyphy_tan2atan (a.d));\n
}\n
\n
bool glyphy_arc_wedge_contains (const glyphy_arc_t a, const vec2 p)\n
{\n
  float d2 = glyphy_tan2atan (a.d);\n
  return dot (p - a.p0, (a.p1 - a.p0) * mat2(1,  d2, -d2, 1)) >= 0. &&\n
	 dot (p - a.p1, (a.p1 - a.p0) * mat2(1, -d2,  d2, 1)) <= 0.;\n
}\n
\n
float dist_point_line_segment( const vec2 line_p0, const vec2 line_p1, const vec2 p)\n
{\n
  vec2 seg = line_p1 - line_p0;\n
  float t = dot( p-line_p0, seg ) / dot( seg, seg );
  return distance( p, mix( line_p0, line_p1, clamp(t, 0.0, 1.0) ) );
}\n
\n
glyphy_arc_list_t glyphy_arc_list_decode (const vec4 v, const ivec2 nominal_size)\n
{\n
  glyphy_arc_list_t l;\n
  ivec4 iv = glyphy_vec4_to_bytes (v);\n
  l.offset = (iv.g * 256) + iv.b;\n
  l.num_endpoints = iv.a;\n
  l.pivot = iv.r / 2;
  l.side = iv.r - l.pivot * 2;\n
  return l;\n
}\n
\n
int winding_line( const vec2 line_p0, const vec2 line_p1, const vec2 p, const vec2 pivot )\n
{\n
  vec2 s1 = line_p1 - line_p0;\n
  vec2 s2 = pivot - p;\n
  vec2 s3 = vec2( line_p0.y - p.y, p.x - line_p0.x);\n

  float denominator = -s2.x * s1.y + s1.x * s2.y;
  if( glyphy_iszero(denominator) ) // the lines are parallel
    return 0;
\n
  float s = dot( s1, s3 ) / denominator;\n
  float t = dot( s2, s3 ) / denominator;\n
\n
  if (s == clamp(s, 0., 1.) && t == clamp(t, 0., 1.))\n  // Collision detected
    return 1;\n
\n
  return 0;\n // No collision
}\n
\n
int winding_arc( const glyphy_arc_t arc, const vec2 arc_center, const vec2 p, const vec2 pivot)\n
{\n
  float arc_radius = distance (arc.p1, arc_center);\n
  \n
  int count  = 0;\n
  vec2 d = pivot - p;\n
  vec2 pc = p - arc_center;\n
  float a = dot(d,d);\n
  float b = dot(d,pc);\n
  float c = dot(pc, pc) - ( arc_radius * arc_radius );\n
  float bb4ac = b * b -  a * c;\n
  if (bb4ac > 0.) {\n
    float mu1 = (-b + sqrt(bb4ac)) / a;\n
    float mu2 = (-b - sqrt(bb4ac)) / a;\n
    if( mu1 == clamp(mu1, 0., 1.) && glyphy_arc_wedge_contains( arc, mix(p, pivot, mu1) ) )\n
        count += 1;\n
    if( mu2 == clamp(mu2, 0., 1.) && glyphy_arc_wedge_contains( arc, mix(p, pivot, mu2) ) )\n
        count += 1;\n
  }\n
  return count;\n
}\n

);
