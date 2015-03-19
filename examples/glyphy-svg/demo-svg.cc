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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "demo-svg.h"
#include "glyphy-svg.h"

using namespace __gnu_cxx; /* This is ridiculous */

struct demo_svg_t {
  unsigned int   refcount;

  GlyphyAtlas  *atlas;
  glyphy_arc_accumulator_t *acc;

  /* stats */
  double       sum_error;
  unsigned int sum_endpoints;
  double       sum_fetch;
  unsigned int sum_bytes;
};

demo_svg_t *
demo_svg_create (GlyphyAtlas *atlas)
{
  demo_svg_t *svg = (demo_svg_t *) calloc (1, sizeof (demo_svg_t));
  svg->refcount = 1;

  svg->atlas = atlas;
  svg->acc = glyphy_arc_accumulator_create ();

  svg->sum_error  = 0;
  svg->sum_endpoints  = 0;
  svg->sum_fetch  = 0;
  svg->sum_bytes  = 0;

  return svg;
}

demo_svg_t *
demo_svg_reference (demo_svg_t *svg)
{
  if (svg) svg->refcount++;
  return svg;
}

void
demo_svg_destroy (demo_svg_t *svg)
{
  if (!svg || --svg->refcount)
    return;

  glyphy_arc_accumulator_destroy (svg->acc);
  svg->atlas = NULL;
  free (svg);
}

GlyphyAtlas *
demo_svg_get_atlas (demo_svg_t *svg)
{
  return svg->atlas;
}

static glyphy_bool_t
accumulate_endpoint (glyphy_arc_endpoint_t         *endpoint,
             std::vector<glyphy_arc_endpoint_t> *endpoints)
{
  endpoints->push_back (*endpoint);
  return true;
}
static void
encode_svg_shape (demo_svg_t      *svg,
         NSVGshape* shape,
         double            tolerance,
         glyphy_rgba_t    *buffer,
         unsigned int      buffer_len,
         unsigned int     *output_len,
         unsigned int     *nominal_width,
         unsigned int     *nominal_height,
         glyphy_extents_t *extents,
         double           *advance)
{
  std::vector<glyphy_arc_endpoint_t> endpoints;

  glyphy_arc_accumulator_reset (svg->acc);
  glyphy_arc_accumulator_set_tolerance (svg->acc, tolerance);
  glyphy_arc_accumulator_set_callback (svg->acc,
      (glyphy_arc_endpoint_accumulator_callback_t) accumulate_endpoint,
      &endpoints);

  if( !glyphy_svg(outline_decompose)(shape, svg->acc) )
    die ("Failed converting svg paths to arcs");

  //assert (glyphy_arc_accumulator_get_error (svg->acc) <= tolerance);

  if (endpoints.size () && ( ((shape->fill).color)>>24 )) //check the fill opacity to know whether needs to fill the internal shape.
  {
    //glyphy_outline_reverse (&endpoints[0], endpoints.size ());
    glyphy_outline_winding_from_even_odd (&endpoints[0], endpoints.size (), false);
  }

  double avg_fetch_achieved;
  double faraway = shape->strokeWidth;
  if (!glyphy_arc_list_encode_blob (endpoints.size () ? &endpoints[0] : NULL, endpoints.size (),
                    buffer,
                    buffer_len,
                    faraway,
                    4, /* UNUSED */
                    &avg_fetch_achieved,
                    output_len,
                    nominal_width,
                    nominal_height,
                    extents))
    die ("Failed encoding arcs");

  svg->sum_error += glyphy_arc_accumulator_get_error (svg->acc) / tolerance;
  svg->sum_endpoints += glyphy_arc_accumulator_get_num_endpoints (svg->acc);
  svg->sum_fetch += avg_fetch_achieved;
  svg->sum_bytes += (*output_len * sizeof (glyphy_rgba_t));
}

void
demo_svg_shape_upload( demo_svg_t *svg, NSVGshape* shape, glyph_info_t *glyph_info)
{
  glyphy_rgba_t buffer[1024 * 16];
  unsigned int output_len;

  encode_svg_shape(svg,
           shape,
           TOLERANCE,
           buffer, ARRAY_LEN (buffer),
           &output_len,
           &glyph_info->nominal_w,
           &glyph_info->nominal_h,
           &glyph_info->extents,
           &glyph_info->advance);

  glyph_info->is_empty = glyphy_extents_is_empty (&glyph_info->extents);
  if (!glyph_info->is_empty)
      svg->atlas->AtlasAlloc (buffer, output_len,
              &glyph_info->atlas_x, &glyph_info->atlas_y);
}
