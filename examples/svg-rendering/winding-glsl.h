/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
static std::string WindingShaderSource = MAKE_STRING(
\n
float dist_point_line_segment( const vec2 line_p0, const vec2 line_p1, const vec2 p)\n
{\n
  vec2 seg = line_p1 - line_p0;\n
  float t = dot( p-line_p0, seg ) / dot( seg, seg );
  return distance( p, mix( line_p0, line_p1, clamp(t, 0.0, 1.0) ) );
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

