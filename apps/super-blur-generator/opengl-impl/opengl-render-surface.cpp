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

#include "opengl-render-surface.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "logging.h"

OpenGLRenderSurface::OpenGLRenderSurface(
    OpenGLContext& context,
    unsigned int height,
    unsigned int width )
: mContext( context ),
  mHeight( height ),
  mWidth( width ),
  mSurface( EGL_NO_SURFACE )
{
  Initialize();
}

OpenGLRenderSurface::~OpenGLRenderSurface()
{
  Terminate();
}

bool OpenGLRenderSurface::Initialize()
{
  EGLint surfaceAttribs[9];
  surfaceAttribs[0] = EGL_HEIGHT;
  surfaceAttribs[1] = mHeight;
  surfaceAttribs[2] = EGL_WIDTH;
  surfaceAttribs[3] = mWidth;
  surfaceAttribs[4] = EGL_TEXTURE_FORMAT;
  surfaceAttribs[5] = EGL_TEXTURE_RGB;
  surfaceAttribs[6] = EGL_TEXTURE_TARGET;
  surfaceAttribs[7] = EGL_TEXTURE_2D;
  surfaceAttribs[8] = EGL_NONE;
  mSurface = eglCreatePbufferSurface( mContext.GetDisplay(),
                                      mContext.GetConfig(),
                                      surfaceAttribs );
  if ( EGL_NO_SURFACE == mSurface )
  {
    LOGW("eglCreatePbufferSurface failed\n");
    Terminate();
    return false;
  }

  /* connect the context to the surface */
  if ( EGL_FALSE ==
      eglMakeCurrent( mContext.GetDisplay(),
                      mSurface,
                      mSurface,
                      mContext.GetContext() ) )
  {
    LOGW("eglMakeCurrent failed\n");
    Terminate();
    return false;
  }

  /* clear the color buffer */
  glClearColor( 1.0, 1.0, 0.0, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT );
  glFlush();

  eglSwapBuffers( mContext.GetDisplay(), mSurface );

  return true;
}

void OpenGLRenderSurface::Terminate()
{
  eglMakeCurrent( mContext.GetDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
  if ( EGL_NO_SURFACE == mSurface )
  {
    eglDestroySurface( mContext.GetDisplay(), mSurface );
  }
}
