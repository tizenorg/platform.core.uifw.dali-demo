/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;
using Dali::Toolkit::ProgressBar;

namespace
{

const char* const BACKGROUND_IMAGE = DEMO_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DEMO_IMAGE_DIR "top-bar.png";
const char* const TOOLBAR_TITLE = "PROGRESS BAR DEMO";

const Vector4 BACKGROUND_COLOUR( 1.0f, 1.0f, 1.0f, 0.15f );

// Layout sizes
const int MARGIN_SIZE = 10;
const int TOP_MARGIN = 85;
const int RADIO_LABEL_THUMBNAIL_SIZE = 60;
const int RADIO_LABEL_THUMBNAIL_SIZE_SMALL = 40;
const int RADIO_IMAGE_SPACING = 8;
const int BUTTON_HEIGHT = 48;

}  // namespace

/** This example shows how to create and use PROGRESS BAR.
 */

class ProgressBarExample: public ConnectionTracker
{
 public:

  ProgressBarExample( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ProgressBarExample::Create );
  }

  ~ProgressBarExample()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    // Creates a default view with a default tool bar.
    // The view is added to the stage.

    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            TOOLBAR_TITLE );

  progressbar = ProgressBar::New();
  progressbar.SetParentOrigin(ParentOrigin::TOP_LEFT);
  progressbar.SetAnchorPoint(ParentOrigin::TOP_LEFT);
  progressbar.SetSize( 500.0f, 10.0f );
  progressbar.SetPosition( 0.0f, 0.0f );

  progressbar.SetProperty(ProgressBar::Property::LOWER_BOUND,        1.0f);
  progressbar.SetProperty(ProgressBar::Property::UPPER_BOUND,        100.0f);
  progressbar.SetProperty(ProgressBar::Property::VALUE,              5.0f);
  progressbar.SetProperty(ProgressBar::Property::DISABLED_COLOR,     Color::BLACK);

  //progressbar.SetProperty(ProgressBar::Property::VALUE_PRECISION,    4);
  //progressbar.SetProperty(ProgressBar::Property::MARKS,              false);
  //progressbar.SetProperty(ProgressBar::Property::MARK_TOLERANCE,     5.5f);


  Toolkit::TableView contentTable = Toolkit::TableView::New( 4, 1 );
  contentTable.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  contentTable.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  contentTable.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  contentTable.SetParentOrigin( ParentOrigin::TOP_LEFT );
  contentTable.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE * 0.5f ) );

  for( unsigned int i = 0; i < contentTable.GetRows(); ++i )
  {
     contentTable.SetFitHeight( i );
  }

  contentTable.SetPosition( 0.0f, TOP_MARGIN );
  mContentLayer.Add( contentTable );

  // Image selector for progress bar
  Toolkit::TableView radioGroup2Background = Toolkit::TableView::New( 2, 2 );
  radioGroup2Background.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  radioGroup2Background.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  radioGroup2Background.SetBackgroundColor( BACKGROUND_COLOUR );
  radioGroup2Background.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );
  radioGroup2Background.SetFitHeight( 0 );
  radioGroup2Background.SetFitHeight( 1 );
  radioGroup2Background.SetFitWidth( 0 );
  contentTable.Add( radioGroup2Background );
  radioGroup2Background.Add( progressbar );

  // Create togglabe button
  Toolkit::TableView toggleBackground = Toolkit::TableView::New( 3, 1 );
  toggleBackground.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  toggleBackground.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  toggleBackground.SetBackgroundColor( BACKGROUND_COLOUR );
  toggleBackground.SetCellPadding( Size( MARGIN_SIZE, MARGIN_SIZE ) );

  for( unsigned int i = 0; i < toggleBackground.GetRows(); ++i )
  {
     toggleBackground.SetFitHeight( i );
  }

  contentTable.Add( toggleBackground );
  mToggleButton = Toolkit::PushButton::New();
  mToggleButton.SetTogglableButton( true );
  mToggleButton.SetLabelText( "Set Progress" );
  mToggleButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mToggleButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  mToggleButton.StateChangedSignal().Connect( this, &ProgressBarExample::OnButtonSelected );

  toggleBackground.Add( mToggleButton );

 }

bool OnButtonSelected( Toolkit::Button button )
{
  //progressbar.Reset();
  progressbar.SetProperty(ProgressBar::Property::VALUE,              5.0f);

  return true;
}

private:

  Application&      mApplication;
  Toolkit::Control  mView;                              ///< The View instance.
  Toolkit::ToolBar  mToolBar;                           ///< The View's Toolbar.
  Layer             mContentLayer;                      ///< Content layer
  ProgressBar progressbar;
  Toolkit::PushButton mToggleButton;
};

void RunTest( Application& application )
{
  ProgressBarExample test( application );
  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );

  RunTest( application );

  return 0;
}
