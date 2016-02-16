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
#include "svg-atlas.h"
#include <iostream>

namespace
{
const unsigned int TEXTURE_ITEM_WIDTH( 64u );
const unsigned int TEXTURE_ITEM_HEIGHT( 128u );
const unsigned int TEXTURE_ITEM_HEIGHT_QUANTUM( 8u );
}

SvgAtlas::SvgAtlas(unsigned int textureWidth,
                   unsigned int textureHeight)
: mTextureWidth( textureWidth ),
  mTextureHeight( textureHeight ),
  mItemWidth( TEXTURE_ITEM_WIDTH ),
  mItemHeightQuantum( TEXTURE_ITEM_HEIGHT_QUANTUM ),
  mCursorX( 0 ),
  mCursorY( 0 )
{
  mAtlasTexture = Dali::BufferImage::New( textureWidth, textureHeight, Dali::Pixel::RGBA8888 );
}

void SvgAtlas::AtlasAlloc( std::vector<unsigned char>& buffer,
		                      unsigned int *px,
		                      unsigned int *py,
		                      bool encodePath)
{
  unsigned int w, h, x, y;
  unsigned int len = buffer.size() / 4u;

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
    TexSubImage( x, y, w, h, &buffer[0]);
  }
  else
  {
    TexSubImage( x, y, w, h-1, &buffer[0]);
    // Upload the last row separately
    TexSubImage( x, y + h - 1, len - (w * (h - 1)), 1 , &buffer[0] + w * 4*(h - 1));
  }

  if( encodePath )
  {
    *px = x / mItemWidth;
    *py = y / mItemHeightQuantum;
  }
  else
  {
    *px = x;
    *py = y;
  }
}

void SvgAtlas::TexSubImage( unsigned int offsetX,
		                    unsigned int offsetY,
		                    unsigned int width,
		                    unsigned int height,
		                    unsigned char* data)
{
  Dali::PixelBuffer* pixbuf = mAtlasTexture.GetBuffer();
  size_t pos;
  size_t dataIndex = 0;
  for( size_t y= offsetY; y< height + offsetY; y++ )
  {
	pos = y * mTextureWidth * 4;
    for( size_t x = offsetX; x < width + offsetX; x++ )
    {
      pixbuf[pos+x*4] =  0xFF & data[dataIndex];
      pixbuf[pos+x*4+1] = 0xFF & data[dataIndex+1];
      pixbuf[pos+x*4+2] = 0xFF & data[dataIndex+2];
      pixbuf[pos+x*4+3] = 0xFF & data[dataIndex+3];
      dataIndex+=4;
    }
  }
 //mAtlasTexture.Update();
}
