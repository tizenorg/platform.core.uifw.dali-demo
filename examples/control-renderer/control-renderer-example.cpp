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

#include <dali-toolkit/dali-toolkit.h>
#include <stdlib.h>

#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>

using namespace Dali;

namespace
{
const char * const TEST_IMAGE(DALI_IMAGE_DIR "gallery-large-1.jpg");
}

// This example shows how to request and use controlRenderer
//
class RendererController : public ConnectionTracker
{
public:

  RendererController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &RendererController::Create );
  }

  ~RendererController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    Toolkit::RendererFactory rendererFactory = Toolkit::RendererFactory::Get();
    Vector2 size(300.f, 200.f);

  //---- Color renderer--------

    Actor actor1 = Actor::New();
    actor1.SetSize(size);
    actor1.SetParentOrigin( ParentOrigin::CENTER );
    actor1.SetAnchorPoint( AnchorPoint::CENTER );
    stage.Add( actor1 );

    Property::Map propertyMap1;
    propertyMap1.Insert("renderer-type", "color-renderer");
    propertyMap1.Insert("blend-color", Color::CYAN );

    Toolkit::ControlRenderer colorRenderer = rendererFactory.GetControlRenderer(propertyMap1);
    colorRenderer.SetSize( size );
    colorRenderer.SetOnStage( actor1 );

// ---- Gradient renderer: linear

    Actor actor2 = Actor::New();
    actor2.SetSize(size);
    actor2.SetParentOrigin( ParentOrigin::TOP_CENTER );
    actor2.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    actor1.Add( actor2 );

    Property::Map propertyMap2;
    propertyMap2.Insert("renderer-type", "gradient-renderer");
    propertyMap2.Insert("gradient-start-position", Vector2(-150.f, -100.f));
    propertyMap2.Insert("gradient-end-position", Vector2(150.f, 100.f));
    propertyMap2.Insert("gradient-units", "user-space");

    Property::Array stopOffsets;
    stopOffsets.PushBack( 0.1f );
    stopOffsets.PushBack( 0.7f );
    stopOffsets.PushBack( 0.4f );
    stopOffsets.PushBack( 0.9f );
    propertyMap2.Insert("gradient-stop-offset", stopOffsets);
    Property::Array stopColors;
    stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
    stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
    stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
    stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 0.f )/255.f );
    propertyMap2.Insert("gradient-stop-color", stopColors);

    Toolkit::ControlRenderer linearGradientRenderer = rendererFactory.GetControlRenderer(propertyMap2);
    linearGradientRenderer.SetSize( size );
    linearGradientRenderer.SetOnStage( actor2 );

// ---- Gradient renderer: radial
/*
    Actor actor3 = Actor::New();
    actor3.SetSize(size);
    actor3.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    actor3.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    actor1.Add( actor3 );

    Property::Map propertyMap3;
    propertyMap3.Insert("renderer-type", "gradient-renderer");
    propertyMap3.Insert("gradient-center", Vector2(0.0f, 0.0f));
    propertyMap3.Insert("gradient-radius", 1.4f);

    propertyMap3.Insert("gradient-stop-offset", stopOffsets);
    propertyMap3.Insert("gradient-stop-color", stopColors);
    propertyMap3.Insert("gradient-spread-method", "repeat");


    Toolkit::ControlRenderer radialGradientRenderer = rendererFactory.GetControlRenderer(propertyMap3);
    radialGradientRenderer.SetSize( size );
    radialGradientRenderer.SetOnStage( actor3 );
*/
    // ---- Image renderer

      Actor actor3 = Actor::New();
      actor3.SetSize(size);
      actor3.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      actor3.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      actor1.Add( actor3 );

      Property::Map propertyMap3;
      propertyMap3.Insert("renderer-type", "image-renderer");
      propertyMap3.Insert("image-url", TEST_IMAGE);


      Toolkit::ControlRenderer imageRenderer = rendererFactory.GetControlRenderer(propertyMap3);
      imageRenderer.SetSize( size );
      imageRenderer.SetOnStage( actor3 );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &RendererController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  RendererController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
