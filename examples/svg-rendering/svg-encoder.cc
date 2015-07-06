/*
 * Copyright 2012 Google, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Google Author(s): Behdad Esfahbod
 */

#include "svg-encoder.h"
#include "glyphy-svg.h"

#include <stdio.h>
#include <assert.h>

static inline void
die (const char *msg)
{
  fprintf (stderr, "%s\n", msg);
  exit (1);
}

typedef struct {
  glyphy_extents_t extents;
  glyphy_bool_t    is_empty; /* has no outline; eg. space; don't draw it */
  unsigned int     nominal_w;
  unsigned int     nominal_h;
  unsigned int     atlas_x;
  unsigned int     atlas_y;
} glyph_info_t;

svg_encoder_t * svg_encoder_create (SvgAtlas *atlas)
{
  svg_encoder_t *encoder = (svg_encoder_t *) calloc (1, sizeof (svg_encoder_t));
  encoder->refcount = 1;

  encoder->vertices = new std::vector<glyph_vertex_t>;

  encoder->atlas = atlas;
  encoder->acc = glyphy_arc_accumulator_create ();

  glyphy_point_t top_left = {0, 0};
  encoder->cursor = top_left;

  return encoder;
}

svg_encoder_t * svg_encoder_reference (svg_encoder_t *encoder)
{
  if (encoder) encoder->refcount++;
  return encoder;
}

void svg_encoder_destroy (svg_encoder_t *encoder)
{
  if (!encoder || --encoder->refcount)
    return;

  delete encoder->vertices;
  glyphy_arc_accumulator_destroy (encoder->acc);
  encoder->atlas = NULL;

  free (encoder);
}

static glyphy_bool_t accumulate_endpoint (glyphy_arc_endpoint_t  *endpoint,
                                          std::vector<glyphy_arc_endpoint_t> *endpoints)
{
  endpoints->push_back (*endpoint);
  return true;
}

static void encode_svg_shape (svg_encoder_t      *svg,
         NSVGshape* shape,
         double            tolerance,
         glyphy_rgba_t    *buffer,
         unsigned int      buffer_len,
         unsigned int     *output_len,
         unsigned int     *nominal_width,
         unsigned int     *nominal_height,
         glyphy_extents_t *extents)
{
  std::vector<glyphy_arc_endpoint_t> endpoints;

  glyphy_arc_accumulator_reset (svg->acc);
  glyphy_arc_accumulator_set_tolerance (svg->acc, tolerance);
  glyphy_arc_accumulator_set_callback (svg->acc,
      (glyphy_arc_endpoint_accumulator_callback_t) accumulate_endpoint,
      &endpoints);

  if( !glyphy_svg(outline_decompose)(shape, svg->acc) )
    die ("Failed converting svg paths to arcs");

  assert (glyphy_arc_accumulator_get_error (svg->acc) <= tolerance);

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
}

void svg_encoder_shape_upload( svg_encoder_t *svg, NSVGshape* shape, glyph_info_t *glyph_info)
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
           &glyph_info->extents);

  glyph_info->is_empty = glyphy_extents_is_empty (&glyph_info->extents);
  if (!glyph_info->is_empty)
      svg->atlas->AtlasAlloc (buffer, output_len,
              &glyph_info->atlas_x, &glyph_info->atlas_y);
}


static unsigned int glyph_encode (unsigned int atlas_x ,  /* 7 bits */
          unsigned int atlas_y,   /* 7 bits */
          unsigned int corner_x,  /* 1 bit */
          unsigned int corner_y,  /* 1 bit */
          unsigned int nominal_w, /* 6 bits */
          unsigned int nominal_h  /* 6 bits */)
{
  assert (0 == (atlas_x & ~0x7F));
  assert (0 == (atlas_y & ~0x7F));
  assert (0 == (corner_x & ~1));
  assert (0 == (corner_y & ~1));
  assert (0 == (nominal_w & ~0x3F));
  assert (0 == (nominal_h & ~0x3F));

  unsigned int x = (((atlas_x << 6) | nominal_w) << 1) | corner_x;
  unsigned int y = (((atlas_y << 6) | nominal_h) << 1) | corner_y;

  return (x << 16) | y;
}

static void glyph_vertex_encode (double x, double y,
             unsigned int corner_x, unsigned int corner_y,
             const glyph_info_t *gi,
             glyph_vertex_t *v)
{
  unsigned int encoded = glyph_encode (gi->atlas_x, gi->atlas_y,
                       corner_x, corner_y,
                       gi->nominal_w, gi->nominal_h);
  v->x = x;
  v->y = y;
  v->g16hi = encoded >> 16;
  v->g16lo = encoded & 0xFFFF;
}

void svg_add_glyph_vertices (glyph_info_t                *gi,
                        std::vector<glyph_vertex_t> *vertices,
                        glyphy_extents_t            *extents)
{
  if (gi->is_empty)
    return;

  glyph_vertex_t v[4];

#define ENCODE_CORNER(_cx, _cy) \
  do { \
    double _vx = ((1-_cx) * gi->extents.min_x + _cx * gi->extents.max_x); \
    double _vy = ((1-_cy) * gi->extents.min_y + _cy * gi->extents.max_y); \
    glyph_vertex_encode (_vx, _vy, _cx, _cy, gi, &v[_cx * 2 + _cy]); \
  } while (0)
  ENCODE_CORNER (0, 0);
  ENCODE_CORNER (0, 1);
  ENCODE_CORNER (1, 0);
  ENCODE_CORNER (1, 1);
#undef ENCODE_CORNER

  vertices->push_back (v[0]);
  vertices->push_back (v[1]);
  vertices->push_back (v[2]);
  vertices->push_back (v[3]);

  if (extents) {
    glyphy_extents_clear (extents);
    for (unsigned int i = 0; i < 4; i++) {
      glyphy_point_t p = {v[i].x, v[i].y};
      glyphy_extents_add (extents, &p);
    }
  }
}

void svg_encoder_add_svg_shape(svg_encoder_t        *encoder,
              NSVGshape  *svgShape)
{
  glyph_info_t gi;
  svg_encoder_shape_upload (encoder, svgShape, &gi);

  /* Update ink extents */
  glyphy_extents_t ink_extents;
  svg_add_glyph_vertices (&gi, encoder->vertices, &ink_extents);
  glyphy_extents_extend (&encoder->ink_extents, &ink_extents);
}
