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

#include "opengl-framebuffer.h"

OpenGLFramebuffer::OpenGLFramebuffer(int width, int height)
: OpenGLTexture(),
  mWidth(width),
  mHeight(height),
  mFrameBufferName(0)
{
  glGenTextures(1, &mTextureObject);
  glActiveTexture(GL_TEXTURE7);  // bind in unused unit so rebind works the first time
  glBindTexture( GL_TEXTURE_2D, mTextureObject);

  // set texture parameters
  // Assign memory for texture in GL memory space
  GLenum pixelFormat = GL_RGBA;
  GLenum pixelDataType = GL_UNSIGNED_BYTE;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, mWidth, mHeight, 0, pixelFormat, pixelDataType, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // generate frame and render buffer names
  glGenFramebuffers(1, &mFrameBufferName);

  int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if ( GL_FRAMEBUFFER_COMPLETE != status )
  {
    printf("Framebuffer status: Error. GlError: %4X\n", glGetError());
  }
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
  glDeleteFramebuffers(1, &mFrameBufferName );
  //glDeleteRenderbuffers(1, &mRenderBufferName );
}


bool OpenGLFramebuffer::Prepare()
{
  glActiveTexture( GL_TEXTURE7 );
  glBindTexture(GL_TEXTURE_2D, mTextureObject);

  if( 0 != mTextureObject )
  {
    // bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferName);

    // attach texture to the color attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureObject, 0);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( GL_FRAMEBUFFER_COMPLETE != status )
    {
      printf("status (0x%x), glError (0x%x)\n", status, glGetError() );
    }
    glViewport(0.0f, 0.0f, mWidth, mHeight);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear( GL_COLOR_BUFFER_BIT );
    glFlush();

    return true;
  }

  // Texture could not be bound
  return false;
}

void OpenGLFramebuffer::Invalidate()
{

}
