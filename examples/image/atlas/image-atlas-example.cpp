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

/**
 * @file image-atlas-example.cpp
 * @brief TODO
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/text-abstraction/text-abstraction.h>

// INTERNAL INCLUDES
#include "basic-text-renderer.h"

using namespace Dali;
using namespace Dali::Toolkit;

/**
 * @brief The main class of the demo.
 */
class ImageAtlasController : public ConnectionTracker
{
public:

  ImageAtlasController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ImageAtlasController::Create );
  }

  ~ImageAtlasController()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();
    fontClient.SetDpi( 96, 96 );

    TextLabel label = TextLabel::New();
    label.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add( label );

    Text::RendererPtr renderer = BasicTextRenderer::New();
    label.SetRenderer( renderer );

    label.SetProperty( TextLabel::PROPERTY_TEXT, "A Quick Brown Fox Jumps Over The Lazy Dog" );

    //Property::Value labelText = label.GetProperty( TextLabel::PROPERTY_TEXT );
    //std::cout << "Got text from label: " << labelText.Get< std::string >() << std::endl;
  }

private:

  Application& mApplication;
};

void RunTest( Application& application )
{
  ImageAtlasController test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
