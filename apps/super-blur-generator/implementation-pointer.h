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

#ifndef SUPER_BLUR_GENERATOR_IMPLEMENTATION_POINTER_H_
#define SUPER_BLUR_GENERATOR_IMPLEMENTATION_POINTER_H_

template<typename T>
class ImplementationPointer
{
public:
  explicit ImplementationPointer(T* p) : mPointer(p){}
  ~ImplementationPointer(){ delete mPointer; }

  T* operator->() { return mPointer; }
  const T* operator->() const { return mPointer; }
  T& operator*() { return *mPointer; }
  const T& operator*() const { return *mPointer; }

private: // Disabled methods
  ImplementationPointer(const ImplementationPointer&);
  ImplementationPointer& operator=(const ImplementationPointer&);

private:
  T* mPointer;
};

#endif // SUPER_BLUR_GENERATOR_IMPLEMENTATION_POINTER_H_

