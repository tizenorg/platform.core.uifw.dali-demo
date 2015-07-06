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
#ifndef DEMO_SVG_H_
#define DEMO_SVG_H_

#include "glyphy.h"
#include "svg-atlas.h"
#undef NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define TOLERANCE 0.65
#define ARRAY_LEN(Array) (sizeof (Array) / sizeof (*Array))

static inline void
die (const char *msg)
{
  fprintf (stderr, "%s\n", msg);
  exit (1);
}

typedef struct {
  glyphy_extents_t extents;
  double           advance;
  glyphy_bool_t    is_empty; /* has no outline; eg. space; don't draw it */
  unsigned int     nominal_w;
  unsigned int     nominal_h;
  unsigned int     atlas_x;
  unsigned int     atlas_y;
} glyph_info_t;


typedef struct demo_svg_t demo_svg_t;

demo_svg_t *
demo_svg_create (SvgAtlas *atlas);

demo_svg_t *
demo_svg_reference (demo_svg_t *svg);

void
demo_svg_destroy (demo_svg_t *svg);

SvgAtlas *
demo_svg_get_atlas (demo_svg_t *svg);

void
demo_svg_shape_upload(demo_svg_t *svg, NSVGshape* shape, glyph_info_t *glyph_info);

#endif /* DEMO_SVG_H_ */
