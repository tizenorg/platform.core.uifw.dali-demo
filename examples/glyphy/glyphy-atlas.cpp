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

#define USE_NEW_ATLAS

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
#ifndef USE_NEW_ATLAS
  mAtlasTexture = Dali::BitmapImage::New( textureWidth, textureHeight, Dali::Pixel::RGBA8888 );
#else
  mNewAtlas = Atlas::New( textureWidth, textureHeight, Pixel::RGBA8888 );
#endif
}

#ifndef USE_NEW_ATLAS

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
    TexSubImage( mAtlasTexture, x, y, w, h, buffer);
  }
  else
  {
    TexSubImage( mAtlasTexture, x, y, w, h-1, buffer);
    // Upload the last row separately
    TexSubImage( mAtlasTexture, x, y + h - 1, len - (w * (h - 1)), 1 , buffer + w * (h - 1));
  }

  mAtlasTexture.Update();

  *px = x / mItemWidth;
  *py = y / mItemHeightQuantum;

  std::cout << "GlyphyAtlas::AtlasAlloc len: " << len << " x,y: " << x << "," << y << std::endl;
}

#else // USE_NEW_ATLAS

void GlyphyAtlas::AtlasAlloc( glyphy_rgba_t* buffer,
                               unsigned int len,
                               unsigned int *px,
                               unsigned int *py )
{
  BitmapImage bitmapImage = CreateBitmap( buffer, len, mItemWidth );
  //BitmapImage bitmapImage = CreateBitmap( buffer, len, 1 );

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

  mNewAtlas.Upload( bitmapImage, *px, *py );

  std::cout << "GlyphyAtlas::AtlasAlloc len: " << len << " x,y: " << x << "," << y << std::endl;
}

#endif // USE_NEW_ATLAS

BitmapImage GlyphyAtlas::CreateBitmap( glyphy_rgba_t* buffer,
		                       unsigned int bufferLength,
                                       unsigned int requiredWidth )
{
  unsigned int h = (bufferLength + requiredWidth - 1) / requiredWidth;

  std::cout << "Creating bitmap image size: " << requiredWidth << "," << h << std::endl;

  BitmapImage bitmap = BitmapImage::New( requiredWidth, h, Dali::Pixel::RGBA8888 );

#if 1
  Dali::PixelBuffer* pixbuf = bitmap.GetBuffer();
  memcpy( pixbuf, buffer, bufferLength*4 );
#else
  unsigned int w = requiredWidth;

  if (w * h == bufferLength)
  {
    TexSubImage( bitmap, 0, 0, w, h, buffer);
  }
  else
  {
    TexSubImage( bitmap, 0, 0, w, h-1, buffer);
    // Upload the last row separately
    TexSubImage( bitmap, 0, 0 + h - 1, bufferLength - (w * (h - 1)), 1 , buffer + w * (h - 1));
  }
#endif

  return bitmap;
}

Dali::Image GlyphyAtlas::GetAtlasTexture()
{
#ifdef USE_NEW_ATLAS
  return mNewAtlas;
#else
  return mAtlasTexture;
#endif
}

void GlyphyAtlas::TexSubImage( BitmapImage destImage,
                                       unsigned int offsetX,
		                       unsigned int offsetY,
		                       unsigned int width,
		                       unsigned int height,
		                       glyphy_rgba_t* data)
{
  Dali::PixelBuffer* pixbuf = destImage.GetBuffer();
  size_t pos;
  size_t dataIndex = 0;

  unsigned int textureWidth = destImage.GetWidth();

  for( size_t y= offsetY; y< height + offsetY; y++ )
  {
    pos = y * textureWidth * 4;

    for( size_t x = offsetX; x < width + offsetX; x++ )
    {
      pixbuf[pos+x*4] =  0xFF & data[dataIndex].r;
      pixbuf[pos+x*4+1] = 0xFF & data[dataIndex].g;
      pixbuf[pos+x*4+2] = 0xFF & data[dataIndex].b;
      pixbuf[pos+x*4+3] = 0xFF & data[dataIndex].a;
      dataIndex++;
    }
  }
}
