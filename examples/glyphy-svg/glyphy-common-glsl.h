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
/*glyphy_arc_endpoint_t glyphy_arc_endpoint_decode_2 (const vec4 v, const vec4 v2, const ivec2 nominal_size)\n
{\n
  vec2 p = (vec2 (glyphy_float_to_byte (v.g), glyphy_float_to_byte(v.a)) + v.rb) / 256.;\n
  float d = float(glyphy_float_to_byte (v2.g)) + v2.r;\n
  if (d == 0.)\n
    d = GLYPHY_INFINITY;\n
  else\n
#define GLYPHY_MAX_D .75\n
    d = float(glyphy_float_to_byte (d) - 32768) * GLYPHY_MAX_D / 32767.;\n
#undef GLYPHY_MAX_D
  return glyphy_arc_endpoint_t (p * vec2(nominal_size), d);\n
}\n
\n*/
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
float glyphy_arc_wedge_signed_dist_shallow (const glyphy_arc_t a, const vec2 p)\n
{\n
  vec2 v = normalize (a.p1 - a.p0);\n
  float line_d = dot (p - a.p0, glyphy_ortho (v));\n
  if (a.d == 0.)\n
    return line_d;\n
\n
  float d0 = dot ((p - a.p0), v);\n
  if (d0 < 0.)\n
    return sign (line_d) * distance (p, a.p0);\n
  float d1 = dot ((a.p1 - p), v);\n
  if (d1 < 0.)\n
    return sign (line_d) * distance (p, a.p1);\n
  float r = 2. * a.d * (d0 * d1) / (d0 + d1);\n
  if (r * line_d > 0.)\n
    return sign (line_d) * min (abs (line_d + r), min (distance (p, a.p0), distance (p, a.p1)));\n
  return line_d + r;\n
}\n
\n
float glyphy_arc_wedge_signed_dist (const glyphy_arc_t a, const vec2 p)\n
{\n
  if (abs (a.d) <= .03)\n
    return glyphy_arc_wedge_signed_dist_shallow (a, p);\n
  vec2 c = glyphy_arc_center (a);\n
  return sign (a.d) * (distance (a.p0, c) - distance (p, c));\n
}\n
\n
/*float glyphy_arc_extended_dist (const glyphy_arc_t a, const vec2 p)\n
{\n
  // Note: this doesn't handle points inside the wedge. \n
  vec2 m = mix (a.p0, a.p1, .5);\n
  float d2 = glyphy_tan2atan (a.d);\n
  if (dot (p - m, a.p1 - m) < 0.)\n
    return dot (p - a.p0, normalize ((a.p1 - a.p0) * mat2(+d2, -1, +1, +d2)));\n
  else\n
    return dot (p - a.p1, normalize ((a.p1 - a.p0) * mat2(-d2, -1, +1, -d2)));\n
}\n
\n*/
\n
glyphy_arc_list_t glyphy_arc_list_decode (const vec4 v, const ivec2 nominal_size)\n
{\n
  glyphy_arc_list_t l;\n
  ivec4 iv = glyphy_vec4_to_bytes (v);\n
//  l.side = 0; /* unsure */\n"
//"  if (iv.r < 127) { /* arc-list encoded */\n"
  l.offset = (iv.g * 256) + iv.b;\n
  l.num_endpoints = iv.a;\n
  l.pivot = iv.r / 2;
  l.side = iv.r - l.pivot * 2;\n
//"  } \n"
/*"else { // single line encoded \n"
"    l.num_endpoints = -1;\n"
"    l.line_distance = float(((iv.r - 128) * 256 + iv.g) - 0x4000) / float (0x1FFF)\n"
"                    * max (float (nominal_size.x), float (nominal_size.y));\n"
"    l.line_angle = float(-((iv.b * 256 + iv.a) - 0x8000)) / float (0x7FFF) * 3.14159265358979;\n"
"  }\n"*/
  return l;\n
}\n
\n
int winding_line( const vec2 line_p0, const vec2 line_p1, const vec2 p, const vec2 cell_center )\n
{\n
  float s1_x = (line_p1.x - line_p0.x);\n
  float s1_y = (line_p1.y - line_p0.y);\n
  float s2_x = cell_center.x - p.x;\n
  float s2_y = cell_center.y - p.y;\n

  float denominator = -s2_x * s1_y + s1_x * s2_y;
  if( glyphy_iszero(denominator) ) // the lines are parallel
    return 0;
\n
  float s = (-s1_y * (line_p0.x - p.x) + s1_x * (line_p0.y- p.y)) / denominator;\n
  float t = ( s2_x * (line_p0.y - p.y) - s2_y * (line_p0.x - p.x)) / denominator;\n
\n
  if (s > 0.0 && s < 1.0 && t > 0.0 && t < 1.0+GLYPHY_EPSILON)\n  // Collision detected
    return 1;\n
\n
  return 0;\n // No collision
}\n

/*int winding_line (const vec2 l0, const vec2 l1, const vec2 p0, const vec2 p1)
{
  int w = 0;
  //Line equation
  float AX = (l1.x - l0.x);
  float BX = l0.x;
  float AY = (l1.y - l0.y);
  float BY = l0.y;

  //Line from pivot to fragment point
  float ax = (p1.x - p0.x);
  float bx = p0.x;
  float ay = (p1.y - p0.y);
  float by = p0.y;

  //Check for vertical line
  if (glyphy_iszero(ax)) {
    float tmp;
    tmp = ax; ax = ay; ay = tmp;
    tmp = bx; bx = by; by = tmp;
    tmp = AX; AX = AY; AY = tmp;
    tmp = BX; BX = BY; BY = tmp;
  }

  //Intersection equation
  float r = ay / ax;
  float a = (AY - r * AX);
  float b = (BY - r * BX) - (by - r * bx);

  //Find root
  float t = -b/a;
  if (t >= 0.0 && t <= 1.0) {
    float k = t * AX/ax + BX/ax - bx/ax;
    if (k >= 0.0 && k <= 1.0) w++;
  }

  return w;
}*/
\n
/*int winding_arc( const glyphy_arc_t arc, const vec2 p, const vec2 cell_center)\n
{\n
  if( distance( arc.p0, arc.p1 ) < 0.1 )
  {
    return winding_line (arc.p0, arc.p1, p, cell_center);
  }

  vec2 arc_center = glyphy_arc_center(arc);\n
  float arc_radius = distance (arc.p1, arc_center);\n

  \n
  int count  = 0;\n
  vec2 d = cell_center - p;\n
  float a = d.x * d.x + d.y * d.y;\n
  float b = 2.0 * (d.x * (p.x - arc_center.x) + d.y * (p.y - arc_center.y));\n
  float c = ( arc_center.x * arc_center.x + arc_center.y * arc_center.y )\n
  + ( p.x * p.x + p.y * p.y )\n
  - ( 2.0 * (arc_center.x * p.x + arc_center.y * p.y) )\n
  - ( arc_radius * arc_radius );\n
  float bb4ac = b * b - 4. * a * c;\n
  if (bb4ac > GLYPHY_EPSILON) {\n
    float mu1 = (-b + sqrt(bb4ac)) / (2.0 * a);\n
    float mu2 = (-b - sqrt(bb4ac)) / (2.0 * a);\n
    vec2 intersect = p + d*mu1;
    if( mu1 > 0.0 && mu1 < 1.0 && glyphy_arc_wedge_contains( arc, intersect) )\n
        count += 1;\n
    intersect = p + d*mu2;
    if( mu2 > 0.0 && mu2 < 1.0 && glyphy_arc_wedge_contains( arc, intersect ) )\n
        count += 1;\n
  }\n
  return count;\n
}\n*/
int winding_arc( const glyphy_arc_t arc, const vec2 p, const vec2 cell_center)\n
{\n
  vec2 arc_center = glyphy_arc_center(arc);\n
  float arc_radius = distance (arc.p1, arc_center);\n
  /*if( distance( arc.p0, arc.p1 ) < 0.075 )
  {
    return winding_line (arc.p0, arc.p1, p, cell_center);
  }*/
  \n
  int count  = 0;\n
  vec2 d = cell_center - p;\n
  vec2 pc = p - arc_center;\n
  float a = d.x * d.x + d.y * d.y;\n
  float b = (d.x * pc.x + d.y * pc.y);\n
  float c =  pc.x * pc.x + pc.y * pc.y  - ( arc_radius * arc_radius );\n
  float bb4ac = b * b -  a * c;\n
  if (bb4ac > 0.) {\n
    float mu1 = (-b + sqrt(bb4ac)) / a;\n
    float mu2 = (-b - sqrt(bb4ac)) /a;\n
    vec2 intersect = p + d*mu1;
    if( mu1 > 0.0 && mu1 < 1.0 && glyphy_arc_wedge_contains( arc, intersect) )\n
        count += 1;\n
    intersect = p + d*mu2;
    if(  mu2 > 0.0 && mu2 < 1.0 && glyphy_arc_wedge_contains( arc, intersect ) )\n
        count += 1;\n
  }\n
  return count;\n
}\n
\n
/*int winding_arc( const glyphy_arc_t arc, const vec2 p, const vec2 cell_center)\n
{\n
  vec2 arc_center = glyphy_arc_center(arc);\n
  float arc_radius = distance (arc.p1, arc_center);\n

  vec4 b_box;
  b_box.xy = min(arc.p0, arc.p1);
  b_box.zw = max(arc.p0, arc.p1);

  vec2 point = arc_center + vec2( -arcRadius, 0.0 );
  if( glyphy_arc_wedge_contains( arc, point) )
  {
    b_box.xy = min(b_box.xy, point);
    b_box.zw = max(b_box.xy, point);
  }
  point = arc_center + vec2( arcRadius, 0.0 );
  if( glyphy_arc_wedge_contains( arc, point) )
  {
    b_box.xy = min(b_box.xy, point);
    b_box.zw = max(b_box.xy, point);
  }
  point = arc_center + vec2( 0.0, -arcRadius );
  if( glyphy_arc_wedge_contains( arc, point) )
  {
    b_box.xy = min(b_box.xy, point);
    b_box.zw = max(b_box.xy, point);
  }
  point = arc_center + vec2( 0.0, arcRadius );
  if( glyphy_arc_wedge_contains( arc, point) )
  {
    b_box.xy = min(b_box.xy, point);
    b_box.zw = max(b_box.xy, point);
  }


\n
  vec2 d = cell_center - p;\n
  float a = d.x * d.x + d.y * d.y;\n
  float b = 2.0 * (d.x * (p.x - arc_center.x) + d.y * (p.y - arc_center.y));\n
  float c = ( arc_center.x * arc_center.x + arc_center.y * arc_center.y )\n
          + ( p.x * p.x + p.y * p.y )\n
          - ( 2.0 * (arc_center.x * p.x + arc_center.y * p.y) )\n
          - ( arc_radius * arc_radius );\n
  float bb4ac = b * b - 4. * a * c;\n
  int count  = 0;\n
  if (bb4ac > GLYPHY_EPSILON) {\n
    float mu1 = (-b + sqrt(bb4ac)) / (2.0 * a);\n
    float mu2 = (-b - sqrt(bb4ac)) / (2.0 * a);\n
    vec2 intersect = p + d*mu1;
    if( mu1 > 0.0 && mu1 < 1.0 && glyphy_arc_wedge_contains( arc, intersect) )\n
      count += 1;\n
    intersect = p + d*mu2;
    if( mu2 > 0.0 && mu2 < 1.0 && glyphy_arc_wedge_contains( arc, intersect ) )\n
      count += 1;\n
  }\n
  return count;\n
}\n*/


/*float atan2(float y, float x)
{
  return abs(x) > abs(y) ? 1.5708 - atan(x,y) : atan(y,x);
}*/

/*float atan2(float y, float x)
{
  return glyphy_iszero(x) ? sign(y)*1.5708 : atan(y, x);
}*/

/*int winding_arc(const glyphy_arc_t arc, const vec2 p, const vec2 cell_center)\n
{
  vec2 arc_center = glyphy_arc_center(arc);\n
  float arc_radius = distance (arc.p1, arc_center);\n

  vec2 d = p - cell_center;\n
  float angle = atan2(d.y,d.x);

  vec2 possibleIntersection = arc_center+arc_radius*vec2(cos(angle), sin(angle));

  if( glyphy_arc_wedge_contains( arc, possibleIntersection ) )
  {
    float c1 = (possibleIntersection.x-cell_center.x)/d.x;
    float c2 = (possibleIntersection.y-cell_center.y)/d.y;

    if( glyphy_iszero(c1-c2) && c1>0. && c1<1. )
      return 1;
  }

  return 0;
}*/
);
