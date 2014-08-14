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

#include "opengl-impl/opengl-textured-quad-renderer.h"
#include "opengl-impl/opengl-context.h"
#include "opengl-impl/opengl-program.h"
#include "opengl-impl/opengl-texture.h"

#include "logging.h"

OpenGLTexturedQuadRenderer::OpenGLTexturedQuadRenderer(
  OpenGLContext& context,
  OpenGLProgram& program,
  const OpenGLTexture& texture,
  float scale)
: mContext( context ),
  mTexture( texture ),
  mScale(scale)
{
  texture.Bind();

  GLfloat vVertices[] =
      { -1.f*scale, -1.f*scale, 0.f, 0.f,
         1.f*scale, -1.f*scale, 1.f, 0.f,
        -1.f*scale,  1.f*scale, 0.f, 1.f,
         1.f*scale,  1.f*scale, 1.f, 1.f };

  glGenBuffers(1, &mVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), &vVertices[0], GL_DYNAMIC_DRAW);

  // Check whether the program supports the expected attributes/uniforms
  const GLint positionLoc = program.GetAttributeLocation( "aPosition" );
  const GLint texCoordLoc = program.GetAttributeLocation( "aTextureCoords" );

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
