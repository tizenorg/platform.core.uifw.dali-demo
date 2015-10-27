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

#include <dali/dali.h>
#include <iostream>
#include <dali-toolkit/dali-toolkit.h>
#include <cstdarg>
#include <stdio.h>

using namespace Dali;
using namespace std;

std::string stringf(const char* format, ...)
{
    va_list arg_list;                                                           
    va_start(arg_list, format);                                                 

    // SUSv2 version doesn't work for buf NULL/size 0, so try printing
    // into a small buffer that avoids the double-rendering and alloca path too...
    char short_buf[256];                                                        
    const size_t needed = vsnprintf(short_buf, sizeof short_buf,
                                    format, arg_list) + 1;
    if (needed <= sizeof short_buf)
        return short_buf;

    // need more space...
    char* p = static_cast<char*>(alloca(needed));
    vsnprintf(p, needed, format, arg_list);
    return p;
}

// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    std::cout << "HelloWorldController::HelloWorldController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "HelloWorldController::Create" << std::endl;

    if(false) // quaternion element range
    {
      Quaternion q(1,0.1f,0.2f,0.3f);
      q.Normalize();
      cout << q << endl;
      cout << q.AsVector() << endl;
    }

    if(false) // from matrix
    {
      float Mref_raw[16] = { 0.535714f,  -0.622936f, 0.570053f,   0.0f,
        0.765794f,  0.642857f,  -0.0171693f, 0.0f,
        -0.355767f, 0.445741f,  0.821429f,   0.0f,
        0.0f,       0.0f,       0.0f,        1.0f};
      Matrix Mref( Mref_raw );

      //Quaternion q( M_PI/3.0f, Vector3( 1.0f, 2.0f, 3.0f ) );
      //Matrix M( q );
      //cout << Mref << endl;
      //cout << M << endl;
    }

    if(false) // quaternion dot vector (rotation)
    {
      Vector3 v0( 1.0f, 0.0f, 0.0f );
      Vector3 v1( 1.0f, 1.0f, 0.0f );
      v0.Normalize();
      v1.Normalize();
      Quaternion q( v0, v1 );
      cout << v0.Dot(v1) << endl;
      cout << q << endl;
      cout << v1 << endl;
      cout << q*v0 << endl;
    }

    if(false)  // zero elements normalization
    {
      Quaternion q( 0,0,0,0 );
      cout << q << endl;
      cout << q.Length() << endl;
      q.Normalize();
      cout << q << endl;
      cout << q.Normalized() << endl;
    }

    if(false)  // 720 deg problem
    {
      {
      Quaternion q( Radian(Degree(0)), Vector3(0,1,0) );
      cout << "0   " << q.AsVector() << " " << q << endl;
      }
      {
      Quaternion q( Radian(Degree(360)), Vector3(0,1,0) );
      cout << "360 " << q.AsVector() << " " << q << endl;
      }
      {
      Quaternion q( Radian(Degree(720)), Vector3(0,1,0) );
      cout << "720 " << q.AsVector() << " " << q << endl;
      }
      {
      Quaternion q;
      cout << "identity " << q.AsVector() << " " << q << endl;
      }
    }

    if(true)  // operator*
    {
      Vector3 a(1,2,3);
      Vector3 b(2,3,4);
      cout << a << endl;
      cout << b << endl;
      cout << a*b << endl;
    }

  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    //mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
