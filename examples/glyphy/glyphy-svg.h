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

#include "glyphy-src/glyphy.h"

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



static glyphy_bool_t
glyphy_svg(move_to) (float to_x, float to_y,
              glyphy_arc_accumulator_t *acc)
{
  glyphy_point_t p1 = {static_cast<double>(to_x), static_cast<double>(to_y)};
  glyphy_arc_accumulator_close_path (acc);
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

/*static glyphy_bool_t
glyphy_svg(conic_to) (float control_x, float control_y,
                      float to_x, float to_y,
               glyphy_arc_accumulator_t *acc)
{
  glyphy_point_t p1 = {static_cast<double>(control_x), static_cast<double>(control_y)};
  glyphy_point_t p2 = {static_cast<double>(to_x), static_cast<double>(to_y)};
  glyphy_arc_accumulator_conic_to (acc, &p1, &p2);
  return glyphy_arc_accumulator_successful (acc);
}*/

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
glyphy_svg(outline_decompose) (const NSVGimage  *svgImage,
                    glyphy_arc_accumulator_t *acc)
{
  NSVGshape* shape;
  NSVGpath* path;
  int i = 0;
  glyphy_bool_t success = true;
  for (shape = svgImage->shapes; shape != NULL; shape = shape->next)
  {
    for (path = shape->paths; path != NULL; path = path->next)
    {
      float* p = &path->pts[0];
      //std::cout<<"move to: "<<p[0]<<", "<<p[1]<<std::endl;
      success = success && glyphy_svg(move_to)( p[0], p[1], acc );

      for (i = 0; i < path->npts-1; i += 3) {
        float* p = &path->pts[i*2];
        if( p[2] < 0.f&& p[3] < 0.f && p[4] < 0.f && p[5] < 0.f )
        {
          //std::cout<<"line to: "<<p[6]<<", "<<p[7]<<std::endl;
          success = success && glyphy_svg(line_to)(p[6], p[7], acc);
        }
        else
        {
          //std::cout<<"curve to: "<<p[2]<<", "<<p[3]<<", "<<p[4]<<", "<<p[5]<<", "<<p[6]<<", "<<p[7]<<std::endl;
          success = success && glyphy_svg(cubic_to)( p[2], p[3], p[4], p[5], p[6], p[7], acc );
        }
      }
    }
  }

  return success;
}

#ifdef __cplusplus
}
#endif
