#ifndef __DALI_DEMO_GLYPHY_ATLAS_H__
#define __DALI_DEMO_GLYPHY_ATLAS_H__

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

#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/images/atlas.h>
#include <dali/public-api/images/bitmap-image.h>

#include "glyphy-src/glyphy.h"

class GlyphyAtlas : public Dali::RefObject
{
public:

  GlyphyAtlas(unsigned int textureWidth,
              unsigned int textureHeight,
              unsigned int itemWidth,
              unsigned int itemHeightQuantum);

  /*
   * demo-atlas.cc
   * void demo_atlas_alloc (demo_atlas_t  *at,
   *                        glyphy_rgba_t *data,
   *                        unsigned int   len,
   *                        unsigned int  *px,
   *                        unsigned int  *py)
   */
  void AtlasAlloc( glyphy_rgba_t* buffer,
		           unsigned int len,
		           unsigned int *px,
		           unsigned int *py);

  Dali::BitmapImage CreateBitmap( glyphy_rgba_t* buffer,
                                         unsigned int bufferLength,
                                         unsigned int requiredWidth );

  unsigned int GetTextureWidth() { return mTextureWidth; }
  unsigned int GetTextureHeight() { return mTextureHeight; }
  unsigned int GetItemWidth() { return mItemWidth; }
  unsigned int GetItemHeightQuantum() { return mItemHeightQuantum; }

  Dali::Image GetAtlasTexture();

private:

  //specify a two-dimensional texture sub-image
  void TexSubImage( unsigned int offsetX,
		            unsigned int offsetY,
		            unsigned int width,
		            unsigned int height,
		            glyphy_rgba_t* data);


private:

  unsigned int mTextureWidth;
  unsigned int mTextureHeight;
  unsigned int mItemWidth;
  unsigned int mItemHeightQuantum;

  unsigned int mCursorX;
  unsigned int mCursorY;

  Dali::BitmapImage mAtlasTexture;

  Dali::Atlas mNewAtlas;
};

#endif //  __DALI_DEMO_GLYPHY_ATLAS_H__
