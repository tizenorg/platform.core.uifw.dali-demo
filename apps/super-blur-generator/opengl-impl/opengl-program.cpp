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
                              const std::string& prefix,
                              const std::string& vertexShaderFile,
                              const std::string& fragmentShaderFile )
: mContext( context ),
  mProgramObject( 0 )
{
  if ( ! Initialize( prefix, vertexShaderFile, fragmentShaderFile ) )
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

void OpenGLProgram::SetVector2Uniform( const std::string& name, float value1, float value2 )
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
    glUniform2f( location, value1, value2 );
  }
}

GLuint OpenGLProgram::GetAttributeLocation( const std::string& name )
{
  GLuint location = glGetAttribLocation( mProgramObject, name.c_str() );
  return location;
}

bool OpenGLProgram::Initialize( const std::string& prefix,
                                const std::string& vertexShaderFile,
                                const std::string& fragmentShaderFile )
{
  bool failed = true;

  GLuint vertexShader = 0;
  {
    FileString vertFile( vertexShaderFile );
    if ( ! vertFile.GetContents().empty() )
    {
      std::string vertexSource = prefix;
      vertexSource += vertFile.GetContents();
      vertexShader = LoadShader( GL_VERTEX_SHADER, vertexSource );
    }
  }

  GLuint fragmentShader = 0;
  {
    FileString fragFile( fragmentShaderFile );
    if ( ! fragFile.GetContents().empty() )
    {
      std::string fragmentSource = prefix + fragFile.GetContents();
      fragmentShader = LoadShader( GL_FRAGMENT_SHADER,  fragmentSource);
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
