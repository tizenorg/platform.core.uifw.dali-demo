#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{
  const char * const RENDERER_TYPE_NAME( "rendererType" );
} //End namespace

class PrimitiveShapesController : public ConnectionTracker
{
public:

  PrimitiveShapesController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &PrimitiveShapesController::Create );
  }

  ~PrimitiveShapesController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    LoadScene();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &PrimitiveShapesController::OnTouch );
  }

  //Creates a layer to hold the shapes, and then loads the shapes into the four quadrants.
  void LoadScene()
  {
    Stage stage = Stage::GetCurrent();

    //Set object dimensions to fit on stage.
    int x = stage.GetSize().width * 0.35;
    int y = stage.GetSize().height * 0.35;

    //Set up 3D layer to place objects on.
    Layer layer = Layer::New();
    layer.SetParentOrigin( ParentOrigin::CENTER );
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    layer.SetBehavior( Layer::LAYER_3D );
    stage.Add( layer );

    LoadSphere( layer, x, y );
    LoadConicSection( layer, x, y );
    LoadCone( layer, x, y );
    LoadBevelledCube( layer, x, y );
  }

  void LoadSphere( Layer layer, int x, int y )
  {
    Toolkit::RendererFactory rendererFactory = Toolkit::RendererFactory::Get();
    Stage stage = Stage::GetCurrent();

    //Set shape properties
    Property::Map map;
    map.Insert( RENDERER_TYPE_NAME, "primitive" );
    map.Insert( "color", Vector4( 0.0, 1.0, 0.3, 1.0 ) );
    map.Insert( "shape", "sphere" );

    Toolkit::ControlRenderer sphereRenderer = rendererFactory.GetControlRenderer( map );

    //Create actor to house renderer.
    Actor actor = Actor::New();
    sphereRenderer.SetOnStage( actor );
    actor.SetSize(x, y);
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetPosition( stage.GetSize().width * -0.25, stage.GetSize().height * -0.25 );
    layer.Add( actor );

    //Make actor spin to demonstrate 3D.
    Animation rotationAnimation = Animation::New(15.f);
    rotationAnimation.AnimateBy(Property(actor, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    rotationAnimation.SetLooping(true);
    rotationAnimation.Play();
  }

  void LoadConicSection( Layer layer, int x, int y )
  {
    Toolkit::RendererFactory rendererFactory = Toolkit::RendererFactory::Get();
    Stage stage = Stage::GetCurrent();

    //Set shape properties
    Property::Map map;
    map.Insert( RENDERER_TYPE_NAME, "primitive" );
    map.Insert( "color", Vector4( 0.0, 0.2, 1.0, 1.0 ) );
    map.Insert( "shape", "conicalFrustrum" );
    map.Insert( "scaleTopRadius", x / 4 );
    map.Insert( "scaleBottomRadius", x / 2 );
    map.Insert( "scaleHeight", y );

    Toolkit::ControlRenderer conicSectionRenderer = rendererFactory.GetControlRenderer( map );

    //Create actor to house renderer.
    Actor actor = Actor::New();
    conicSectionRenderer.SetOnStage( actor );
    actor.SetSize(x, y);
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetPosition( stage.GetSize().width * 0.25, stage.GetSize().height * -0.25 );
    layer.Add( actor );

    //Make actor spin to demonstrate 3D.
    Animation rotationAnimation = Animation::New(15.f);
    rotationAnimation.AnimateBy(Property(actor, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    rotationAnimation.SetLooping(true);
    rotationAnimation.Play();
  }

  void LoadCone( Layer layer, int x, int y )
  {
    Toolkit::RendererFactory rendererFactory = Toolkit::RendererFactory::Get();
    Stage stage = Stage::GetCurrent();

    //Set shape properties
    Property::Map map;
    map.Insert( RENDERER_TYPE_NAME, "primitive" );
    map.Insert( "color", Vector4( 1.0, 0.2, 0.0, 1.0 ) );
    map.Insert( "shape", "cone" );
    map.Insert( "scaleBottomRadius", x / 2 );
    map.Insert( "scaleHeight", y );

    Toolkit::ControlRenderer coneRenderer = rendererFactory.GetControlRenderer( map );

    //Create actor to house renderer.
    Actor actor = Actor::New();
    coneRenderer.SetOnStage( actor );
    actor.SetSize(x, y);
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetPosition( stage.GetSize().width * -0.25, stage.GetSize().height * 0.25 );
    layer.Add( actor );

    //Make actor spin to demonstrate 3D.
    Animation rotationAnimation = Animation::New(15.f);
    rotationAnimation.AnimateBy(Property(actor, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    rotationAnimation.SetLooping(true);
    rotationAnimation.Play();
  }

  void LoadBevelledCube( Layer layer, int x, int y )
  {
    Toolkit::RendererFactory rendererFactory = Toolkit::RendererFactory::Get();
    Stage stage = Stage::GetCurrent();

    //Set shape properties
    Property::Map map;
    map.Insert( RENDERER_TYPE_NAME, "primitive" );
    map.Insert( "color", Vector4( 1.0, 0.0, 1.0, 1.0 ) );
    map.Insert( "shape", "bevelledCube" );
    map.Insert( "bevelPercentage", 0.3f );

    Toolkit::ControlRenderer cubeRenderer = rendererFactory.GetControlRenderer( map );

    //Create actor to house renderer.
    Actor actor = Actor::New();
    cubeRenderer.SetOnStage( actor );
    actor.SetSize(x, y);
    actor.SetParentOrigin( ParentOrigin::CENTER );
    actor.SetAnchorPoint( AnchorPoint::CENTER );
    actor.SetPosition( stage.GetSize().width * 0.25, stage.GetSize().height * 0.25 );
    layer.Add( actor );

    //Make actor spin to demonstrate 3D.
    Animation rotationAnimation = Animation::New(15.f);
    rotationAnimation.AnimateBy(Property(actor, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    rotationAnimation.SetLooping(true);
    rotationAnimation.Play();
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
  Vector3 mSceneCenter;
  Vector3 mSceneSize;
};

void RunTest( Application& application )
{
  PrimitiveShapesController test( application );

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
