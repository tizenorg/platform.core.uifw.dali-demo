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

#ifndef SVG_BUFFER_H
#define SVG_BUFFER_H

#include "glyphy.h"
#include "svg-encoder.h"
#undef NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

struct glyph_vertex_t {
  /* Position */
  float x;
  float y;
  /* Glyph info */
  float g16hi;
  float g16lo;
};

struct svg_buffer_t {
  unsigned int   refcount;

  glyphy_point_t cursor;
  std::vector<glyph_vertex_t> *vertices;
  glyphy_extents_t ink_extents;
  glyphy_extents_t logical_extents;
  bool dirty;
};

svg_buffer_t *
svg_buffer_create (void);

svg_buffer_t *
svg_buffer_reference (svg_buffer_t *buffer);

void
svg_buffer_destroy (svg_buffer_t *buffer);


void
svg_buffer_clear (svg_buffer_t *buffer);

void
svg_buffer_extents (svg_buffer_t    *buffer,
		     glyphy_extents_t *ink_extents,
		     glyphy_extents_t *logical_extents);

void
svg_buffer_move_to (svg_buffer_t        *buffer,
		     const glyphy_point_t *p);

void
svg_buffer_current_point (svg_buffer_t  *buffer,
			   glyphy_point_t *p);

void
svg_buffer_add_svg_shape(svg_buffer_t        *buffer,
              NSVGshape  *svgShape,
              svg_encoder_t *svg);

void
svg_add_glyph_vertices (const glyphy_point_t        &p,
				double                       font_size,
				glyph_info_t                *gi,
				std::vector<glyph_vertex_t> *vertices,
				glyphy_extents_t            *extents);

#endif /* SVG_BUFFER_H */
