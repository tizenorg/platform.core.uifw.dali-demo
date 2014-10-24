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

namespace
{

const char* const BACKGROUND_IMAGE = DALI_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DALI_IMAGE_DIR "top-bar.png";

const char* const TOOLBAR_TITLE = "Text Input";
const int TOOLBAR_HEIGHT = 80;

const int NUM_TABLE_ROWS = 12;
const int NUM_TABLE_COLUMNS = 12;

}  // namespace

/**
 * Helper structs
 */
struct TableCell
{
  unsigned int row;
  unsigned int column;
  unsigned int rowSpan;
  unsigned int columnSpan;
};

struct TableString
{
  std::string text;

  Toolkit::Alignment::Type horizontalAlignment;
  Toolkit::Alignment::Type verticalAlignment;
  float padding;

  float orientation;

  TableCell cellPosition;
};

namespace
{
} // namespace

/**
 * This example shows the usage of TextView.
 */
class TextViewController: public ConnectionTracker
{
public:

  TextViewController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextViewController::Create );
  }

  ~TextViewController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    Stage stage = Stage::GetCurrent();

    // Respond to key events
    stage.KeyEventSignal().Connect(this, &TextViewController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );

    Toolkit::TextInput textInput = Toolkit::TextInput::New();
    textInput.SetInitialText("hello");
    textInput.SetSize(300.0f, 200.0f );
    textInput.SetPosition(200.0f, 200.0f );
    textInput.SetBackgroundColor( Vector4( 0.0, 0.0, 0.0, 0.1 ) );
    textInput.SetScrollEnabled( true );

    mContentLayer.Add( textInput );
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

private:

  Application&      mApplication;
  Toolkit::View     mView;                   ///< The View instance.
  Toolkit::ToolBar  mToolBar;                ///< The View's Toolbar.
  Layer             mContentLayer;           ///< Content layer

};

void RunTest( Application& application )
{
  TextViewController test( application );

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
