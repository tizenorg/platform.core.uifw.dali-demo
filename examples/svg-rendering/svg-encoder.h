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

#ifndef SVG_ENCODER_H
#define SVG_ENCODER_H

#include "glyphy.h"
#include "svg-atlas.h"
#undef NANOSVG_IMPLEMENTATION
#undef NANOSVG_ALL_COLOR_KEYWORDS
#include "nanosvg.h"

#define TOLERANCE 0.65
#define ARRAY_LEN(Array) (sizeof (Array) / sizeof (*Array))

struct glyph_vertex_t {
  /* Position */
  float x;
  float y;
  /* Glyph info */
  float g16hi;
  float g16lo;
};

struct svg_encoder_t {
  unsigned int   refcount;

  glyphy_point_t cursor;
  std::vector<glyph_vertex_t> *vertices;
  glyphy_extents_t ink_extents;

  SvgAtlas  *atlas;
  glyphy_arc_accumulator_t *acc;
};

svg_encoder_t *
svg_encoder_create (SvgAtlas *atlas);

svg_encoder_t *
svg_encoder_reference (svg_encoder_t *encoder);

void
svg_encoder_destroy (svg_encoder_t *encoder);

void
svg_encoder_add_svg_shape(svg_encoder_t        *encoder,
              NSVGshape  *svgShape);

#endif /* SVG_ENCODER_H_ */
