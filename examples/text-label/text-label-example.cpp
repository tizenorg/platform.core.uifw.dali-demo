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
#include "shared/multi-language-strings.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{
  const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "button-up.9.png";

  const unsigned int KEY_ZERO = 10;
  const unsigned int KEY_ONE = 11;
  const unsigned int KEY_H = 43;
  const unsigned int KEY_V = 55;
  const unsigned int KEY_M = 58;
  const unsigned int KEY_L = 46;
  const unsigned int KEY_S = 39;
  const unsigned int KEY_PLUS = 21;
  const unsigned int KEY_MINUS = 20;

  const char* H_ALIGNMENT_STRING_TABLE[] =
  {
    "BEGIN",
    "CENTER",
    "END"
  };

  const unsigned int H_ALIGNMENT_STRING_COUNT = sizeof( H_ALIGNMENT_STRING_TABLE ) / sizeof( H_ALIGNMENT_STRING_TABLE[0u] );

  const char* V_ALIGNMENT_STRING_TABLE[] =
  {
    "TOP",
    "CENTER",
    "BOTTOM"
  };

  const unsigned int V_ALIGNMENT_STRING_COUNT = sizeof( V_ALIGNMENT_STRING_TABLE ) / sizeof( V_ALIGNMENT_STRING_TABLE[0u] );

  int ConvertToEven(int value)
  {
    return (value % 2 == 0) ? value : (value + 1);
  }
}

/**
 * @brief The main class of the demo.
 */
class TextLabelExample : public ConnectionTracker
{
public:

  TextLabelExample( Application& application )
  : mApplication( application ),
    mLanguageId( 0u ),
    mAlignment( 0u )
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

    stage.KeyEventSignal().Connect(this, &TextLabelExample::OnKeyEvent);
    Vector2 stageSize = stage.GetSize();

    mContainer = Control::New();
    mContainer.SetName( "Container" );
    mContainer.SetParentOrigin( ParentOrigin::CENTER );
    mContainer.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    mLayoutSize = Vector2(stageSize.width*0.6f, stageSize.width*0.6f);
    mContainer.SetPreferredSize( mLayoutSize );
    mContainer.SetBackgroundImage( ResourceImage::New( BACKGROUND_IMAGE ) );
    mContainer.GetChildAt(0).SetZ(-1.0f);
    stage.Add( mContainer );

    // Resize the center layout when the corner is grabbed
    mGrabCorner = Control::New();
    mGrabCorner.SetName( "GrabCorner" );
    mGrabCorner.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    mGrabCorner.SetParentOrigin( ParentOrigin::BOTTOM_RIGHT );
    mGrabCorner.SetResizePolicy( FIXED, ALL_DIMENSIONS );
    mGrabCorner.SetPreferredSize( Vector2(stageSize.width*0.1f, stageSize.width*0.1f) );
    mGrabCorner.SetZ(1.0f);
    mContainer.Add( mGrabCorner );

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( mGrabCorner );
    mPanGestureDetector.DetectedSignal().Connect( this, &TextLabelExample::OnPan );

    mLabel = TextLabel::New( "A Quick Brown Fox Jumps Over The Lazy Dog" );
    mLabel.SetName( "TextLabel" );
    mLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mLabel.SetResizePolicy( FILL_TO_PARENT, HEIGHT );
    mLabel.SetProperty( TextLabel::Property::MULTI_LINE, true );
    mLabel.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 1.0f, 1.0f ) );
    mLabel.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLACK );
    mContainer.Add( mLabel );

    Property::Value labelText = mLabel.GetProperty( TextLabel::Property::TEXT );
    std::cout << "Displaying text: \"" << labelText.Get< std::string >() << "\"" << std::endl;
  }

  // Resize the text-label with pan gesture
  void OnPan( Actor actor, const PanGesture& gesture )
  {
    mLayoutSize.x += gesture.displacement.x * 2.0f;
    mLayoutSize.y += gesture.displacement.y * 2.0f;

    if( mLayoutSize.x >= 2.0f &&
        mLayoutSize.y >= 2.0f )
    {
      // Avoid pixel mis-alignment issue
      Vector2 clampedSize = Vector2( ConvertToEven(static_cast<int>(mLayoutSize.x)),
                                     ConvertToEven(static_cast<int>(mLayoutSize.y)) );

      mContainer.SetPreferredSize( clampedSize );
    }
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
            mLabel.SetProperty( TextLabel::Property::RENDERING_BACKEND, event.keyCode - 10 );
            break;
          }
          case KEY_H:
          {
            if( ++mAlignment >= H_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, H_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_V:
          {
            if( ++mAlignment >= V_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, V_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_M:
          {
            bool multiLine = mLabel.GetProperty<bool>( TextLabel::Property::MULTI_LINE );
            mLabel.SetProperty( TextLabel::Property::MULTI_LINE, !multiLine );
            break;
          }
          case KEY_L:
          {
            const Language& language = LANGUAGES[ mLanguageId ];

            mLabel.SetProperty( TextLabel::Property::TEXT, language.text );

            if( ++mLanguageId >= NUMBER_OF_LANGUAGES )
            {
              mLanguageId = 0u;
            }
            break;
          }
          case KEY_S:
          {
            if( Color::BLACK == mLabel.GetProperty<Vector4>( TextLabel::Property::SHADOW_COLOR ) )
            {
              mLabel.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::RED );
            }
            else
            {
              mLabel.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLACK );
            }
            break;
          }
          case KEY_PLUS:
          {
            mLabel.SetProperty( TextLabel::Property::SHADOW_OFFSET, mLabel.GetProperty<Vector2>( TextLabel::Property::SHADOW_OFFSET ) + Vector2( 1.0f, 1.0f ) );
            break;
          }
          case KEY_MINUS:
          {
            mLabel.SetProperty( TextLabel::Property::SHADOW_OFFSET, mLabel.GetProperty<Vector2>( TextLabel::Property::SHADOW_OFFSET ) - Vector2( 1.0f, 1.0f ) );
            break;
          }

        }
      }
    }
  }

private:

  Application& mApplication;

  TextLabel mLabel;

  Control mContainer;
  Actor mGrabCorner;

  PanGestureDetector mPanGestureDetector;

  Vector2 mLayoutSize;

  unsigned int mLanguageId;
  unsigned int mAlignment;
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
