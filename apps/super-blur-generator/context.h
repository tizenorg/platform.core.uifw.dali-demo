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

#ifndef SUPER_BLUR_GENERATOR_CONTEXT_H_
#define SUPER_BLUR_GENERATOR_CONTEXT_H_

#include "implementation-pointer.h"

class OpenGLContext;

/**
 * Represents a rendering context
 */
class Context
{
public:
  /**
   * Constructor
   */
  Context();

  /**
   * Destructor
   */
  ~Context();

private:
  ImplementationPointer<OpenGLContext> mImpl;

public:
  OpenGLContext& GetImpl()
  {
    return *mImpl;
  }
  const OpenGLContext& GetImpl() const
  {
    return *mImpl;
  }
};

#endif // SUPER_BLUR_GENERATOR_CONTEXT_H_
