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

#include "opengl-texture.h"
#include "logging.h"
#include <dali/dali.h>
#include <dali/integration-api/bitmap.h>

OpenGLTexture::OpenGLTexture( )
: mTextureObject( 0 )
{
}

OpenGLTexture::OpenGLTexture( const Dali::BitmapLoader& image )
: mTextureObject( 0 )
{
  glGenTextures( 1, &mTextureObject );
  glBindTexture( GL_TEXTURE_2D, mTextureObject );

  Dali::Pixel::Format pixelFormat = image.GetPixelFormat();
  unsigned int glDataType, glPixelFormat;
  Dali::Integration::ConvertToGlFormat( pixelFormat,
                                        glDataType,
                                        glPixelFormat );

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // We always use tightly packed data
  glTexImage2D( GL_TEXTURE_2D,
                0,
                glPixelFormat,
                image.GetImageWidth(),
                image.GetImageHeight(),
                0,
                glPixelFormat,
                glDataType,
                image.GetPixelData() );

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

OpenGLTexture::~OpenGLTexture()
{
  glDeleteTextures( 1, &mTextureObject );
}

void OpenGLTexture::Bind() const
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture( GL_TEXTURE_2D, mTextureObject );
}
