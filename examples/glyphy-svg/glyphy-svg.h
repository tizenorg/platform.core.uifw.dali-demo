/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
 */

/* Intentionally doesn't have include guards */

#include "glyphy.h"
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

#undef NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#ifndef GLYPHY_SVG_PREFIX
#define GLYPHY_SVG_PREFIX glyphy_svg_
#endif

#ifndef glyphy_svgNSVGimage
#define glyphy_svg(name) GLYPHY_PASTE (GLYPHY_SVG_PREFIX, name)
#endif

namespace
{
const float epsilon = 0.1f;

bool float_equal( float f1, float f2 )
{
  return fabsf(f1-f2) < epsilon;
}

}

static glyphy_bool_t
glyphy_svg(move_to) (float to_x, float to_y,
              glyphy_arc_accumulator_t *acc)
{
  glyphy_point_t p1 = {static_cast<double>(to_x), static_cast<double>(to_y)};
 // glyphy_arc_accumulator_close_path (acc);
  glyphy_arc_accumulator_move_to (acc, &p1);
  return glyphy_arc_accumulator_successful (acc);
}

static glyphy_bool_t
glyphy_svg(line_to) (float to_x, float to_y,
                     glyphy_arc_accumulator_t *acc)
{
  glyphy_point_t p1 = {static_cast<double>(to_x), static_cast<double>(to_y)};
  glyphy_arc_accumulator_line_to (acc, &p1);
  return glyphy_arc_accumulator_successful (acc);
}

static glyphy_bool_t
glyphy_svg(cubic_to) (float control1_x, float control1_y,
                      float control2_x, float control2_y,
                      float to_x, float to_y,
                      glyphy_arc_accumulator_t *acc)
{
  glyphy_point_t p1 = {static_cast<double>(control1_x), static_cast<double>(control1_y)};
  glyphy_point_t p2 = {static_cast<double>(control2_x), static_cast<double>(control2_y)};
  glyphy_point_t p3 = {static_cast<double>(to_x), static_cast<double>(to_y)};
  glyphy_arc_accumulator_cubic_to (acc, &p1, &p2, &p3);
  return glyphy_arc_accumulator_successful (acc);
}

static glyphy_bool_t
glyphy_svg(segment_to) ( float from_x, float from_y,
                         float control1_x, float control1_y,
                         float control2_x, float control2_y,
                         float to_x, float to_y,
                         glyphy_arc_accumulator_t *acc)
{
  if( float_equal( control1_x, to_x )
   && float_equal( control1_y, to_y )
   && float_equal( control2_x, from_x )
   && float_equal( control2_y, from_y ))
  {
    return glyphy_svg(line_to)(to_x, to_y, acc);
  }
  else
  {
    return glyphy_svg(cubic_to)( control1_x, control1_y, control2_x, control2_y, to_x, to_y, acc );
  }
}

static glyphy_bool_t
glyphy_svg(outline_decompose) (const NSVGshape  *svgShape,
                    glyphy_arc_accumulator_t *acc)
{
  NSVGpath* path;
  int i = 0;
  glyphy_bool_t success = true;
  for (path = svgShape->paths; path != NULL; path = path->next)
  {
    float* p = &path->pts[0];
    success = success && glyphy_svg(move_to)( p[0], p[1], acc );

    for (i = 0; i < path->npts-1; i += 3)
    {
      float* p = &path->pts[i*2];
      {
        success = success && glyphy_svg(segment_to)( p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], acc );
      }
    }
  }
  if( svgShape->fill.type > 0 )
  {
    glyphy_arc_accumulator_close_path (acc);
  }

  return success;
}

#ifdef __cplusplus
}
#endif
