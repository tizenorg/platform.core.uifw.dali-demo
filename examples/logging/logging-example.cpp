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

#include "../shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;

// Define this so that it is interchangeable
// "DP" stands for Device independent Pixels
#define DP(x) DemoHelper::ScalePointSize(x)

enum ButtonType
{
  PUSH_BUTTON,
  TOGGLE_BUTTON
};

struct ButtonItem
{
  ButtonType type;
  const char* name;
  const char* text;
  const char* altText;
};

namespace
{

const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DALI_IMAGE_DIR "top-bar.png";

const char* const TOOLBAR_TITLE = "Logging";
const int TOOLBAR_HEIGHT = 62;

const char* const PUSHBUTTON_PRESS_IMAGE = DALI_IMAGE_DIR "button-down.9.png";
const char* const PUSHBUTTON_BUTTON_IMAGE = DALI_IMAGE_DIR "button-up.9.png";
const char* const PUSHBUTTON_DIM_IMAGE = DALI_IMAGE_DIR "button-disabled.9.png";

// Item view
const char* const CREATE_BUTTON1_ID = "CREATE_BUTTON1";
const char* const DELETE_BUTTON1_ID = "DELETE_BUTTON1";
const char* const TOGGLE_TIMER_BUTTON1_ID = "TOGGLE_TIMER_BUTTON1";
const char* const HIGH_FREQ_BUTTON1_ID = "INC_FREQ_BUTTON1";
const char* const LOW_FREQ_BUTTON1_ID = "DEC_FREQ_BUTTON1";
const char* const TOGGLE_ENABLE_BUTTON1_ID = "TOGGLE_ENABLE_BUTTON1";

const char* const CREATE_BUTTON2_ID = "CREATE_BUTTON2";
const char* const DELETE_BUTTON2_ID = "DELETE_BUTTON2";
const char* const TOGGLE_TIMER_BUTTON2_ID = "TOGGLE_TIMER_BUTTON2";
const char* const HIGH_FREQ_BUTTON2_ID = "INC_FREQ_BUTTON2";
const char* const LOW_FREQ_BUTTON2_ID = "DEC_FREQ_BUTTON2";
const char* const TOGGLE_ENABLE_BUTTON2_ID = "TOGGLE_ENABLE_BUTTON2";

const char* const CREATE_BUTTON3_ID = "CREATE_BUTTON3";
const char* const DELETE_BUTTON3_ID = "DELETE_BUTTON3";
const char* const TOGGLE_TIMER_BUTTON3_ID = "TOGGLE_TIMER_BUTTON3";
const char* const HIGH_FREQ_BUTTON3_ID = "INC_FREQ_BUTTON3";
const char* const LOW_FREQ_BUTTON3_ID = "DEC_FREQ_BUTTON3";
const char* const TOGGLE_ENABLE_BUTTON3_ID = "TOGGLE_ENABLE_BUTTON3";

// Data for creating the buttons
const ButtonItem BUTTON_ITEMS[] = {
    { PUSH_BUTTON,   CREATE_BUTTON1_ID,        "Create1",    "" },          // Creates a new logger
    { PUSH_BUTTON,   CREATE_BUTTON2_ID,        "Create2",    "" },
    { PUSH_BUTTON,   CREATE_BUTTON3_ID,        "Create3",    "" },
    { PUSH_BUTTON,   DELETE_BUTTON1_ID,        "Delete1",    "" },          // Deletes the logger
    { PUSH_BUTTON,   DELETE_BUTTON2_ID,        "Delete2",    "" },
    { PUSH_BUTTON,   DELETE_BUTTON3_ID,        "Delete3",    "" },
    { TOGGLE_BUTTON, TOGGLE_ENABLE_BUTTON1_ID, "Disable1",   "Enable1" },   // Enables/disables the logger
    { TOGGLE_BUTTON, TOGGLE_ENABLE_BUTTON2_ID, "Disable2",   "Enable2" },
    { TOGGLE_BUTTON, TOGGLE_ENABLE_BUTTON3_ID, "Disable3",   "Enable3" },
    { TOGGLE_BUTTON, TOGGLE_TIMER_BUTTON1_ID,  "Start1",     "Stop1" },     // Trigger start/stop signals for the logger
    { TOGGLE_BUTTON, TOGGLE_TIMER_BUTTON2_ID,  "Start2",     "Stop2" },     // These signals must be raised before a vsync prints and clears the logger info
    { TOGGLE_BUTTON, TOGGLE_TIMER_BUTTON3_ID,  "Start3",     "Stop3" },
    { PUSH_BUTTON,   HIGH_FREQ_BUTTON1_ID,     "High Freq.1", "" },         // Log every 1 second
    { PUSH_BUTTON,   HIGH_FREQ_BUTTON2_ID,     "High Freq.2", "" },
    { PUSH_BUTTON,   HIGH_FREQ_BUTTON3_ID,     "High Freq.3", "" },
    { PUSH_BUTTON,   LOW_FREQ_BUTTON1_ID,      "Low Freq.1",  "" },         // Log every 10 seconds
    { PUSH_BUTTON,   LOW_FREQ_BUTTON2_ID,      "Low Freq.2",  "" },
    { PUSH_BUTTON,   LOW_FREQ_BUTTON3_ID,      "Low Freq.3",  "" }
};

const unsigned int BUTTON_ITEMS_COUNT = sizeof( BUTTON_ITEMS ) / sizeof( BUTTON_ITEMS[0] );

const unsigned int HIGH_FREQUENCY = 1;   // Seconds
const unsigned int LOW_FREQUENCY = 10;   // Seconds

}  // namespace

/**
 * This example is a test harness for performance loggers.
 *
 * Press one of the create buttons to create a logger. This will output on vsync at the default frequency (2 seconds).
 * In case nothing appears in the log, force a vsync by touching anywhere on the screen. Loggers can be deleted
 * with the delete buttons. They can be enabled or disabled in which case logging will appear or disappear in the console
 * respectively. To record information in a logger press the start and then stop button in succession quickly in between
 * the time period when it would print to the console. This is necessary as the logger is cleared of information when
 * it prints. The output will contain the smallest and largest times between start and stop recorded (minimum and maximum),
 * the total time recorded by the logger as well as the average and standard deviation of all the times recorded. The
 * frequency of log output can be set to high frequency (every 1 second) or low frequency (every 10 seconds).
 */
class LoggingController: public ConnectionTracker, public Toolkit::ItemFactory
{
 public:

  LoggingController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &LoggingController::Create );
  }

  ~LoggingController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Respond to key events
    Stage::GetCurrent().KeyEventSignal().Connect(this, &LoggingController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );

    Vector2 stageSize = Stage::GetCurrent().GetSize();

    mItemView = Toolkit::ItemView::New( *this );
    mItemView.SetParentOrigin( ParentOrigin::CENTER );
    mItemView.SetAnchorPoint( AnchorPoint::CENTER );
    mItemView.SetSize( stageSize );

    // Use a grid layout for tests
    Toolkit::GridLayoutPtr gridLayout = Toolkit::GridLayout::New();
    gridLayout->SetNumberOfColumns( 3 );
    gridLayout->SetTopMargin( DP(TOOLBAR_HEIGHT) + DP(20.0f) );
    gridLayout->SetBottomMargin( DP(100.0f) );
    gridLayout->SetRowSpacing( DP(5.0f) );
    mItemView.AddLayout( *gridLayout );

    float layoutWidth = Toolkit::IsHorizontal( gridLayout->GetOrientation() ) ? stageSize.height : stageSize.width;
    float gridItemSize = ( layoutWidth / gridLayout->GetNumberOfColumns() ) * 0.5f;
    gridLayout->SetScrollSpeedFactor( gridLayout->GetNumberOfColumns() / gridItemSize * 0.5f );

    mItemView.ActivateLayout( 0, Vector3(stageSize.x, stageSize.y, stageSize.x), 0.0f );

    mContentLayer.Add( mItemView );
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        // Exit application when click back or escape.
        mApplication.Quit();
      }
    }
  }

  bool OnButtonClicked(Toolkit::Button button)
  {
    if( button.GetName() == CREATE_BUTTON1_ID )
    {
      mPerformanceLogger1 = Dali::PerformanceLogger::New( "Test logger 1" );
    }
    else if ( button.GetName() == DELETE_BUTTON1_ID )
    {
      mPerformanceLogger1.Reset();
    }
    else if ( button.GetName() == HIGH_FREQ_BUTTON1_ID )
    {
      if( mPerformanceLogger1 )
      {
        mPerformanceLogger1.SetLoggingFrequency( HIGH_FREQUENCY );
      }
    }
    else if ( button.GetName() == LOW_FREQ_BUTTON1_ID )
    {
      if( mPerformanceLogger1 )
      {
        mPerformanceLogger1.SetLoggingFrequency( LOW_FREQUENCY );
      }
    }
    else if( button.GetName() == CREATE_BUTTON2_ID )
    {
      mPerformanceLogger2 = Dali::PerformanceLogger::New( "Test logger 2" );
    }
    else if ( button.GetName() == DELETE_BUTTON2_ID )
    {
      mPerformanceLogger2.Reset();
    }
    else if ( button.GetName() == HIGH_FREQ_BUTTON2_ID )
    {
      if( mPerformanceLogger2 )
      {
        mPerformanceLogger2.SetLoggingFrequency( HIGH_FREQUENCY );
      }
    }
    else if ( button.GetName() == LOW_FREQ_BUTTON2_ID )
    {
      if( mPerformanceLogger2 )
      {
        mPerformanceLogger2.SetLoggingFrequency( LOW_FREQUENCY );
      }
    }
    else if( button.GetName() == CREATE_BUTTON3_ID )
    {
      mPerformanceLogger3 = Dali::PerformanceLogger::New( "Test logger 3" );
    }
    else if ( button.GetName() == DELETE_BUTTON3_ID )
    {
      mPerformanceLogger3.Reset();
    }
    else if ( button.GetName() == HIGH_FREQ_BUTTON3_ID )
    {
      if( mPerformanceLogger3 )
      {
        mPerformanceLogger3.SetLoggingFrequency( HIGH_FREQUENCY );
      }
    }
    else if ( button.GetName() == LOW_FREQ_BUTTON3_ID )
    {
      if( mPerformanceLogger3 )
      {
        mPerformanceLogger3.SetLoggingFrequency( LOW_FREQUENCY );
      }
    }

    return true;
  }

  bool OnButtonToggled( Toolkit::Button button, bool state )
  {
    const ButtonItem* buttonItem = FindButtonItem( button.GetName() );
    if( buttonItem )
    {
      if( button.GetName() == TOGGLE_TIMER_BUTTON1_ID )
      {
        if( mPerformanceLogger1 )
        {
          mPerformanceLogger1.AddMarker( (state) ? Dali::PerformanceLogger::START_EVENT : Dali::PerformanceLogger::END_EVENT );
        }
      }
      else if( button.GetName() == TOGGLE_ENABLE_BUTTON1_ID )
      {
        if( mPerformanceLogger1 )
        {
          mPerformanceLogger1.EnableLogging( !state );
        }
      }
      else if( button.GetName() == TOGGLE_TIMER_BUTTON2_ID )
      {
        if( mPerformanceLogger2 )
        {
          mPerformanceLogger2.AddMarker( (state) ? Dali::PerformanceLogger::START_EVENT : Dali::PerformanceLogger::END_EVENT );
        }
      }
      else if( button.GetName() == TOGGLE_ENABLE_BUTTON2_ID )
      {
        if( mPerformanceLogger2 )
        {
          mPerformanceLogger2.EnableLogging( !state );
        }
      }
      else if( button.GetName() == TOGGLE_TIMER_BUTTON3_ID )
      {
        if( mPerformanceLogger3 )
        {
          mPerformanceLogger3.AddMarker( (state) ? Dali::PerformanceLogger::START_EVENT : Dali::PerformanceLogger::END_EVENT );
        }
      }
      else if( button.GetName() == TOGGLE_ENABLE_BUTTON3_ID )
      {
        if( mPerformanceLogger3 )
        {
          mPerformanceLogger3.EnableLogging( !state );
        }
      }

      Toolkit::PushButton pushButton = Toolkit::PushButton::DownCast( button );
      if( pushButton )
      {
        if( state )
        {
          pushButton.SetLabelText( buttonItem->altText );
        }
        else
        {
          pushButton.SetLabelText( buttonItem->text );
        }
      }
    }

    return true;
  }

 public: // From ItemFactory

   /**
    * @brief Return the number of items to display in the item view
    *
    * @return Return the number of items to display
    */
   virtual unsigned int GetNumberOfItems()
   {
     return BUTTON_ITEMS_COUNT;
   }

   /**
    * @brief Create a new item to populate the item view with
    *
    * @param[in] itemId The index of the item to create
    * @return Return the created actor for the given ID
    */
   virtual Actor NewItem(unsigned int itemId)
   {
     Toolkit::PushButton button = Toolkit::PushButton::New();
     button.SetName( BUTTON_ITEMS[itemId].name );
     button.SetLabelText( BUTTON_ITEMS[itemId].text );

     button.SetPressedImage( Dali::Image::New( PUSHBUTTON_PRESS_IMAGE ) );
     button.SetButtonImage( Dali::Image::New( PUSHBUTTON_BUTTON_IMAGE ) );
     button.SetDimmedImage( Dali::Image::New( PUSHBUTTON_DIM_IMAGE ) );

     switch( BUTTON_ITEMS[itemId].type )
     {
       case PUSH_BUTTON:
       {
         button.ClickedSignal().Connect( this, &LoggingController::OnButtonClicked );

         break;
       }

       case TOGGLE_BUTTON:
       {
         button.SetToggleButton( true );
         button.ToggledSignal().Connect( this, &LoggingController::OnButtonToggled );

         break;
       }
     }

     return button;
   }

 private:

   const ButtonItem* FindButtonItem( const std::string& name )
   {
     for( unsigned int i = 0; i < BUTTON_ITEMS_COUNT; ++i )
     {
       if( strcmp( BUTTON_ITEMS[i].name, name.c_str() ) == 0 )
       {
         return &BUTTON_ITEMS[i];
       }
     }

     return NULL;
   }

 private:

  Application&      mApplication;
  Toolkit::View     mView;                   ///< The View instance.
  Toolkit::ToolBar  mToolBar;                ///< The View's Toolbar.
  Layer             mContentLayer;           ///< Content layer

  Toolkit::ItemView mItemView;               ///< ItemView to hold buttons

  Dali::PerformanceLogger mPerformanceLogger1;
  Dali::PerformanceLogger mPerformanceLogger2;
  Dali::PerformanceLogger mPerformanceLogger3;

};

void RunTest( Application& application )
{
  LoggingController test( application );

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
