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

/**
 * SuperBlurGerator creates blur levels from an image file to be used with
 * the SuperBlurView
 */

#include "program.h"

#include "opengl-impl/opengl-context.h"
#include "opengl-impl/opengl-program.h"

Program::Program( Context& context,
                  const std::string& vertexShaderFile,
                  const std::string& fragmentShaderFile )
: mImpl( new OpenGLProgram( context.GetImpl(),
                            vertexShaderFile,
                            fragmentShaderFile ) )
{
}

Program::~Program()
{
}

/**
 * Activate this shader
 */
void Program::Activate()
{
  GetImpl().Activate();
}

/**
 * Set uniform with a float value
 */
void Program::SetFloatUniform( const std::string& name, float value )
{
  GetImpl().SetFloatUniform( name, value );
}
