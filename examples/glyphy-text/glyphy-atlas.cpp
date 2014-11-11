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
  mAtlasTexture = Dali::BitmapImage::New( textureWidth, textureHeight, Dali::Pixel::RGBA8888 );
}

void GlyphyAtlas::AtlasAlloc( glyphy_rgba_t* buffer,
		                      unsigned int len,
		                      unsigned int *px,
		                      unsigned int *py)
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

  if (w * h == len)
  {
    TexSubImage( x, y, w, h, buffer);
  }
  else
  {
    TexSubImage( x, y, w, h-1, buffer);
    // Upload the last row separately
    TexSubImage( x, y + h - 1, len - (w * (h - 1)), 1 , buffer + w * (h - 1));
  }

  *px = x / mItemWidth;
  *py = y / mItemHeightQuantum;
}

void GlyphyAtlas::TexSubImage( unsigned int offsetX,
		                       unsigned int offsetY,
		                       unsigned int width,
		                       unsigned int height,
		                       glyphy_rgba_t* data)
{
  Dali::PixelBuffer* pixbuf = mAtlasTexture.GetBuffer();
  size_t pos;
  size_t dataIndex = 0;
  for( size_t y= offsetY; y< height + offsetY; y++ )
  {
	pos = y * mTextureWidth * 4;
    for( size_t x = offsetX; x < width + offsetX; x++ )
    {
      pixbuf[pos+x*4] =  0xFF & data[dataIndex].r;
      pixbuf[pos+x*4+1] = 0xFF & data[dataIndex].g;
      pixbuf[pos+x*4+2] = 0xFF & data[dataIndex].b;
      pixbuf[pos+x*4+3] = 0xFF & data[dataIndex].a;
      dataIndex++;
    }
  }
 mAtlasTexture.Update();
}
