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

const char* const TOOLBAR_TITLE = "Text View";
const int TOOLBAR_HEIGHT = 80;

const int NUM_TABLE_ROWS = 12;
const int NUM_TABLE_COLUMNS = 12;

}  // namespace

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

    // Create a table view the height of the stage
    Toolkit::TableView textContainer = Toolkit::TableView::New( NUM_TABLE_ROWS, NUM_TABLE_COLUMNS );
    textContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
    textContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textContainer.SetPosition( 0, TOOLBAR_HEIGHT );
    textContainer.SetSize( stage.GetSize().width, stage.GetSize().height - TOOLBAR_HEIGHT );

    mContentLayer.Add( textContainer );

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 13.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::EXTRABOLD );
      textStyle.SetTextColor( Vector4( 1.0f, 0.5f, 0.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "Hello!" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 0, 0, 2, 4 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 8.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::REGULAR );
      textStyle.SetTextColor( Vector4( 0.5f, 1.0f, 0.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "Howdy" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 0.0f, 0.0f, 10.0f, 0.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 0, 4, 1, 4 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 18.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::REGULAR );
      textStyle.SetTextColor( Vector4( 1.0f, 0.75f, 0.25f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "שלום" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 1, 4, 1, 4 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 8.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::REGULAR );
      textStyle.SetTextColor( Vector4( 0.5f, 0.0f, 1.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "Hi there" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalBottom );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 0.0f, 0.0f, 10.0f, 0.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 0, 8, 2, 4 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 12.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::REGULAR );
      textStyle.SetTextColor( Vector4( 0.5f, 1.0f, 1.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "Hola" );
      textView.SetStyleToCurrentText( textStyle );
      textView.SetRotation( Dali::Degree( 90.0f ), Vector3( 0.0f, 0.0f, 1.0f ) );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 0.0f, 10.0f, 0.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 2, 0, 4, 2 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 18.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::BOLD );
      textStyle.SetTextColor( Vector4( 0.5f, 1.0f, 0.5f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "Bonjour" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 2, 2, 2, 4 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 12.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::REGULAR );
      textStyle.SetTextColor( Vector4( 1.0f, 1.0f, 0.5f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "Ciao" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 2, 7, 2, 3 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 26.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::EXTRABLACK );
      textStyle.SetTextColor( Vector4( 0.5f, 0.0f, 0.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "Hello" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 4, 2, 1, 6 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 8.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::DEMIBOLD );
      textStyle.SetTextColor( Vector4( 0.0f, 0.5f, 0.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "Top of the morning to you" );
      textView.SetStyleToCurrentText( textStyle );
      textView.SetRotation( Dali::Degree( 90.0f ), Vector3( 0.0f, 0.0f, 1.0f ) );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 4, 10, 8, 2 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 13.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::DEMIBOLD );
      textStyle.SetTextColor( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "हैलो" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 6, 1, 1, 3 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 8.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::DEMIBOLD );
      textStyle.SetTextColor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "สวัสดี" );
      textView.SetStyleToCurrentText( textStyle );
      textView.SetRotation( Dali::Degree( 90.0f ), Vector3( 0.0f, 0.0f, 1.0f ) );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 6, 5, 2, 1 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 18.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::REGULAR );
      textStyle.SetTextColor( Vector4( 0.0f, 1.0f, 1.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "你好" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalBottom );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 6, 6, 1, 3 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 34.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::REGULAR );
      textStyle.SetTextColor( Vector4( 0.0f, 0.0f, 1.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "G'day" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 7, 0, 2, 10 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 16.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::EXTRABLACK );
      textStyle.SetTextColor( Vector4( 0.0f, 0.5f, 1.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "مرحبا" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 9, 1, 2, 4 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 10.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::EXTRABLACK );
      textStyle.SetTextColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "こんにちは" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalCenter );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 10, 0, 2, 6 ) );
    }

    {
      Dali::TextStyle textStyle;
      textStyle.SetFontName( "HelveticaNue" );
      textStyle.SetFontStyle( "Regular" );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( 14.0f ) ) );
      textStyle.SetWeight( Dali::TextStyle::REGULAR );
      textStyle.SetTextColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );

      Toolkit::TextView textView = Toolkit::TextView::New( "aloha" );
      textView.SetStyleToCurrentText( textStyle );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::VerticalTop );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( 10.0f, 10.0f, 10.0f, 10.0f ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, Toolkit::TableView::CellPosition( 10, 6, 2, 4 ) );
    }
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
