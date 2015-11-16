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
 *
 */

#include <iostream>

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

typedef Signal< void ()> TestSignalType1;

class TestEmitter
{
public:

  TestEmitter()
  {
  }

  ~TestEmitter()
  {
  }

  TestSignalType1 signal1;
};

class TestController : public ConnectionTracker
{
public:

  TestController()
  {
  }

  ~TestController()
  {
  }

  void ConnectSignal1( TestEmitter& emitter )
  {
    emitter.signal1.Connect( this, &TestController::Callback1 );
  }

  void Callback1()
  {
    std::cout << "Callback 1" << std::endl;
  }
};

void RunTest()
{
  TestEmitter emitter;

  TestController controller;
  controller.ConnectSignal1( emitter );

  emitter.signal1.Emit();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  RunTest();

  return 0;
}

