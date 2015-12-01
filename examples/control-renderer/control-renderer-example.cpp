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
const char * const TEST_N_PATCH_IMAGE(DALI_IMAGE_DIR "heartsframe.9.png");
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
    Vector2 size(150.f, 150.f);

  //---- Color renderer--------

    Actor actor1 = Actor::New();
    actor1.SetSize(size);
    actor1.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    actor1.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    stage.Add( actor1 );

    Property::Map propertyMap1;
    propertyMap1.Insert("rendererType", "colorRenderer");
    propertyMap1.Insert("blendColor", Color::CYAN );

    Toolkit::ControlRenderer colorRenderer = rendererFactory.GetControlRenderer(propertyMap1);
    colorRenderer.SetOnStage( actor1 );

    // border renderer
    Actor actor2 = Actor::New();
    actor2.SetSize(size);
    actor2.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
    actor2.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    actor1.Add( actor2 );
    actor1.SetX(20.f);

    Toolkit::ControlRenderer borderRenderer = rendererFactory.GetControlRenderer(10.f, Color::MAGENTA);
    borderRenderer.SetOnStage( actor2 );

// ---- Gradient renderer: linear

    Actor actor3 = Actor::New();
    actor3.SetSize(size);
    actor3.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
    actor3.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    actor2.Add( actor3 );

    Property::Map propertyMap2;
    propertyMap2.Insert("rendererType", "gradientRenderer");
    propertyMap2.Insert("gradientStartPosition", Vector2(-150.f, -100.f));
    propertyMap2.Insert("gradientEndPosition", Vector2(150.f, 100.f));
    propertyMap2.Insert("gradientUnits", "userSpace");

    Property::Array stopOffsets;
    stopOffsets.PushBack( 0.0f );
    stopOffsets.PushBack( 0.4f );
    stopOffsets.PushBack( 0.7f );
    stopOffsets.PushBack( 1.0f );
    propertyMap2.Insert("gradientStopOffset", stopOffsets);
    Property::Array stopColors;
    stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
    stopColors.PushBack( Vector4( 214.f, 37.f, 139.f, 191.f )/255.f );
    stopColors.PushBack( Vector4( 196.f, 198.f, 71.f, 122.f )/255.f );
    stopColors.PushBack( Vector4( 129.f, 198.f, 193.f, 255.f )/255.f );
    propertyMap2.Insert("gradientStopColor", stopColors);

    Toolkit::ControlRenderer linearGradientRenderer = rendererFactory.GetControlRenderer(propertyMap2);
    linearGradientRenderer.SetOnStage( actor3 );

// ---- Gradient renderer: radial

    Actor actor4 = Actor::New();
    actor4.SetSize(size);
    actor4.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
    actor4.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    actor3.Add( actor4 );

    Property::Map propertyMap3;
    propertyMap3.Insert("rendererType", "gradientRenderer");
    propertyMap3.Insert("gradientCenter", Vector2(0.5f, 0.5f));
    propertyMap3.Insert("gradientRadius", 1.f);

    propertyMap3.Insert("gradientStopOffset", stopOffsets);
    propertyMap3.Insert("gradientStopColor", stopColors);
    propertyMap3.Insert("gradientSpreadMethod", "repeat");


    Toolkit::ControlRenderer radialGradientRenderer = rendererFactory.GetControlRenderer(propertyMap3);
    radialGradientRenderer.SetOnStage( actor4 );

    // ---- Image renderer

      Actor actor5 = Actor::New();
      actor5.SetSize(size);
      actor5.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      actor5.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      actor1.Add( actor5 );

      Property::Map propertyMap4;
      propertyMap4.Insert("rendererType", "borderRenderer");
      propertyMap4.Insert("borderColor", Color::YELLOW );
      propertyMap4.Insert("borderSize", 4.f );

      Toolkit::ControlRenderer imageRenderer = rendererFactory.GetControlRenderer(TEST_IMAGE);
      imageRenderer.SetOnStage( actor5 );

      Toolkit::ControlRenderer borderRenderer2 = rendererFactory.GetControlRenderer(propertyMap4);
      borderRenderer2.SetDepthIndex(1.f);
      borderRenderer2.SetOnStage( actor5 );

    //  --- n patch renderer

      Actor actor6 = Actor::New();
      actor6.SetSize(size);
      actor6.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
      actor6.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      actor5.Add( actor6 );

      Toolkit::ControlRenderer nPatchRenderer = rendererFactory.GetControlRenderer(TEST_N_PATCH_IMAGE);
      nPatchRenderer.SetOnStage( actor6 );

      Actor actor7 = Actor::New();
      actor7.SetSize(Vector2(size.x*2, size.y));
      actor7.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
      actor7.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
      actor6.Add( actor7 );

      nPatchRenderer.SetOnStage( actor7 );


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
