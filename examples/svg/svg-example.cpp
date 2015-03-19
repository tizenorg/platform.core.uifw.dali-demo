/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <stdio.h>
#include <string.h>
#include <math.h>
#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION      // Expands implementation
#include "nanosvg.h"

using namespace Dali;

// This example shows how to create and display Hello World! using a simple TextActor
//
class SVGController : public ConnectionTracker
{
public:

  SVGController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SVGController::Create );
  }

  ~SVGController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Load
    NSVGimage* image;
    image = nsvgParseFromFile(DALI_IMAGE_DIR "drawing.svg", "px", 96);
     std::cout<<"size: "<<image->width<<"x"<<image->height<<std::endl;
    // Use...
    for (NSVGshape* shape = image->shapes; shape != NULL; shape = shape->next)
    {
      std::string fillRule = (shape->fillRule == 0)?"nonzero":"evenodd";
      std::cout<<"shape: \n"
               <<"  fill color: "<< RGB_Color( (shape->fill).color )<<"\n"
               <<"  fill rule: "<< fillRule <<"\n"
               <<"  stroke color: "<< RGB_Color( (shape->stroke).color )<<"\n"
               << " stroke width: "<< shape->strokeWidth<<std::endl;

        for (NSVGpath* path = shape->paths; path != NULL; path = path->next) {
            for (int i = 0; i < path->npts-1; i += 3) {
     //           float* p = &path->pts[i*2];
    //            std::cout<<p[0]<<" "<<p[1]<<" "<< p[2]<<" "<<p[3]<<" "<<p[4]<<" "<<p[5]<<" "<<p[6]<<" "<<p[7]<<std::endl;
            }
        }
    }
    // Delete
    nsvgDelete(image);

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    stage.KeyEventSignal().Connect(this, &SVGController::OnKeyEvent);
  }

  Vector3 RGB_Color( unsigned int color )
  {
    unsigned int r = color & 0xFF;
    unsigned int g = (color >> 8) & 0xFF;
    unsigned int b = (color >> 16)& 0xFF;
    return Vector3( r, g, b);
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  SVGController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
