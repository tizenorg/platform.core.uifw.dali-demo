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
 *
 */

// CLASS HEADER
#include "glyphy-atlas.h"

#include <string.h>

using namespace Dali;

GlyphyAtlas::GlyphyAtlas(unsigned int textureWidth,
                         unsigned int textureHeight,
                         unsigned int itemWidth,
                         unsigned int itemHeightQuantum)
: mTextureWidth( textureWidth ),
  mTextureHeight( textureHeight ),
  mItemWidth( itemWidth ),
  mItemHeightQuantum( itemHeightQuantum ),
  mCursorX( 0 ),
  mCursorY( 0 )
{
  mNewAtlas = Atlas::New( textureWidth, textureHeight, Pixel::RGBA8888 );
}

void GlyphyAtlas::AtlasAlloc( BitmapImage& bitmapImage,
                               unsigned int len,
                               unsigned int *px,
                               unsigned int *py )
{
  unsigned int w, h, x, y;

  w = mItemWidth;
  h = (len + w - 1) / w;

  if (mCursorY + h > mTextureHeight)
  {
    /* Go to next column */
    mCursorX +=mItemWidth;
    mCursorY = 0;
  }

  if(mCursorX + w <= mTextureWidth && mCursorY + h <= mTextureHeight)
  {
    x = mCursorX;
    y = mCursorY;
    mCursorY += (h + mItemHeightQuantum - 1) & ~(mItemHeightQuantum - 1);
  }
  else
  {
    std::cout<<"Ran out of atlas memory"<<std::endl;
    exit(1);
  }

  *px = x / mItemWidth;
  *py = y / mItemHeightQuantum;

  mNewAtlas.Upload( bitmapImage, x, y );
}

Dali::Image GlyphyAtlas::GetAtlasTexture()
{
  return mNewAtlas;
}

