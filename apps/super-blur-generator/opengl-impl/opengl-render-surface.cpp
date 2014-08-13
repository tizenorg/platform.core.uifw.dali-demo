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

#include "opengl-render-surface.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "logging.h"

OpenGLRenderSurface::OpenGLRenderSurface(
    OpenGLContext& context,
    unsigned int width,
    unsigned int height )
: mContext( context ),
  mHeight( height ),
  mWidth( width ),
  mSurface( EGL_NO_SURFACE )
{

  printf( "Width: %u, Height: %u\n", width, height);

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
  glClearColor( 1.0, 0.0, 1.0, 1.0 );
  glClear( GL_COLOR_BUFFER_BIT );
  glFlush();

  return true;
}

unsigned char* OpenGLRenderSurface::ReadPixelBuffer()
{
  glFlush();

  GLint readFormat;
  glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &readFormat);
  GLint readType;
  glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &readType);

  int bytesPerPixel = 4;
  if( readFormat == GL_RGB )
  {
    bytesPerPixel = 3;
  }
  // readType should be UNSIGNED_BYTE
  size_t bufferSize = mWidth * mHeight * bytesPerPixel;
  unsigned char* pixelData = new unsigned char[bufferSize];
  glReadPixels( 0, 0, mWidth, mHeight, readFormat, readType, pixelData );

  GLenum errorCode = glGetError();
  if( errorCode != GL_NO_ERROR )
  {
    printf("Can't read pixel data from render surface: error code=%4X\n", errorCode);
    delete[] pixelData;
    return NULL;
  }

  return pixelData;
}

void OpenGLRenderSurface::Terminate()
{
  eglMakeCurrent( mContext.GetDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
  if ( EGL_NO_SURFACE == mSurface )
  {
    eglDestroySurface( mContext.GetDisplay(), mSurface );
  }
}
