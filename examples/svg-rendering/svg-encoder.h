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

#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/intrusive-ptr.h>

class SvgEncoder : public Dali::RefObject
{
public:

  struct glyph_vertex_t {
    /* Position */
    float x;
    float y;
    /* Glyph info */
    float g16hi;
    float g16lo;
  };
  typedef std::vector<glyph_vertex_t>   VertexContainer;

  SvgEncoder();

  ~SvgEncoder();

   void AddSvgShape( NSVGshape  *svgShape );

   const VertexContainer& GetVertices() { return mVertices; }

   const glyphy_extents_t& GetExtents() { return mExtents; }

   SvgAtlasPtr GetAtlas() { return mAtlas; }

private:

   struct glyph_info_t{
     glyphy_extents_t extents;
     unsigned int     nominal_w;
     unsigned int     nominal_h;
     unsigned int     atlas_x;
     unsigned int     atlas_y;
   };

   void encode_svg_shape ( NSVGshape* shape,
       double tolerance,
       glyphy_rgba_t *buffer,
       unsigned int buffer_len,
       unsigned int *output_len,
       unsigned int *nominal_width,
       unsigned int *nominal_height,
       glyphy_extents_t *extents);

   void glyph_vertex_encode ( const glyph_info_t& gi);

private:
  SvgAtlasPtr                 mAtlas;
  VertexContainer             mVertices;
  glyphy_arc_accumulator_t   *mAccumulator;
  glyphy_extents_t            mExtents;

};

#endif /* SVG_ENCODER_H_ */
