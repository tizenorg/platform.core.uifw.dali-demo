#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  //Keeps information about each model for access.
  struct Model
  {
    Actor actor; // Actor housing the mesh renderer of the model.
    Vector2 rotation; // Keeps track of rotation about x and y axis for manual rotation.
    Animation rotationAnimation; // Automatically rotates when left alone.
  };

  //Files for meshes
  const char * const MODEL_FILE[] = {
      DEMO_MODEL_DIR "Dino.obj",
      DEMO_MODEL_DIR "ToyRobot-Metal.obj",
      DEMO_MODEL_DIR "Toyrobot-Plastic.obj"
  };

  const char * const MATERIAL_FILE[] = {
      DEMO_MODEL_DIR "Dino.mtl",
      DEMO_MODEL_DIR "ToyRobot-Metal.mtl",
      DEMO_MODEL_DIR "Toyrobot-Plastic.mtl"
  };

  const char * const TEXTURES_PATH( DEMO_IMAGE_DIR "" );

  //Files for background and toolbar
  const char * const BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-1.jpg");
  const char * const TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );

  const float X_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float Y_ROTATION_DISPLACEMENT_FACTOR = 60.0f;

} //End namespace

class SharedMeshRendererController : public ConnectionTracker
{
public:

  SharedMeshRendererController( Application& application )
  : mApplication( application ),
    mShaderType( "allTextures" ),
    mShaderIndex( 0 ),
    mModelIndex( 1 )
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

    //Attach gesture detector to pan models when rotated.
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( container1 );
    mPanGestureDetector.Attach( container2 );
    mPanGestureDetector.DetectedSignal().Connect( this, &SharedMeshRendererController::OnPan );

    //Create actors to display meshes.
    Control actor1 = Control::New();
    actor1.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    actor1.SetParentOrigin( ParentOrigin::CENTER );
    actor1.SetAnchorPoint( AnchorPoint::CENTER );
    container1.Add( actor1 );

    Control actor2 = Control::New();
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

    //Store model information in corresponding structs.
    mModel1.actor = actor1;
    mModel1.rotation.x = 0.0;
    mModel1.rotation.y = 0.0;
    mModel1.rotationAnimation = rotationAnimation1;

    mModel2.actor = actor2;
    mModel2.rotation.x = 0.0;
    mModel2.rotation.y = 0.0;
    mModel2.rotationAnimation = rotationAnimation2;

    //Calling this sets the model in the two actors.
    ReloadModel();

    //Create button for model changing
    Toolkit::PushButton modelButton = Toolkit::PushButton::New();
    modelButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    modelButton.ClickedSignal().Connect( this, &SharedMeshRendererController::OnChangeModelClicked );
    modelButton.SetParentOrigin( Vector3( 0.1, 0.9, 0.5 ) ); //Offset from bottom left
    modelButton.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    modelButton.SetLabelText( "Change Model" );
    layer.Add( modelButton );

    //Create button for shader changing
    Toolkit::PushButton shaderButton = Toolkit::PushButton::New();
    shaderButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    shaderButton.ClickedSignal().Connect( this, &SharedMeshRendererController::OnChangeShaderClicked );
    shaderButton.SetParentOrigin( Vector3( 0.9, 0.9, 0.5 ) ); //Offset from bottom right
    shaderButton.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    shaderButton.SetLabelText( "Change Shader" );
    layer.Add( shaderButton );
  }

  void ReloadModel()
  {
    //Create mesh property map
    Property::Map map;
    map.Insert( "rendererType", "mesh" );
    map.Insert( "objectUrl", MODEL_FILE[mModelIndex] );
    map.Insert( "materialUrl", MATERIAL_FILE[mModelIndex] );
    map.Insert( "texturesPath", TEXTURES_PATH );
    map.Insert( "shaderType", mShaderType );

    //Set the two actors to use the mesh
    mModel1.actor.SetProperty( Control::Property::BACKGROUND, Property::Value( map ) );
    mModel2.actor.SetProperty( Control::Property::BACKGROUND, Property::Value( map ) );
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    switch( gesture.state )
    {
      case Gesture::Started:
      {
        //Find out which model has been selected
        int modelIndex;
        actor.GetProperty( actor.GetPropertyIndex( "Tag" ) ).Get( modelIndex );
        mSelectedModel = ( modelIndex == 1 ) ? mModel1: mModel2;

        //Pause current animation, as the gesture will be used to manually rotate the model
        mSelectedModel.rotationAnimation.Pause();

        break;
      }
      case Gesture::Continuing:
      {
        //Rotate based off the gesture.
        mSelectedModel.rotation.x -= gesture.displacement.y / X_ROTATION_DISPLACEMENT_FACTOR; // Y displacement rotates around X axis
        mSelectedModel.rotation.y += gesture.displacement.x / Y_ROTATION_DISPLACEMENT_FACTOR; // X displacement rotates around Y axis
        Quaternion rotation = Quaternion( Radian( mSelectedModel.rotation.x ), Vector3::XAXIS) *
                              Quaternion( Radian( mSelectedModel.rotation.y ), Vector3::YAXIS);

        mSelectedModel.actor.SetOrientation( rotation );

        break;
      }
      case Gesture::Finished:
      {
        //Return to automatic animation
        mSelectedModel.rotationAnimation.Play();

        break;
      }
      case Gesture::Cancelled:
      {
        //Return to automatic animation
        mSelectedModel.rotationAnimation.Play();

        break;
      }
      default:
      {
        break;
      }
    }
  }

  bool OnChangeModelClicked( Toolkit::Button button )
  {
    mModelIndex++;
    mModelIndex %= 3;

    ReloadModel();

    return true;
  }

  bool OnChangeShaderClicked( Toolkit::Button button )
  {
    mShaderIndex++;
    mShaderIndex %= 3;

    if( mShaderIndex == 0 )
    {
      mShaderType = "allTextures";
    }
    else if( mShaderIndex == 1 )
    {
      mShaderType = "diffuseTexture";
    }
    else if( mShaderIndex == 2 )
    {
      mShaderType = "textureless";
    }

    ReloadModel();

    return true;
  }

private:
  Application&  mApplication;

  //The models displayed on screen, including information about rotation.
  Model mModel1;
  Model mModel2;

  //The model currently being interacted with through gestures.
  Model mSelectedModel;

  //Used to detect panning to rotate the selected model.
  PanGestureDetector mPanGestureDetector;

  std::string mShaderType;
  int mShaderIndex;
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
