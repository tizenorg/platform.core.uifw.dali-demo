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

// CLASS HEADER
#include "dali-app.h"

// EXTERNAL INCLUDES
#include <fstream>


using namespace Dali;

#define TOKEN_STRING(x) #x

namespace
{
std::string GetFileContents(const std::string &fn)
{
  std::ifstream t(fn.c_str());
  return std::string((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
};

}

DaliApplication::DaliApplication( Dali::Application application, std::string layoutFileName, std::string scriptFileName )
: mApplication( application ),
  mLayoutFileName(layoutFileName),
  mScriptFileName(scriptFileName)
{
  mApplication.InitSignal().Connect( this, &DaliApplication::Create );
}

DaliApplication::~DaliApplication()
{
}

void DaliApplication::Create( Dali::Application& application )
{
  TextActor textActor = TextActor::New( "JSON & JavaScript Launcher..." );

  // Reposition the actor
  textActor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  textActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  textActor.SetPosition( 20, 0 );

  // Get a handle to the stage
  Stage stage = Stage::GetCurrent();

  // Display the actor on the stage
  stage.Add( textActor );

  // change the background color to purple
  Stage::GetCurrent().SetBackgroundColor( Vector4(0.2,0.2,0.4,1.0) );

  // Try loading a JSON file
  if( !mLayoutFileName.empty() )
  {
    mBuilder = Toolkit::Builder::New();

    Property::Map defaultDirs;
    defaultDirs[ TOKEN_STRING(DALI_IMAGE_DIR) ]  = DALI_IMAGE_DIR;
    defaultDirs[ TOKEN_STRING(DALI_MODEL_DIR) ]  = DALI_MODEL_DIR;
    defaultDirs[ TOKEN_STRING(DALI_SCRIPT_DIR) ] = DALI_SCRIPT_DIR;
    mBuilder.AddConstants( defaultDirs );

    std::string json_data(GetFileContents( mLayoutFileName ));
    mBuilder.LoadFromString(json_data);
    mBuilder.AddActors( stage.GetRootLayer() );
  }

  // Try load a JavaScript file
  if( !mScriptFileName.empty() )
  {
    // execute the script
    mScript = Toolkit::Script::New();

    mScript.ExecuteFile( mScriptFileName);
  }


}

void DaliApplication::MainLoop()
{
  mApplication.MainLoop();
}
