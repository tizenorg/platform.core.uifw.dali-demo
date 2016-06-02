#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  //Files for robot mesh
  const char * const MODEL_FILE( DEMO_MODEL_DIR "ToyRobot-Metal.obj" );
  const char * const MATERIAL_FILE( DEMO_MODEL_DIR "ToyRobot-Metal.mtl" );
  const char * const TEXTURES_PATH( DEMO_IMAGE_DIR "" );

  //Files for background and toolbar
  const char * const BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-gradient.jpg");
  const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );

  const char * const APPLICATION_TITLE( "Shared Mesh Renderer" );
  const char * const DEMO_DETAILS( "(this is two actors sharing one mesh renderer)" );

  const float DETAILS_OFFSET = 18.0f;

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

    //Add background
    Image imageBackground = ResourceImage::New( BACKGROUND_IMAGE );
    ImageView backView = ImageView::New( imageBackground );
    backView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( backView );

    //Add description of demo
    TextLabel details = TextLabel::New( DEMO_DETAILS );
    details.SetParentOrigin( ParentOrigin::TOP_CENTER );
    details.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    details.SetPosition(0.0, DETAILS_OFFSET ); //Add gap between description and top of screen.
    details.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    details.SetProperty( TextLabel::Property::POINT_SIZE, 18.0f );
    details.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    stage.Add( details );

    //Load the 3D models
    LoadScene();

    // Respond to a click anywhere on the stage by quitting the application
    stage.GetRootLayer().TouchedSignal().Connect( this, &SharedMeshRendererController::OnTouch );
  }

  //Creates two actors to house a single mesh renderer, and places them on a 3D layer.
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

    //Create mesh renderer
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
