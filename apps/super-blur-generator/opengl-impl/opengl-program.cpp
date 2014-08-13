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

#include "opengl-program.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <iostream>
#include <fstream>

#include "logging.h"

class FileString
{
public:
  FileString( const std::string& filename )
  {
    std::ifstream file( filename.c_str() );
    if ( file )
    {
      file.seekg( 0, std::ios::end );
      mContents.resize( file.tellg() );
      file.seekg(0, std::ios::beg);
      file.read(&(mContents[0]), mContents.size());
    }
  }
  const std::string& GetContents() const
  {
    return mContents;
  }
private:
  std::string mContents;
};

OpenGLProgram::OpenGLProgram( OpenGLContext& context,
                              const std::string& vertexShaderFile,
                              const std::string& fragmentShaderFile )
: mContext( context ),
  mProgramObject( 0 )
{
  if ( ! Initialize( vertexShaderFile, fragmentShaderFile ) )
  {
    LOGE("Failed to create program.\n");
  }
}

OpenGLProgram::~OpenGLProgram()
{
  Terminate();
}

void OpenGLProgram::Activate()
{
  if ( mProgramObject )
  {
    // Use the program object
    glUseProgram( mProgramObject );
  }
}

void OpenGLProgram::SetFloatUniform( const std::string& name, float value )
{
  GLint location;
  UniformsMap::iterator it = mUniformLocations.find( name );
  if ( it != mUniformLocations.end() )
  {
    location = it->second;
  }
  else
  {
    location = glGetUniformLocation( mProgramObject, name.c_str() );
  }
  if ( location > 0 )
  {
    glUniform1f( location, value );
  }
}

GLuint OpenGLProgram::GetAttributeLocation( const std::string& name )
{
  return glGetAttribLocation( mProgramObject, name.c_str() );
}

bool OpenGLProgram::Initialize( const std::string& vertexShaderFile,
                                const std::string& fragmentShaderFile )
{
  bool failed = true;

  printf("Vertex shader path: %s\n", vertexShaderFile.c_str());
  printf("Frag shader path: %s\n", fragmentShaderFile.c_str());

  GLuint vertexShader = 0;
  {
    FileString vertSource( vertexShaderFile );
    if ( ! vertSource.GetContents().empty() )
    {
      printf("Compiling Vertex Shader:\n");
      vertexShader = LoadShader( GL_VERTEX_SHADER, vertSource.GetContents() );
    }
  }

  GLuint fragmentShader = 0;
  {
    FileString fragSource( fragmentShaderFile );
    if ( ! fragSource.GetContents().empty() )
    {
      printf("Compiling Fragment Shader:\n");
      fragmentShader = LoadShader( GL_FRAGMENT_SHADER, fragSource.GetContents() );
    }
  }

  if ( vertexShader && fragmentShader )
  {
    // Create the program object
    mProgramObject = glCreateProgram();
    if ( ! mProgramObject )
    {
      LOGW( "glCreateProgram failed!\n" );
    }

    if ( mProgramObject )
    {
      glAttachShader( mProgramObject, vertexShader );
      glAttachShader( mProgramObject, fragmentShader );

      // Link the program
      glLinkProgram( mProgramObject );

      // check the link status
      GLint linked;
      glGetProgramiv( mProgramObject, GL_LINK_STATUS, &linked );

      failed = ! linked;
      if ( failed )
      {
        LOGW("glLinkProgram failed!\n");
        GLint infoLength = 0;

        glGetProgramiv( mProgramObject, GL_INFO_LOG_LENGTH, &infoLength );

        if( infoLength > 1 )
        {
          std::string info( infoLength, '\0' );
          glGetProgramInfoLog( mProgramObject, infoLength, NULL, &(info[0]) );

          LOGW("Error linking program:\n%s\n", info.c_str());
        }

        glDeleteProgram( mProgramObject );

        // set program to invalid value
        mProgramObject = 0;
      }
    }
  }

  return ! failed;
}

void OpenGLProgram::Terminate()
{
  if ( 0 != mProgramObject )
  {
    glDeleteProgram( mProgramObject );
  }
}

GLuint OpenGLProgram::LoadShader( GLenum type, const std::string& source )
{
  GLuint shader;

  // Create the shader object
  shader = glCreateShader( type );
  if ( 0 == shader )
  {
    LOGW("glCreateShader failed!\n");
  }

  if( shader != 0 )
  {
    // Load the shader source
    const char* shaderSourcePtr = source.c_str();
    glShaderSource( shader, 1, &shaderSourcePtr, NULL );

    // Compile the shader
    glCompileShader( shader );

    // Check the compile status
    GLint compiled;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

    if( ! compiled )
    {
      GLint infoLength = 0;

      glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLength );

      if( infoLength > 1 )
      {
        std::string info( infoLength, '\0' );
        glGetShaderInfoLog( shader, infoLength, NULL, &(info[0]) );

        LOGW("Error compiling shader:\n%s\n", info.c_str());
      }

      glDeleteShader( shader );

      // set shader to invalid value
      shader = 0;
    }
  }
  return shader;
}
