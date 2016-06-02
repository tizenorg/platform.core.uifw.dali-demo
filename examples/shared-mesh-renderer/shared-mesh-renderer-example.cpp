#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include "shared/view.h"

#include <iostream> //TODO: TMP

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  //Keeps information about each model for access.
  struct Model
  {
    Actor actor; // Actor housing the mesh renderer of the model.
    Radian rotationX; // Keeps track of rotation about x axis for manual rotation.
    Radian rotationY; // Keeps track of rotation about y axis for manual rotation.
    Animation rotationAnimation; // Automatically rotates when left alone.
  };

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
  const float X_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float Y_ROTATION_DISPLACEMENT_FACTOR = 60.0f;

} //End namespace

class SharedMeshRendererController : public ConnectionTracker
{
public:

  SharedMeshRendererController( Application& application )
  : mApplication( application ),
    mModelIndex( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SharedMeshRendererController::Create );
  }

  ~SharedMeshRendererController()
  {
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    //Add background
    ImageView backView = ImageView::New( BACKGROUND_IMAGE );
    backView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    stage.Add( backView );

    //Add description of demo
    TextLabel details = TextLabel::New( DEMO_DETAILS );
    details.SetParentOrigin( Vector3(0.5, 0.025, 0.5) ); //Set at top, offset by a small amount of padding.
    details.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    details.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    details.SetProperty( TextLabel::Property::POINT_SIZE, 18.0f );
    details.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    stage.Add( details );

    //Load the 3D models
    LoadScene();
  }

  //Creates two actors to house a single mesh renderer, and places them on a 3D layer.
  void LoadScene()
  {
    Stage stage = Stage::GetCurrent();

    //Set up 3D layer to place objects on.
    Layer layer = Layer::New();
    layer.SetParentOrigin( ParentOrigin::CENTER );
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    layer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    layer.SetBehavior( Layer::LAYER_3D );
    stage.Add( layer );

    //Containers to house each renderer-holding-actor, to provide a constant hitbox.
    Actor container1 = Actor::New();
    container1.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    container1.SetSizeModeFactor( Vector3( 0.45f, 0.45f, 0.0f ) );
    container1.SetParentOrigin( ParentOrigin::CENTER );
    container1.SetAnchorPoint( AnchorPoint::CENTER );
    container1.SetPosition( stage.GetSize().width * 0.25, 0.0 );
    container1.RegisterProperty( "Tag", Property::Value( 1 ) ); // Used to identify this actor.
    layer.Add( container1 );

    Actor container2 = Actor::New();
    container2.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    container2.SetSizeModeFactor( Vector3( 0.225f, 0.225f, 0.0f ) );
    container2.SetParentOrigin( ParentOrigin::CENTER );
    container2.SetAnchorPoint( AnchorPoint::CENTER );
    container2.SetPosition( stage.GetSize().width * -0.25, 0.0 );
    container2.RegisterProperty( "Tag", Property::Value( 2 ) ); // Used to identify this actor.
    layer.Add( container2 );

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
    actor1.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    actor1.SetParentOrigin( ParentOrigin::CENTER );
    actor1.SetAnchorPoint( AnchorPoint::CENTER );
    container1.Add( actor1 );

    //Create second actor to also house mesh renderer.
    Actor actor2 = Actor::New();
    meshRenderer.SetOnStage( actor2 );
    actor2.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    actor2.SetParentOrigin( ParentOrigin::CENTER );
    actor2.SetAnchorPoint( AnchorPoint::CENTER );
    container2.Add( actor2 );

    //Make actors spin to demonstrate 3D.
    Animation rotationAnimation1 = Animation::New( 15.f );
    rotationAnimation1.AnimateBy( Property( actor1, Actor::Property::ORIENTATION ),
                                  Quaternion( Degree( 0.f ), Degree( 360.f ), Degree( 0.f ) ) );
    rotationAnimation1.SetLooping( true );
    rotationAnimation1.Play();

    Animation rotationAnimation2 = Animation::New( 15.f );
    rotationAnimation2.AnimateBy( Property( actor2, Actor::Property::ORIENTATION ),
                                  Quaternion( Degree( 0.f ), Degree( -360.f ), Degree( 0.f ) ) );
    rotationAnimation2.SetLooping( true );
    rotationAnimation2.Play();

    //Attach gesture detector to pan models when rotated.
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( container1 );
    mPanGestureDetector.Attach( container2 );
    mPanGestureDetector.DetectedSignal().Connect( this, &SharedMeshRendererController::OnPan );

    //Store model information in corresponding structs.
    mModel1.actor = actor1;
    mModel1.rotationX = 0.0;
    mModel1.rotationY = 0.0;
    mModel1.rotationAnimation = rotationAnimation1;

    mModel2.actor = actor2;
    mModel2.rotationX = 0.0;
    mModel2.rotationY = 0.0;
    mModel2.rotationAnimation = rotationAnimation2;
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    //If the gesture has just begun, figure out which model corresponds to the selected actor
    if( mModelIndex == 0)
    {
      actor.GetProperty( actor.GetPropertyIndex( "Tag" ) ).Get( mModelIndex );
    }

    Model& selectedModel = ( mModelIndex == 1) ? mModel1: mModel2;

    switch (gesture.state)
    {
      case Gesture::Continuing:
      {
        //Pause current animation and rotate based off the panning gesture.
        selectedModel.rotationAnimation.Pause();

        selectedModel.rotationX = selectedModel.rotationX - gesture.displacement.y / X_ROTATION_DISPLACEMENT_FACTOR; // Y displacement rotates around X axis
        selectedModel.rotationY = selectedModel.rotationY + gesture.displacement.x / Y_ROTATION_DISPLACEMENT_FACTOR; // X displacement rotates around Y axis
        Quaternion rotation = Quaternion( selectedModel.rotationX, Vector3::XAXIS) *
                              Quaternion( selectedModel.rotationY, Vector3::YAXIS);

        selectedModel.actor.SetOrientation( rotation );

        break;
      }
      case Gesture::Finished:
      {
        //Return to automatic animation
        selectedModel.rotationAnimation.Play();

        //Unselect the current model
        mModelIndex = 0;

        break;
      }
      default:
      {
        break;
      }
    }
  }

private:
  Application&  mApplication;

  //The models displayed on screen, including information about rotation.
  Model mModel1;
  Model mModel2;

  //Used to detect panning to rotate the selected model.
  PanGestureDetector mPanGestureDetector;

  //Identifies which model is being interacted with. A value of zero indicates neither.
  int mModelIndex;
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
