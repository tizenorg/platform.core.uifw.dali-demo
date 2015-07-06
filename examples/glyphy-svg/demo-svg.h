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

#include "glyphy-info.h"
#include "glyphy-atlas.h"
#undef NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

typedef struct demo_svg_t demo_svg_t;

demo_svg_t *
demo_svg_create (GlyphyAtlas *atlas);

demo_svg_t *
demo_svg_reference (demo_svg_t *svg);

void
demo_svg_destroy (demo_svg_t *svg);

GlyphyAtlas *
demo_svg_get_atlas (demo_svg_t *svg);

void
demo_svg_shape_upload(demo_svg_t *svg, NSVGshape* shape, glyph_info_t *glyph_info);

#endif /* DEMO_SVG_H_ */
