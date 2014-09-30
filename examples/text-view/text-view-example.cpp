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
 * Helper struct
 */
struct TableString
{
  std::string fontName;
  std::string fontStyle;
  float fontSize;
  Dali::TextStyle::Weight fontWeight;
  Vector4 fontColour;

  std::string text;

  Toolkit::Alignment::Type horizontalAlignment;
  Toolkit::Alignment::Type verticalAlignment;
  float padding;

  float orientation;

  Toolkit::TableView::CellPosition cellPosition;

  TableString() {}
  TableString( const std::string& _fontName,
               const std::string& _fontStyle,
               float _fontSize,
               Dali::TextStyle::Weight _fontWeight,
               const Vector4& _fontColour,
               const std::string& _text,
               Toolkit::Alignment::Type _horizontalAlignment,
               Toolkit::Alignment::Type _verticalAlignment,
               float _padding,
               float _orientation,
               const Toolkit::TableView::CellPosition& _cellPosition )
    : fontName( _fontName ),
      fontStyle( _fontStyle ),
      fontSize( _fontSize ),
      fontWeight( _fontWeight ),
      fontColour( _fontColour ),
      text( _text ),
      horizontalAlignment( _horizontalAlignment ),
      verticalAlignment( _verticalAlignment ),
      padding( _padding ),
      orientation( _orientation ),
      cellPosition( _cellPosition )
    {}
};

typedef std::vector<TableString*> TableStringVector;

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

    // Data
    TableStringVector tableStrings;
    tableStrings.reserve( 32 );

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             8.0f,                                                // Font point size
                                             Dali::TextStyle::REGULAR,                            // Font weight
                                             Vector4( 0.5f, 1.0f, 0.0f, 1.0f ),                   // Text colour
                                             "Howdy",                                             // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalTop,                     // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 0, 0, 2, 4 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             13.0f,                                               // Font point size
                                             Dali::TextStyle::EXTRABOLD,                          // Font weight
                                             Vector4( 1.0f, 0.5f, 0.0f, 1.0f ),                   // Text colour
                                             "Hello!",                                            // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalCenter,                  // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 0, 4, 1, 4 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             18.0f,                                               // Font point size
                                             Dali::TextStyle::REGULAR,                            // Font weight
                                             Vector4( 1.0f, 0.75f, 0.25f, 1.0f ),                 // Text colour
                                                                                         "שלום",                                                                                                // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalTop,                     // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 1, 4, 1, 4 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             8.0f,                                                // Font point size
                                             Dali::TextStyle::REGULAR,                            // Font weight
                                             Vector4( 0.5f, 0.0f, 1.0f, 1.0f ),                   // Text colour
                                             "Hi there",                                          // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalBottom,                  // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 0, 8, 2, 4 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             12.0f,                                               // Font point size
                                             Dali::TextStyle::REGULAR,                            // Font weight
                                             Vector4( 0.5f, 1.0f, 1.0f, 1.0f ),                   // Text colour
                                             "Hola",                                              // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalCenter,                  // Vertical alignment
                                             10.0f,                                               // Padding
                                             90.0f,                                               // Orientation
                                             Toolkit::TableView::CellPosition( 2, 0, 4, 2 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             18.0f,                                               // Font point size
                                             Dali::TextStyle::BOLD,                               // Font weight
                                             Vector4( 0.5f, 1.0f, 0.5f, 1.0f ),                   // Text colour
                                             "Bonjour",                                           // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalTop,                     // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 2, 2, 2, 4 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             12.0f,                                               // Font point size
                                             Dali::TextStyle::REGULAR,                            // Font weight
                                             Vector4( 1.0f, 1.0f, 0.5f, 1.0f ),                   // Text colour
                                             "Ciao",                                              // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalCenter,                  // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 2, 7, 2, 3 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             26.0f,                                               // Font point size
                                             Dali::TextStyle::EXTRABLACK,                         // Font weight
                                             Vector4( 0.5f, 0.0f, 0.0f, 1.0f ),                   // Text colour
                                             "Hello",                                             // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalCenter,                  // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 4, 2, 1, 6 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             8.0f,                                                // Font point size
                                             Dali::TextStyle::DEMIBOLD,                           // Font weight
                                             Vector4( 0.0f, 0.5f, 0.0f, 1.0f ),                   // Text colour
                                             "Top of the morning to you",                         // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalCenter,                  // Vertical alignment
                                             10.0f,                                               // Padding
                                             90.0f,                                               // Orientation
                                             Toolkit::TableView::CellPosition( 4, 10, 8, 2 ) ) ); // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             13.0f,                                               // Font point size
                                             Dali::TextStyle::DEMIBOLD,                           // Font weight
                                             Vector4( 1.0f, 1.0f, 1.0f, 1.0f ),                   // Text colour
                                                                                         "हैलो",                                                                                                  // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalTop,                     // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 6, 1, 1, 3 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             8.0f,                                                // Font point size
                                             Dali::TextStyle::DEMIBOLD,                           // Font weight
                                             Vector4( 1.0f, 1.0f, 0.0f, 1.0f ),                   // Text colour
                                             "สวัสดี",                                              // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalTop,                     // Vertical alignment
                                             10.0f,                                               // Padding
                                             90.0f,                                               // Orientation
                                             Toolkit::TableView::CellPosition( 6, 5, 2, 1 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             18.0f,                                               // Font point size
                                             Dali::TextStyle::REGULAR,                            // Font weight
                                             Vector4( 0.0f, 1.0f, 1.0f, 1.0f ),                   // Text colour
                                             "你好",                                               // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalBottom,                  // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 6, 6, 1, 3 ) ) );  // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             34.0f,                                               // Font point size
                                             Dali::TextStyle::REGULAR,                            // Font weight
                                             Vector4( 0.0f, 0.0f, 1.0f, 1.0f ),                   // Text colour
                                             "G'day",                                             // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalCenter,                  // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 7, 0, 2, 10 ) ) ); // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             16.0f,                                               // Font point size
                                             Dali::TextStyle::EXTRABLACK,                         // Font weight
                                             Vector4( 0.0f, 0.5f, 1.0f, 1.0f ),                   // Text colour
                                             "مرحبا",                                             // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalTop,                     // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 9, 1, 2, 4 ) ) ); // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             10.0f,                                               // Font point size
                                             Dali::TextStyle::EXTRABLACK,                         // Font weight
                                             Vector4( 1.0f, 0.0f, 0.0f, 1.0f ),                   // Text colour
                                                                                          "こんにちは",                                                                                        // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalCenter,                  // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 10, 0, 2, 6 ) ) ); // Table cell position and span

    tableStrings.push_back( new TableString( "HelveticaNue",                                      // Font
                                             "Regular",                                           // Font style
                                             14.0f,                                               // Font point size
                                             Dali::TextStyle::REGULAR,                            // Font weight
                                             Vector4( 0.0f, 1.0f, 0.0f, 1.0f ),                   // Text colour
                                             "aloha",                                             // Text
                                             Toolkit::Alignment::HorizontalCenter,                // Horizontal alignment
                                             Toolkit::Alignment::VerticalTop,                     // Vertical alignment
                                             10.0f,                                               // Padding
                                             0.0f,                                                // Orientation
                                             Toolkit::TableView::CellPosition( 10, 6, 2, 4 ) ) ); // Table cell position and span

    // Add to table
    for( TableStringVector::const_iterator it = tableStrings.begin(), itEnd = tableStrings.end(); it != itEnd; ++it )
    {
      TableString* tableString = *it;

      Dali::TextStyle textStyle;
      textStyle.SetFontName( tableString->fontName );
      textStyle.SetFontStyle( tableString->fontStyle );
      textStyle.SetFontPointSize( Dali::PointSize( DemoHelper::ScalePointSize( tableString->fontSize ) ) );
      textStyle.SetWeight( tableString->fontWeight );
      textStyle.SetTextColor( tableString->fontColour );

      Toolkit::TextView textView = Toolkit::TextView::New( tableString->text );
      textView.SetStyleToCurrentText( textStyle );
      textView.SetRotation( Dali::Degree( tableString->orientation ), Vector3( 0.0f, 0.0f, 1.0f ) );

      Toolkit::Alignment alignmentContainer = Toolkit::Alignment::New( tableString->horizontalAlignment, tableString->verticalAlignment );
      alignmentContainer.SetPadding( Toolkit::Alignment::Padding( tableString->padding, tableString->padding, tableString->padding, tableString->padding ) );
      alignmentContainer.SetScaling( Toolkit::Alignment::ScaleToFill );
      alignmentContainer.Add( textView );

      textContainer.AddChild( alignmentContainer, tableString->cellPosition );

      delete tableString;
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
