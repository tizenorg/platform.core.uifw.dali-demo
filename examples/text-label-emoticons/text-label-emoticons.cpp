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

// INTERNAL INCLUDES
#include "vertical-layout.h"

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

 // TODO Need to expose Text::RENDER.....
const int ATLAS_RENDERER = 1;

// This example shows how to create and display Hello World! using a simple TextActor
//
class EmoticonController : public ConnectionTracker
{
public:

  typedef uint32_t SizeType;

  EmoticonController( Application& application )
  : mApplication( application )
  {
    std::cout << "EmoticonController::EmoticonController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &EmoticonController::Create );
  }

  ~EmoticonController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {

    mLayout = VerticalLayout::New();
    mLayout.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mLayout.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    Stage::GetCurrent().Add( mLayout );

    TextLabel label = TextLabel::New();
    label.SetProperty(TextLabel::PROPERTY_RENDERING_BACKEND, ATLAS_RENDERER );
    label.SetProperty( TextLabel::PROPERTY_MULTI_LINE, true );
    label.SetParentOrigin( ParentOrigin:: CENTER );
    label.SetProperty( TextLabel::PROPERTY_TEXT, "Smile with.....\xF0\x9F\x98\x89 Emoticons!" );
    mLayout.AddLabel( label );

    TextLabel label2 = TextLabel::New();
    label2.SetProperty(TextLabel::PROPERTY_RENDERING_BACKEND, ATLAS_RENDERER );
    label2.SetProperty( TextLabel::PROPERTY_MULTI_LINE, true );
    label2.SetParentOrigin( ParentOrigin:: CENTER );
    label2.SetProperty( TextLabel::PROPERTY_TEXT, "Say more........\xF0\x9F\x98\x8a with Emoticons" );
    mLayout.AddLabel( label2 );

    TextLabel label3 = TextLabel::New();
    label3.SetProperty(TextLabel::PROPERTY_RENDERING_BACKEND, ATLAS_RENDERER );
    label3.SetProperty( TextLabel::PROPERTY_MULTI_LINE, true );
    label3.SetParentOrigin( ParentOrigin:: CENTER );
    label3.SetProperty( TextLabel::PROPERTY_TEXT, "Say even more......\xF0\x9F\x98\x81 with Emoticons" );
    mLayout.AddLabel( label3 );

    TextLabel label4 = TextLabel::New();
    label4.SetProperty(TextLabel::PROPERTY_RENDERING_BACKEND, ATLAS_RENDERER );
    label4.SetProperty( TextLabel::PROPERTY_MULTI_LINE, true );
    label4.SetParentOrigin( ParentOrigin:: CENTER );
    label4.SetProperty( TextLabel::PROPERTY_TEXT, "Say even even more.....\xF0\x9F\x98\x82 with Emoticons" );
    mLayout.AddLabel( label4 );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  VerticalLayout mLayout;
};

void RunTest( Application& application )
{
  EmoticonController test( application );

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
