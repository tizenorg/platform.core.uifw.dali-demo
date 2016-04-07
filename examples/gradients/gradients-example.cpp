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

#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const APPLICATION_TITLE( "Color Gradients" );

const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char * const CHANGE_ICON( DEMO_IMAGE_DIR "icon-change.png" );
const char * const CHANGE_ICON_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );
}

// This example shows how to render color gradients
//
class GradientController : public ConnectionTracker
{
public:

  GradientController( Application& application )
  : mApplication( application ),
    mIndex( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &GradientController::Create );
  }

  ~GradientController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.KeyEventSignal().Connect(this, &GradientController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    Toolkit::ToolBar toolBar;
    Layer content = DemoHelper::CreateView( application,
                                            mView,
                                            toolBar,
                                            "",
                                            TOOLBAR_IMAGE,
                                            APPLICATION_TITLE );

// ---- Gradient for background

    Dali::Property::Map borderMap;
    borderMap[ "rendererType" ] = "border";
    borderMap[ "color"  ] = Vector4( 129.f, 198.f, 193.f, 255.f )/255.f;
    borderMap[ "size"   ] = 15.0f;

    mView.SetProperty( Control::Property::BACKGROUND, borderMap );

  }


  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
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

  Control mView;
  unsigned mIndex;
};

void RunTest( Application& application )
{
  GradientController test( application );

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
