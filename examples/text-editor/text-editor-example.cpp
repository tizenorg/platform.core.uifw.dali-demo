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

/**
 * @file simple-text-editor-example.cpp
 * @brief Very basic usage of TextEditor control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>
#include <sstream>

// INTERNAL INCLUDES
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const Vector4 BACKGROUND_COLOR( 0.04f, 0.345f, 0.392f, 1.0f );      ///< The background color.
const char*   TOOLBAR_IMAGE = DEMO_IMAGE_DIR "top-bar.png";         ///< The tool-bar image.
const float   TOOLBAR_BUTTON_PERCENTAGE = 0.1f;                     ///< The button's space width as a percentage of the toolbar's width.
const float   TOOLBAR_TITLE_PERCENTAGE = 0.7f;                      ///< The title's width as a percentage of the toolbar's width.
const float   TOOLBAR_HEIGHT_PERCENTAGE = 0.05f;                    ///< The toolbar's height as a percentage of the stage's height.
const float   TOOLBAR_PADDING = 4.f;                                ///< The padding in pixels.
const Vector3 BUTTON_PERCENTAGE( 0.8f, 0.8f, 1.f );                 ///< The button's width as a percentage of the space for the buttons in the toolbar.
const Vector3 TEXT_EDITOR_RELATIVE_SIZE( 1.f, 0.45f, 1.0f );        ///< The size of the text editor as a percentage of the stage's size.
const Vector4 TEXT_EDITOR_BACKGROUND_COLOR( 1.f, 1.f, 1.f, 0.15f ); ///< The background color of the text editor.

const Vector4 COLORS[] = { Color::RED,
                           Color::GREEN,
                           Color::BLUE,
                           Color::YELLOW,
                           Color::CYAN,
                           Color::MAGENTA,
                           Color::WHITE,
                           Color::BLACK };
const unsigned int NUMBER_OF_COLORS = sizeof( COLORS ) / sizeof( Vector4 );

} // Unnamed namespace

/**
 * @brief The main class of the demo.
 */
class TextEditorExample : public ConnectionTracker
{
public:
  TextEditorExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextEditorExample::Create );
  }

  ~TextEditorExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    // Respond to key events
    stage.KeyEventSignal().Connect(this, &TextEditorExample::OnKeyEvent);

    // Set a background color.
    stage.SetBackgroundColor( BACKGROUND_COLOR );

    // The stage size.
    const Vector2 stageSize = stage.GetSize();

    // Creates a default view with a default tool bar.
    // The view is added to the stage.

    // Set the toolbar style
    const float toolBarHeight = TOOLBAR_HEIGHT_PERCENTAGE * stageSize.height;
    const DemoHelper::ViewStyle viewStyle( TOOLBAR_BUTTON_PERCENTAGE,
                                           TOOLBAR_TITLE_PERCENTAGE,
                                           toolBarHeight,
                                           TOOLBAR_PADDING );

    Layer contents = DemoHelper::CreateView( mApplication,
                                             mView,
                                             mToolBar,
                                             "",
                                             TOOLBAR_IMAGE,
                                             "",
                                             viewStyle );

    // Create a 'select color' button.
    mColorButtonOption = Toolkit::PushButton::New();
    mColorButtonOption.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mColorButtonOption.SetSizeModeFactor( BUTTON_PERCENTAGE );

    mColorButtonOption.SetProperty( Button::Property::UNSELECTED_COLOR, Color::BLACK );
    mColorButtonOption.SetProperty( Button::Property::SELECTED_COLOR, Color::BLACK );

    mColorButtonOption.ClickedSignal().Connect( this, &TextEditorExample::OnChangeColorButtonClicked );

    mToolBar.AddControl( mColorButtonOption, viewStyle.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    // Create the text editor.
    mEditor = TextEditor::New();
    mEditor.SetParentOrigin( ParentOrigin::TOP_CENTER );
    mEditor.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    mEditor.SetPosition( 0.f, toolBarHeight, 0.f );
    mEditor.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mEditor.SetSizeModeFactor( TEXT_EDITOR_RELATIVE_SIZE );

    mEditor.SetBackgroundColor( TEXT_EDITOR_BACKGROUND_COLOR );

    const Size boundingBoxSize( stageSize * TEXT_EDITOR_RELATIVE_SIZE.GetVectorXY() );
    Rect<int> boundingBox( 0,
                           static_cast<int>( toolBarHeight ),
                           static_cast<int>( boundingBoxSize.width ),
                           static_cast<int>( boundingBoxSize.height - toolBarHeight ) );

    mEditor.SetProperty( TextEditor::Property::DECORATION_BOUNDING_BOX, boundingBox );
    mEditor.SetProperty( TextEditor::Property::TEXT_COLOR, Color::BLACK );
    mEditor.SetProperty( TextEditor::Property::TEXT,
                         "Lorem ipsum dolor sit amet, aeque definiebas ea mei, posse iracundia ne cum.\n"
                         "Usu ne nisl maiorum iudicabit, veniam epicurei oporteat eos an.\n"
                         "Ne nec nulla regione albucius, mea doctus delenit ad!\n"
                         "Et everti blandit adversarium mei, eam porro neglegentur suscipiantur an.\n"
                         "Quidam corpora at duo. An eos possim scripserit?\n\n"
                         "Aťqui dicant sěnťenťíae aň vel!\n"
                         "Vis viris médiocrem elaboraret ét, verear civibus moderatius ex duo!\n"
                         "Án veri laborě iňtěgré quó, mei aď poššit lobortis, mei prompťa čonsťitůťó eů.\n"
                         "Aliquip sanctůs delicáta quí ěá, et natum aliquam est?\n"
                         "Asšúm sapěret usu ůť.\n"
                         "Síť ut apeirián laboramúš percipitur, sůas hařum ín éos?\n" );

    mEditor.InputStyleChangedSignal().Connect( this, &TextEditorExample::OnTextInputStyleChanged );

    contents.Add( mEditor );

    // Will call the OnTextEditorRelayout() callback after the text editor is relaid-out.
    // This callback addds an iddle callback to the application which may trigger a
    // TextEditor::InputStyleChangedSignal() if the input style changed as a result of
    // moving the cursor position.
    mEditor.OnRelayoutSignal().Connect( this, &TextEditorExample::OnTextEditorRelayout );
  }

  void CreateButtonContainer()
  {
    mButtonContainer = Toolkit::TableView::New( NUMBER_OF_COLORS, 1u );
    mButtonContainer.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mButtonContainer.SetAnchorPoint( AnchorPoint::TOP_LEFT );

    Stage stage = Stage::GetCurrent();
    const Vector2 stageSize = stage.GetSize();
    const float toolBarHeight = TOOLBAR_HEIGHT_PERCENTAGE * stageSize.height;
    mButtonContainer.SetPosition( TOOLBAR_PADDING, 2.f * TOOLBAR_PADDING + toolBarHeight, 0.f );

    mButtonContainer.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );

    const Vector3 containerPercentage( 0.8f * TOOLBAR_BUTTON_PERCENTAGE, NUMBER_OF_COLORS, 1.f );
    mButtonContainer.SetSizeModeFactor( containerPercentage );

    Layer toolbarLayer = mToolBar.GetLayer();
    toolbarLayer.Add( mButtonContainer );

    const Vector3 buttonPercentage( 1.f, 0.8f / static_cast<float>( NUMBER_OF_COLORS ), 1.f );
    for( unsigned int index = 0u; index < NUMBER_OF_COLORS; ++index )
    {
      Toolkit::PushButton button = Toolkit::PushButton::New();
      button.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      button.SetSizeModeFactor( buttonPercentage );

      std::ostringstream s;
      s << "color" << index;
      button.SetName( s.str() );

      button.SetProperty( Button::Property::UNSELECTED_COLOR, COLORS[index] );
      button.SetProperty( Button::Property::SELECTED_COLOR, COLORS[index] );

      button.ClickedSignal().Connect( this, &TextEditorExample::OnColorButtonClicked );

      mButtonContainer.Add( button );
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

  bool OnChangeColorButtonClicked( Button button )
  {
    if( !mButtonContainer )
    {
      CreateButtonContainer();
    }

    mButtonContainer.SetVisible( true );
    mButtonContainer.SetSensitive( true );
    return true;
  }

  bool OnColorButtonClicked( Button button )
  {
    const std::string& name = button.GetName();

    Vector4 color;
    if( "color" == name.substr( 0u, 5u ) )
    {
      const unsigned int index = strtoul( name.substr( 5u, 1u ).c_str(), NULL, 10u );
      color = COLORS[index];
      mEditor.SetProperty( TextEditor::Property::INPUT_COLOR, color );
    }

    mColorButtonOption.SetProperty( Button::Property::UNSELECTED_COLOR, color );
    mColorButtonOption.SetProperty( Button::Property::SELECTED_COLOR, color );

    mButtonContainer.SetVisible( false );
    mButtonContainer.SetSensitive( false );

    return true;
  }

  void OnTextInputStyleChanged( TextEditor editor, TextEditor::InputStyle::Mask mask )
  {
    if( TextEditor::InputStyle::NONE != static_cast<TextEditor::InputStyle::Mask>( mask & TextEditor::InputStyle::COLOR ) )
    {
      const Vector4 color = editor.GetProperty( TextEditor::Property::INPUT_COLOR ).Get<Vector4>();

      mColorButtonOption.SetProperty( Button::Property::UNSELECTED_COLOR, color );
      mColorButtonOption.SetProperty( Button::Property::SELECTED_COLOR, color );
    }

    editor.Reset();
  }

  void OnTextEditorRelayout( Actor actor )
  {
    mApplication.AddIdle( MakeCallback( this, &TextEditorExample::OnApplicationIddle ) );
  }

  void OnApplicationIddle()
  {
    std::cout << "OnApplicationIddle" << std::endl;
    mEditor.ApplicationIddleSignal().Emit();
  }

private:

  Application& mApplication;

  Toolkit::Control    mView;
  Toolkit::ToolBar    mToolBar;
  Toolkit::TextEditor mEditor;
  Toolkit::PushButton mColorButtonOption;
  Toolkit::TableView  mButtonContainer;
};

void RunTest( Application& application )
{
  TextEditorExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  // DALI_DEMO_THEME_PATH not passed to Application so TextEditor example uses default Toolkit style sheet.
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
