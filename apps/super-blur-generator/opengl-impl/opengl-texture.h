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

#ifndef SUPER_BLUR_GENERATOR_OPENGL_TEXTURE_H_
#define SUPER_BLUR_GENERATOR_OPENGL_TEXTURE_H_

#include <stdio.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <dali/dali.h>

#include "opengl-context.h"


class OpenGLTexture
{
public:
  OpenGLTexture( OpenGLContext& context, const Dali::BitmapLoader& image );
  ~OpenGLTexture();

private:
  OpenGLContext& mContext;
  GLuint mTextureObject;
};

#endif // SUPER_BLUR_GENERATOR_OPENGL_TEXTURE_H_
