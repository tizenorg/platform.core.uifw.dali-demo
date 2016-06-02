#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>

using namespace Dali;

namespace
{
  const char * const RENDERER_TYPE_NAME( "rendererType" );

  const char * const MODEL_FILE( DEMO_MODEL_DIR "ToyRobot-Metal.obj" );

  const char * const MATERIAL_FILE( DEMO_MODEL_DIR "ToyRobot-Metal.mtl" );

  const char * const TEXTURES_PATH( DEMO_IMAGE_DIR "" );

} //End namespace

class SharedMeshRendererController : public ConnectionTracker
{
public:

  SharedMeshRendererController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SharedMeshRendererController::Create );
  }

  ~SharedMeshRendererController()
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
    stage.GetRootLayer().TouchedSignal().Connect( this, &SharedMeshRendererController::OnTouch );
  }

  //Creates a layer to hold the shapes, and then loads the shapes into the four quadrants.
  void LoadScene()
  {
    Stage stage = Stage::GetCurrent();

    //Set object dimensions to fit on stage.
    int x = stage.GetSize().width * 0.45;
    int y = stage.GetSize().height * 0.45;

    //Set up 3D layer to place objects on.
    Layer layer = Layer::New();
    layer.SetParentOrigin( ParentOrigin::CENTER );
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    layer.SetBehavior( Layer::LAYER_3D );
    stage.Add( layer );

    Toolkit::RendererFactory rendererFactory = Toolkit::RendererFactory::Get();

    Property::Map map;
    map.Insert( "rendererType", "mesh" );
    map.Insert( "objectUrl", MODEL_FILE );
    map.Insert( "materialUrl", MATERIAL_FILE );
    map.Insert( "texturesPath", TEXTURES_PATH );
    Toolkit::ControlRenderer meshRenderer = rendererFactory.GetControlRenderer( map );

    //Create first actor to house mesh renderer.
    Actor actor1 = Actor::New();
    meshRenderer.SetOnStage( actor1 );
    actor1.SetSize(x, y);
    actor1.SetParentOrigin( ParentOrigin::CENTER );
    actor1.SetAnchorPoint( AnchorPoint::CENTER );
    actor1.SetPosition( stage.GetSize().width * 0.25, 0.0 );
    layer.Add( actor1 );

    //Create second actor to also house mesh renderer.
    Actor actor2 = Actor::New();
    meshRenderer.SetOnStage( actor2 );
    actor2.SetSize(x / 2, y / 2);
    actor2.SetParentOrigin( ParentOrigin::CENTER );
    actor2.SetAnchorPoint( AnchorPoint::CENTER );
    actor2.SetPosition( stage.GetSize().width * -0.25, 0.0 );
    layer.Add( actor2 );

    //Make actors spin to demonstrate 3D.
    Animation rotationAnimation1 = Animation::New(15.f);
    rotationAnimation1.AnimateBy(Property(actor1, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(360.f), Degree(0.f)) );
    rotationAnimation1.SetLooping(true);
    rotationAnimation1.Play();

    Animation rotationAnimation2 = Animation::New(15.f);
    rotationAnimation2.AnimateBy(Property(actor2, Actor::Property::ORIENTATION), Quaternion(Degree(0.f), Degree(-360.f), Degree(0.f)) );
    rotationAnimation2.SetLooping(true);
    rotationAnimation2.Play();
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
  SharedMeshRendererController test( application );

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
