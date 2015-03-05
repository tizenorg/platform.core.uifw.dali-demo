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

/**
 * @file text-label-example.cpp
 * @brief Basic usage of TextLabel control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

// INTERNAL INCLUDES
#include "center-layout.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  const unsigned int KEY_ZERO = 10;
  const unsigned int KEY_ONE = 11;
  const unsigned int KEY_M = 58;
}

/**
 * @brief The main class of the demo.
 */
class TextLabelExample : public ConnectionTracker
{
public:

  TextLabelExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextLabelExample::Create );
  }

  ~TextLabelExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.SetBackgroundColor( Color::BLUE );
    stage.KeyEventSignal().Connect(this, &TextLabelExample::OnKeyEvent);
    Vector2 stageSize = stage.GetSize();

    CenterLayout centerLayout = CenterLayout::New();
    centerLayout.SetParentOrigin( ParentOrigin::CENTER );
    centerLayout.SetSize( stageSize.width*0.6f, stageSize.width*0.6f );
    stage.Add( centerLayout );

    mLabel = TextLabel::New();
    mLabel.SetBackgroundColor( Color::BLACK );
    centerLayout.Add( mLabel );

    mLabel.SetProperty( TextLabel::PROPERTY_MULTI_LINE, true );
    mLabel.SetProperty( TextLabel::PROPERTY_TEXT, "A Quick Brown Fox Jumps Over The Lazy Dog" );

    // TODO
    //Property::Value labelText = label.GetProperty( TextLabel::PROPERTY_TEXT );
    //std::cout << "Got text from label: " << labelText.Get< std::string >() << std::endl;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
      else if( event.IsCtrlModifier() )
      {
        switch( event.keyCode )
        {
          case KEY_ZERO: // fall through
          case KEY_ONE:
          {
            mLabel.SetProperty( TextLabel::PROPERTY_RENDERING_BACKEND, event.keyCode - 10 );
            break;
          }
          case KEY_M:
          {
            bool multiLine = mLabel.GetProperty<bool>( TextLabel::PROPERTY_MULTI_LINE );
            mLabel.SetProperty( TextLabel::PROPERTY_MULTI_LINE, !multiLine );
            break;
          }
        }
      }
    }
  }

private:

  Application& mApplication;

  TextLabel mLabel;
};

void RunTest( Application& application )
{
  TextLabelExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
