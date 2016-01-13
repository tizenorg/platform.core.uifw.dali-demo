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
#include <dali-toolkit/devel-api/controls/text-controls/text-editor.h>
#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;


/**
 * @brief The main class of the demo.
 */
class SimpleTextEditorExample : public ConnectionTracker
{
public:

  SimpleTextEditorExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SimpleTextEditorExample::Create );
  }

  ~SimpleTextEditorExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Vector4( 0.04f, 0.345f, 0.392f, 1.0f ) );

    TextEditor editor = TextEditor::New();
    editor.SetParentOrigin( ParentOrigin::CENTER );
    editor.SetSize( 300.f, 400.f );
    editor.SetBackgroundColor( Color::WHITE );
    editor.SetBackgroundColor( Vector4( 1.f, 1.f, 1.f, 0.15f ) );

    editor.SetProperty( TextEditor::Property::TEXT_COLOR, Color::BLACK );

    stage.Add( editor );
  }

private:

  Application& mApplication;
};

void RunTest( Application& application )
{
  SimpleTextEditorExample test( application );

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
