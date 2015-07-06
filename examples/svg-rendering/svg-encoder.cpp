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

namespace
{

#define TOLERANCE 0.65
#define ARRAY_LEN(Array) (sizeof (Array) / sizeof (*Array))

const unsigned int TEXTURE_ITEM_WIDTH( 64u );

void die (const char *msg)
{
  fprintf (stderr, "%s\n", msg);
  exit (1);
}

glyphy_bool_t accumulate_endpoint (glyphy_arc_endpoint_t  *endpoint,
                                          std::vector<glyphy_arc_endpoint_t> *endpoints)
{
  endpoints->push_back (*endpoint);
  return true;
}

unsigned int glyph_encode (unsigned int atlas_x ,  /* 7 bits */
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

}

SvgEncoder::SvgEncoder()
{
  mAtlas = new SvgAtlas( 1024,512, TEXTURE_ITEM_WIDTH, 8 );
  mAccumulator = glyphy_arc_accumulator_create ();
  glyphy_extents_clear (&mExtents);
}

SvgEncoder::~SvgEncoder()
{
  mVertices.clear();
  glyphy_arc_accumulator_destroy (mAccumulator);
}

void SvgEncoder::AddSvgShape( NSVGshape  *svgShape )
{
  glyphy_rgba_t buffer[1024 * 16];
  unsigned int output_len;
  glyph_info_t glyph_info;

  encode_svg_shape(svgShape,
      TOLERANCE,
      buffer, ARRAY_LEN (buffer),
      &output_len,
      &glyph_info.nominal_w,
      &glyph_info.nominal_h,
      &glyph_info.extents);

  if (!glyphy_extents_is_empty (&glyph_info.extents))
  {
    mAtlas->AtlasAlloc (buffer, output_len, &glyph_info.atlas_x, &glyph_info.atlas_y);

    glyph_vertex_encode(glyph_info);
  }
}

void SvgEncoder::encode_svg_shape ( NSVGshape* shape,
         double            tolerance,
         glyphy_rgba_t    *buffer,
         unsigned int      buffer_len,
         unsigned int     *output_len,
         unsigned int     *nominal_width,
         unsigned int     *nominal_height,
         glyphy_extents_t *extents)
{
  std::vector<glyphy_arc_endpoint_t> endpoints;

  glyphy_arc_accumulator_reset (mAccumulator);
  glyphy_arc_accumulator_set_tolerance (mAccumulator, tolerance);
  glyphy_arc_accumulator_set_callback (mAccumulator,
      (glyphy_arc_endpoint_accumulator_callback_t) accumulate_endpoint,
      &endpoints);

  if( !glyphy_svg(outline_decompose)(shape, mAccumulator) )
    die ("Failed converting svg paths to arcs");

  assert (glyphy_arc_accumulator_get_error (mAccumulator) <= tolerance);

  double faraway = shape->strokeWidth;
  if (!glyphy_arc_list_encode_blob (endpoints.size () ? &endpoints[0] : NULL, endpoints.size (),
                    buffer,
                    buffer_len,
                    faraway,
                    output_len,
                    nominal_width,
                    nominal_height,
                    extents))
    die ("Failed encoding arcs");
}


void SvgEncoder::glyph_vertex_encode ( const glyph_info_t& glyph_info )
{
  // ENCODE_CORNER (0, 0);   ENCODE_CORNER (0, 1);   ENCODE_CORNER (1, 0); ENCODE_CORNER (1, 1);
  glyphy_extents_t ink_extents;
  glyphy_extents_clear (&ink_extents);

  for( unsigned int corner_y = 0u;  corner_y<2; corner_y++ )
    for( unsigned int corner_x = 0u;  corner_x<2; corner_x++ )
    {
      glyph_vertex_t vertex;
      vertex.x = ((1-corner_x) * glyph_info.extents.min_x + corner_x * glyph_info.extents.max_x);
      vertex.y = ((1-corner_y) * glyph_info.extents.min_y + corner_y * glyph_info.extents.max_y);

      unsigned int encoded = glyph_encode (glyph_info.atlas_x, glyph_info.atlas_y,
          corner_x, corner_y,
          glyph_info.nominal_w, glyph_info.nominal_h);

      vertex.g16hi = encoded >> 16;
      vertex.g16lo = encoded & 0xFFFF;
      mVertices.push_back (vertex);

      glyphy_point_t p = {vertex.x, vertex.y};
      glyphy_extents_add (&ink_extents, &p);
    }

  glyphy_extents_extend (&mExtents, &ink_extents);
}
