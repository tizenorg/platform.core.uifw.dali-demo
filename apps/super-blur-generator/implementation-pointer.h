#ifndef SUPER_BLUR_GENERATOR_IMPLEMENTATION_POINTER_H_
#define SUPER_BLUR_GENERATOR_IMPLEMENTATION_POINTER_H_

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
