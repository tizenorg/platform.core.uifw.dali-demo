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
#include <iostream>

// INTERNAL INCLUDES
#include "shared/multi-language-strings.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{
  const char* const BACKGROUND_IMAGE = DEMO_IMAGE_DIR "grab-handle.png";

  const unsigned int KEY_ZERO = 10;
  const unsigned int KEY_ONE = 11;
  const unsigned int KEY_F = 41;
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

  enum Labels
  {
    SMALL = 1u << 0,
    RTL = 1u << 1,
    LARGE = 1u << 2,
    RTL_LONG = 1u << 3,
    NONE = 1u << 6,
  };

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


  void CreateContainer( Control& control, std::string name , Size size )
  {
    control.SetName( name );
    control.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    control.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mLayoutSize = size;
    control.SetSize( mLayoutSize );
    control.SetDrawMode( DrawMode::OVERLAY_2D );
  }

  void SetUpLabel( TextLabel& label, std::string name )
  {
    label.SetName( name );
    label.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    label.SetProperty( TextLabel::Property::MULTI_LINE, false );
    label.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    label.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 1.0f, 1.0f ) );
    label.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLACK );

    Dali::Property::Map bgMap;
    bgMap[ "rendererType" ] = "color";
    bgMap[ "mixColor" ] = Color::BLUE;

    label.SetProperty( Control::Property::BACKGROUND, bgMap );
  }

  void EnableScrolling( Labels labels )
  {
    Actor label;
    switch( labels )
    {
      case LARGE:
      {
        label = mLabel;
        break;
      }
      case RTL:
      {
        label = mRtlLabel;
        break;
      }
      case SMALL:
      {
        label = mSmallLabel;
        break;
      }
     case RTL_LONG:
      {
        label = mRtlLongLabel;
        break;
      }
      case NONE:
      {
        return;
      }
    }

    if ( labels != NONE )
    {
      Property::Value value = label.GetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL);
      if (value.Get< bool >())
      {
        label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false  );
      }
      else
      {
        label.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true  );
      }
    }
  }

  bool OnRtlLongButtonClicked( Toolkit::Button button )
  {
    EnableScrolling( RTL_LONG );
    return true;
  }

  bool OnRtlButtonClicked( Toolkit::Button button )
  {
    EnableScrolling( RTL );
    return true;
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
//   Property::Value value = button.GetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL);
//
//    if (value.Get< bool >())
//    {
//      std::cout <<"Button setting to false" << std::endl;
//      button.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, false  );
//    }
//    else
//    {
//      std::cout <<"Button setting to true" << std::endl;
//      button.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true  );
//    }
//    return true;
    EnableScrolling( LARGE );
    return true;
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
    CreateContainer( mContainer, "Container", Size(stageSize.width*0.6f, stageSize.width*0.25f) );
    mContainer.SetPosition( 10.0f, 0.0f );
    stage.Add( mContainer );

    Control containerTwo = Control::New();
    CreateContainer( containerTwo, "ContainerTwo", Size(stageSize.width*0.6f, stageSize.width*0.25f) );
    stage.Add( containerTwo );
    containerTwo.SetPosition( 10.0f, -stageSize.width*0.6f );

    Control containerThree = Control::New();
    CreateContainer( containerThree, "ContainerThree", Size(stageSize.width*0.6f, stageSize.width*0.25f) );
    stage.Add( containerThree );
    containerThree.SetPosition( 10.0f, -stageSize.width*0.3f );

    Control containerFour = Control::New();
    CreateContainer( containerFour, "ContainerFour", Size(stageSize.width*0.6f, stageSize.width*0.25f) );
    stage.Add( containerFour );
    containerFour.SetPosition( 10.0f, -stageSize.width*0.15f );

    mLabel = TextLabel::New( "A Quick Brown Fox Jumps Over The Lazy Dog" );
    mSmallLabel = TextLabel::New( "A Quick Brown Fox" );
    mSmallLabel = TextLabel::New( "مرحبا بالعالم");
    mRtlLabel = TextLabel::New( "مرحبا بالعالما");
    mRtlLongLabel = TextLabel::New( "مرحبا بالعالمرحبا بالعالمرحبابالعالمرحبابالعالمرحبا بالعالمرحبا");

    SetUpLabel( mLabel, "TextLabel" );
    SetUpLabel( mSmallLabel, "smallLabel" );
    SetUpLabel( mRtlLabel, "rtlLabel" );
    SetUpLabel( mRtlLongLabel, "rtlLabel" );


    mSmallLabel.SetProperty( TextLabel::Property::ENABLE_AUTO_SCROLL, true  );

    mContainer.Add( mLabel );
    containerTwo.Add( mSmallLabel );
    containerThree.Add( mRtlLabel );
    containerFour.Add( mRtlLongLabel );

    Toolkit::PushButton scrollButton = Toolkit::PushButton::New();
    scrollButton.SetLabelText( "Scroll" );
    scrollButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    scrollButton.ClickedSignal().Connect( this, &TextLabelExample::OnButtonClicked );
    scrollButton.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    scrollButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mContainer.Add(scrollButton);

    Toolkit::PushButton rtlScrollButton = Toolkit::PushButton::New();
    rtlScrollButton.SetLabelText( "Scroll" );
    rtlScrollButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    rtlScrollButton.ClickedSignal().Connect( this, &TextLabelExample::OnRtlButtonClicked );
    rtlScrollButton.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    rtlScrollButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    containerThree.Add(rtlScrollButton);

    Toolkit::PushButton rtlLongScrollButton = Toolkit::PushButton::New();
    rtlLongScrollButton.SetLabelText( "Scroll" );
    rtlLongScrollButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    rtlLongScrollButton.ClickedSignal().Connect( this, &TextLabelExample::OnRtlLongButtonClicked );
    rtlLongScrollButton.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    rtlLongScrollButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    containerFour.Add(rtlLongScrollButton);
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
          // Select rendering back-end
          case KEY_ZERO: // fall through
          case KEY_ONE:
          {
            mLabel.SetProperty( TextLabel::Property::RENDERING_BACKEND, event.keyCode - 10 );
            break;
          }
          case KEY_F: // Fill vertically
          {
            if( ResizePolicy::DIMENSION_DEPENDENCY == mLabel.GetResizePolicy(Dimension::HEIGHT) )
            {
              mLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
            }
            else
            {
              mLabel.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );
            }
            break;
          }
          case KEY_H: // Horizontal alignment
          {
            if( ++mAlignment >= H_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, H_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_V: // Vertical alignment
          {
            if( ++mAlignment >= V_ALIGNMENT_STRING_COUNT )
            {
              mAlignment = 0u;
            }

            mLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, V_ALIGNMENT_STRING_TABLE[ mAlignment ] );
            break;
          }
          case KEY_M: // Multi-line
          {
            bool multiLine = mLabel.GetProperty<bool>( TextLabel::Property::MULTI_LINE );
            mLabel.SetProperty( TextLabel::Property::MULTI_LINE, !multiLine );
            break;
          }
          case KEY_L: // Language
          {
            const Language& language = LANGUAGES[ mLanguageId ];

            mLabel.SetProperty( TextLabel::Property::TEXT, language.text );

            if( ++mLanguageId >= NUMBER_OF_LANGUAGES )
            {
              mLanguageId = 0u;
            }
            break;
          }
          case KEY_S: // Shadow color
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
          case KEY_PLUS: // Increase shadow offset
          {
            mLabel.SetProperty( TextLabel::Property::SHADOW_OFFSET, mLabel.GetProperty<Vector2>( TextLabel::Property::SHADOW_OFFSET ) + Vector2( 1.0f, 1.0f ) );
            break;
          }
          case KEY_MINUS: // Decrease shadow offset
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
  TextLabel mSmallLabel;
  TextLabel mRtlLabel;
  TextLabel mRtlLongLabel;

  Control mContainer;

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
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
