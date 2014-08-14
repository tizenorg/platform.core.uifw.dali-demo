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

#include "opengl-impl/opengl-context.h"

#include <vector>

#include "logging.h"

OpenGLContext::OpenGLContext()
: mDisplay( EGL_NO_DISPLAY ),
  mContext( EGL_NO_CONTEXT ),
  mConfig( NULL )
{
  if ( ! Initialize() )
  {
    LOGE( "Failed to initialize context.\n" );
  }
}

OpenGLContext::~OpenGLContext()
{
  Terminate();
}

EGLDisplay OpenGLContext::GetDisplay()
{
  return mDisplay;
}

EGLContext OpenGLContext::GetContext()
{
  return mContext;
}

EGLConfig OpenGLContext::GetConfig()
{
  return mConfig;
}

bool OpenGLContext::Initialize()
{
  /* get an EGL display connection */
  mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if ( EGL_NO_DISPLAY == mDisplay )
  {
    LOGW("eglGetDisplay failed\n");
    Terminate();
    return false;
  }

  /* initialize the EGL display connection */
  if ( EGL_TRUE != eglInitialize(mDisplay, NULL, NULL) )
  {
    LOGW("eglInitialize failed\n");
    Terminate();
    return false;
  }

  /* get an appropriate EGL frame buffer configuration */
  EGLint numConfig;
  std::vector<EGLint> configAttribs;
  configAttribs.push_back( EGL_SURFACE_TYPE );
  configAttribs.push_back( EGL_PBUFFER_BIT );
  configAttribs.push_back( EGL_RENDERABLE_TYPE );
  configAttribs.push_back( EGL_OPENGL_ES2_BIT );
  configAttribs.push_back( EGL_RED_SIZE );
  configAttribs.push_back( 8 );
  configAttribs.push_back( EGL_GREEN_SIZE );
  configAttribs.push_back( 8 );
  configAttribs.push_back( EGL_BLUE_SIZE );
  configAttribs.push_back( 8 );
  //configAttribs.push_back( EGL_ALPHA_SIZE );
  //configAttribs.push_back( 8 );
  configAttribs.push_back( EGL_NONE );

  eglChooseConfig(mDisplay, &(configAttribs[0]), &mConfig, 1, &numConfig);
  if ( 1 != numConfig )
  {
    EGLint error = eglGetError();

    switch ( error )
    {
      case EGL_BAD_DISPLAY:
      {
        LOGW("eglChooseConfig failed: EGL_BAD_DISPLAY\n");
        break;
      }
      case EGL_BAD_ATTRIBUTE:
      {
        LOGW("eglChooseConfig failed: EGL_BAD_ATTRIBUTE\n");
        break;
      }
      case EGL_NOT_INITIALIZED:
      {
        LOGW("eglChooseConfig failed: EGL_NOT_INITIALIZED\n");
        break;
      }
      case EGL_BAD_PARAMETER:
      {
        LOGW("eglChooseConfig failed: EGL_BAD_PARAMETER\n");
        break;
      }
      default:
      {
        LOGW("eglChooseConfig failed: unknown error!\n");
        break;
      }
    }
    Terminate();
    return false;
  }

  /* create an EGL rendering context */
  EGLint contextAttribs[3];
  contextAttribs[0] = EGL_CONTEXT_CLIENT_VERSION;
  contextAttribs[1] = 2;
  contextAttribs[2] = EGL_NONE;
  mContext = eglCreateContext(mDisplay, mConfig, EGL_NO_CONTEXT, contextAttribs);
  if ( EGL_NO_CONTEXT == mContext )
  {
    LOGW("eglCreateContext failed\n");
    Terminate();
    return false;
  }

  return true;
}

void OpenGLContext::Terminate()
{
  if ( EGL_NO_DISPLAY != mDisplay )
  {
    if ( EGL_NO_CONTEXT != mContext )
    {
      eglMakeCurrent( mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
      eglDestroyContext( mDisplay, mContext );
      mContext = EGL_NO_CONTEXT;
    }
    eglTerminate( mDisplay );
    mDisplay = EGL_NO_DISPLAY;
  }
}

void OpenGLContext::Prepare(size_t width, size_t height)
{
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  glViewport(0.0f, 0.0f, width, height);
}
