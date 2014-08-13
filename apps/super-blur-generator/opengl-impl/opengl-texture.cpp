/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali Adaptor

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

#include "opengl-texture.h"
#include "logging.h"
#include <dali/dali.h>
#include <dali/integration-api/bitmap.h>

OpenGLTexture::OpenGLTexture( OpenGLContext& context, const Dali::BitmapLoader& image )
: mContext( context ),
  mTextureObject( 0 )
{
  glGenTextures( 1, &mTextureObject );
  glBindTexture( GL_TEXTURE_2D, mTextureObject );

  Dali::Pixel::Format pixelFormat = image.GetPixelFormat();
  unsigned int glDataType, glPixelFormat;
  Dali::Integration::ConvertToGlFormat( pixelFormat,
                                        glDataType,
                                        glPixelFormat );

  glTexImage2D( GL_TEXTURE_2D,
                0,
                glPixelFormat,
                image.GetImageWidth(),
                image.GetImageHeight(),
                0,
                glPixelFormat,
                glDataType,
                image.GetPixelData() );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

OpenGLTexture::~OpenGLTexture()
{
  glDeleteTextures( 1, &mTextureObject );
}
