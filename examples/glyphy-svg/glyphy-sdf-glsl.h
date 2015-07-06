static std::string glyphySdfShaderSource = MAKE_STRING(
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
vec4 glyphy_texture_func (int offset, sampler2D _tex, ivec4 _atlas_info, vec2 tex_pos)\n
{\n
  vec2 pos = tex_pos + vec2 (mod (float (offset), float (_atlas_info.z)), offset / _atlas_info.z)/ vec2(_atlas_info.xy);\n
  return texture2D (_tex, pos);\n
}\n
\n
glyphy_arc_list_t glyphy_arc_list (const ivec2 cell, const ivec2 nominal_size, sampler2D _tex, ivec4 _atlas_info, vec2 tex_pos)\n
{\n
  int cell_offset = cell.y * nominal_size.x + cell.x;\n
  vec4 arc_list_data = glyphy_texture_func (cell_offset, _tex, _atlas_info, tex_pos);\n
  return glyphy_arc_list_decode (arc_list_data, nominal_size);\n
}\n
\n
float glyphy_sdf (const vec2 p, const ivec2 nominal_size, sampler2D _tex, ivec4 _atlas_info, ivec2 _atlas_pos)\n
{\n
  ivec2 cell =  ivec2 (clamp (floor (p), vec2 (0.,0.), vec2(nominal_size - 1)));\n
  vec2 tex_pos = (vec2 (_atlas_pos.xy * _atlas_info.zw) + vec2 (.5, .5)) / vec2(_atlas_info.xy);
  glyphy_arc_list_t arc_list = glyphy_arc_list (cell, nominal_size , _tex, _atlas_info, tex_pos);\n
\n
  float side = float(arc_list.side);\n
  vec2 pivot = vec2(cell) + vec2(0.3+0.4*float(arc_list.pivot/2), 0.3+0.4*mod( float(arc_list.pivot),2.0 ));\n
  /* Short-circuits: far-away cell */\n
  if (arc_list.num_endpoints == 0) {\n
    return GLYPHY_INFINITY * (side > 0.5 ? -1.0 : 1.0);\n
  }
\n
  float min_dist = GLYPHY_INFINITY;\n
\n
  glyphy_arc_endpoint_t endpoint_prev;\n
  glyphy_arc_endpoint_t endpoint;\n
  endpoint_prev = glyphy_arc_endpoint_decode ( glyphy_texture_func (arc_list.offset, _tex, _atlas_info, tex_pos), nominal_size);\n
  for (int i = 1; i < arc_list.num_endpoints; i++)\n
  {\n
    endpoint = glyphy_arc_endpoint_decode ( glyphy_texture_func (arc_list.offset + i, _tex, _atlas_info, tex_pos), nominal_size);\n
    glyphy_arc_t a = glyphy_arc_t (endpoint_prev.p, endpoint.p, endpoint.d);\n
    endpoint_prev = endpoint;\n
    if (glyphy_isinf (a.d)) continue;\n
\n
    if(glyphy_iszero(a.d))\n //line segment
    {\n
      side += float(winding_line( a.p0, a.p1, p, pivot ));\n
      min_dist = min(min_dist, dist_point_line_segment(a.p0,a.p1,p));
    }else\n // arc
    {
      a.d = 2. * a.d / (1. - a.d * a.d); // tan (2 * atan (d))
      vec2 arc_center = mix (a.p0, a.p1, .5) + glyphy_ortho (a.p1 - a.p0) / (2. * a.d);\n
      side += float(winding_arc(a, arc_center, p, pivot));\n
      if (glyphy_arc_wedge_contains (a, p))\n
      {\n
        min_dist = min(abs(distance (a.p0, arc_center) - distance (p, arc_center)), min_dist);
      } else {\n
        min_dist = min(min_dist, min (distance (p, a.p0), distance (p, a.p1)));\n
      }\n
    }
  }\n
\n
  side = mod(side,2.0) > 0.5 ? -1.0 : 1.0;\n
  return min_dist * side;\n
}\n
);
