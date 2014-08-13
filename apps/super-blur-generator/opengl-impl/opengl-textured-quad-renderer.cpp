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
#include "opengl-impl/opengl-program.h"
#include "opengl-impl/opengl-textured-quad-renderer.h"

#include "logging.h"

OpenGLTexturedQuadRenderer::OpenGLTexturedQuadRenderer(
  OpenGLContext& context,
  OpenGLProgram& program,
  const OpenGLTexture& texture )
: mContext( context ),
  mTexture( texture )
{
  GLfloat vVertices[] =
      { -.5f, -.5f, 0.f, 0.f,
         .5f, -.5f, 1.f, 0.f,
         .5f,  .5f, 1.f, 1.f,
        -.5f,  .5f, 0.f, 1.f };

  glGenBuffers(1, &mVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), &vVertices[0], GL_DYNAMIC_DRAW);

  // Check whether the program supports the expected attributes/uniforms
  const GLint positionLoc = program.GetAttributeLocation( "aPosition" );
  const GLint texCoordLoc = program.GetAttributeLocation( "aTexCoords" );

  glEnableVertexAttribArray( positionLoc );
  const int stride = 4 * sizeof(float);
  glVertexAttribPointer( positionLoc, 2, GL_FLOAT, GL_FALSE, stride, 0 );

  glEnableVertexAttribArray( texCoordLoc );
  glVertexAttribPointer( texCoordLoc, 2, GL_FLOAT, GL_FALSE, stride, (const void*) (sizeof(float)*2) );

  const GLsizei vertexCount = 4;
  glDrawArrays( GL_TRIANGLE_STRIP, 0, vertexCount );
  glDisableVertexAttribArray( positionLoc );
  glDisableVertexAttribArray( texCoordLoc );
}

OpenGLTexturedQuadRenderer::~OpenGLTexturedQuadRenderer()
{
  glDeleteBuffers( 1, &mVertexBuffer );
}
