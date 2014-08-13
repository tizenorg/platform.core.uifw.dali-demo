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

#include "opengl-impl/opengl-context.h"

#include<vector>

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
