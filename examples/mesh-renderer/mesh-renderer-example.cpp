#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/object/property-map.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  //Keeps information about each model for access.
  struct Model
  {
    Control control; // Control housing the mesh renderer of the model.
    Vector2 rotation; // Keeps track of rotation about x and y axis for manual rotation.
    Animation rotationAnimation; // Automatically rotates when left alone.
  };

  //Files for meshes
  const char * const MODEL_FILE[] =
  {
      DEMO_MODEL_DIR "Dino.obj",
      DEMO_MODEL_DIR "ToyRobot-Metal.obj",
      DEMO_MODEL_DIR "Toyrobot-Plastic.obj"
  };

  const char * const MATERIAL_FILE[] =
  {
      DEMO_MODEL_DIR "Dino.mtl",
      DEMO_MODEL_DIR "ToyRobot-Metal.mtl",
      DEMO_MODEL_DIR "Toyrobot-Plastic.mtl"
  };

  const char * const TEXTURES_PATH( DEMO_IMAGE_DIR "" );

  //Possible shader options.
  const char * const SHADER_TYPE[] =
  {
    "ALL_TEXTURES",
    "DIFFUSE_TEXTURE",
    "TEXTURELESS"
  };

  //Files for background and toolbar
  const char * const BACKGROUND_IMAGE( DEMO_IMAGE_DIR "background-1.jpg");

  const float X_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float Y_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float MODEL_SCALE = 0.75f;
  const int NUM_MESHES = 3;

  //Used to identify what is being panned.
  const int PAN_MODEL = 0;
  const int PAN_LIGHT = 1;

} //End namespace

class MeshRendererController : public ConnectionTracker
{
public:

  MeshRendererController( Application& application )
  : mApplication( application ),   //Store handle to the application.
    mModelIndex( 1 ),              //Start with metal robot.
    mShaderIndex( 0 ),             //Start with all textures.
    mSelectedModelIndex( 0 )       //Non-valid default, which will get set to a correct value when used.
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &MeshRendererController::Create );
  }

  ~MeshRendererController()
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

    //Setup and load the 3D models and buttons
    LoadScene();

    //Allow for exiting of the application via key presses.
    stage.KeyEventSignal().Connect( this, &MeshRendererController::OnKeyEvent );
  }

  //Sets up the on-screen elements.
  void LoadScene()
  {
    Stage stage = Stage::GetCurrent();

    //Set up 3D layer to place objects on.
    Layer layer = Layer::New();
    layer.SetParentOrigin( ParentOrigin::CENTER );
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    layer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    layer.SetBehavior( Layer::LAYER_2D ); //We use a 2D layer as this is closer to UI work than full 3D scene creation.
    layer.SetDepthTestDisabled( false ); //Enable depth testing, as otherwise the 2D layer would not do so.
    stage.Add( layer );

    //Create gesture detector for panning of models.
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect( this, &MeshRendererController::OnPan );

    //Add containers to house each renderer-holding-actor.
    for( int i = 0; i < NUM_MESHES; i++ )
    {
      mContainers[i] = Actor::New();
      mContainers[i].SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mContainers[i].RegisterProperty( "Tag", Property::Value( i ) ); //Used to identify the actor and index into the model.
      mContainers[i].RegisterProperty( "Pan", Property::Value( PAN_MODEL ) ); //Used to differentiate between pan-able objects.

      //Position each container on screen
      if( i == 0 )
      {
        //Main, central model
        mContainers[i].SetSizeModeFactor( Vector3( MODEL_SCALE, MODEL_SCALE, 0.0f ) );
        mContainers[i].SetParentOrigin( ParentOrigin::CENTER );
        mContainers[i].SetAnchorPoint( AnchorPoint::CENTER );
      }
      else if( i == 1 )
      {
        //Top left model
        mContainers[i].SetSizeModeFactor( Vector3( MODEL_SCALE / 3.0f, MODEL_SCALE / 3.0f, 0.0f ) );
        mContainers[i].SetParentOrigin( Vector3( 0.05, 0.03, 0.5 ) ); //Offset from top left
        mContainers[i].SetAnchorPoint( AnchorPoint::TOP_LEFT );
      }
      else if( i == 2 )
      {
        //Top right model
        mContainers[i].SetSizeModeFactor( Vector3( MODEL_SCALE / 3.0f, MODEL_SCALE / 3.0f, 0.0f ) );
        mContainers[i].SetParentOrigin( Vector3( 0.95, 0.03, 0.5 ) ); //Offset from top right
        mContainers[i].SetAnchorPoint( AnchorPoint::TOP_RIGHT );
      }

      mPanGestureDetector.Attach( mContainers[i] );
      layer.Add( mContainers[i] );
    }

    //Set up models
    for( int i = 0; i < NUM_MESHES; i++ )
    {
      //Create control to display model
      Control control = Control::New();
      control.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      control.SetParentOrigin( ParentOrigin::CENTER );
      control.SetAnchorPoint( AnchorPoint::CENTER );
      mContainers[i].Add( control );

      //Make model spin to demonstrate 3D
      Animation rotationAnimation = Animation::New( 15.0f );
      float spin = i % 2 == 0 ? 1.0f : -1.0f; //Make actors spin in different directions to better show independence.
      rotationAnimation.AnimateBy( Property( control, Actor::Property::ORIENTATION ),
                                   Quaternion( Degree( 0.0f ), Degree( spin * 360.0f ), Degree( 0.0f ) ) );
      rotationAnimation.SetLooping( true );
      rotationAnimation.Play();

      //Store model information in corresponding structs.
      mModels[i].control = control;
      mModels[i].rotation.x = 0.0f;
      mModels[i].rotation.y = 0.0f;
      mModels[i].rotationAnimation = rotationAnimation;
    }

    //Calling this sets the model in the controls.
    ReloadModel();

    //Create button for model changing
    Toolkit::PushButton modelButton = Toolkit::PushButton::New();
    modelButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    modelButton.ClickedSignal().Connect( this, &MeshRendererController::OnChangeModelClicked );
    modelButton.SetParentOrigin( Vector3( 0.05, 0.95, 0.5 ) ); //Offset from bottom left
    modelButton.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    modelButton.SetLabelText( "Change Model" );
    layer.Add( modelButton );

    //Create button for shader changing
    Toolkit::PushButton shaderButton = Toolkit::PushButton::New();
    shaderButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    shaderButton.ClickedSignal().Connect( this, &MeshRendererController::OnChangeShaderClicked );
    shaderButton.SetParentOrigin( Vector3( 0.95, 0.95, 0.5 ) ); //Offset from bottom right
    shaderButton.SetAnchorPoint( AnchorPoint::BOTTOM_RIGHT );
    shaderButton.SetLabelText( "Change Shader" );
    layer.Add( shaderButton );

    //Create button for pausing animations
    Toolkit::PushButton pauseButton = Toolkit::PushButton::New();
    pauseButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    pauseButton.ClickedSignal().Connect( this, &MeshRendererController::OnPauseClicked );
    pauseButton.SetParentOrigin( Vector3( 0.5, 0.95, 0.5 ) ); //Offset from bottom center
    pauseButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    pauseButton.SetLabelText( " || " );
    layer.Add( pauseButton );

    //Create control to act as light source of scene.
    mLightSource = Control::New();
    mLightSource.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::WIDTH );
    mLightSource.SetResizePolicy( ResizePolicy::DIMENSION_DEPENDENCY, Dimension::HEIGHT );

    //Set position relative to top left, as the light source property is also relative to the top left.
    mLightSource.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mLightSource.SetAnchorPoint( AnchorPoint::CENTER );
    //mLightSource.SetPositionInheritanceMode( DONT_INHERIT_POSITION );
    mLightSource.SetPosition( Stage::GetCurrent().GetSize().x * 0.5f, Stage::GetCurrent().GetSize().y * 0.1f );

    Property::Map lightMap;
    lightMap.Insert( "rendererType", "color" );
    lightMap.Insert( "mixColor", Color::WHITE );
    mLightSource.SetProperty( Control::Property::BACKGROUND, Property::Value( lightMap ) );

    TextLabel lightLabel = TextLabel::New( "Light" );
    lightLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    lightLabel.SetParentOrigin( ParentOrigin::CENTER );
    lightLabel.SetAnchorPoint( AnchorPoint::CENTER );
    float padding = 5.0f;
    lightLabel.SetPadding( Padding( padding, padding, padding, padding ) );
    mLightSource.Add( lightLabel );

    mLightSource.RegisterProperty( "Pan", Property::Value( PAN_LIGHT ) ); //Used to differentiate between pan-able objects.
    mPanGestureDetector.Attach( mLightSource );

    layer.Add( mLightSource );

    //Calling this sets the light position of each model to that of the light source control.
    UpdateLight();
  }

  //Updates the displayed models to account for parameter changes.
  void ReloadModel()
  {
    //Create mesh property map
    Property::Map map;
    map.Insert( "rendererType", "MESH" );
    map.Insert( "objectUrl", MODEL_FILE[mModelIndex] );
    map.Insert( "materialUrl", MATERIAL_FILE[mModelIndex] );
    map.Insert( "texturesPath", TEXTURES_PATH );
    map.Insert( "shaderType", SHADER_TYPE[mShaderIndex] );

    //Set the two controls to use the mesh
    for( int i = 0; i < NUM_MESHES; i++ )
    {
      mModels[i].control.SetProperty( Control::Property::BACKGROUND, Property::Value( map ) );
    }
  }

  //Updates the light position for each model to account for changes in the source on screen.
  void UpdateLight()
  {
    //Set light position to the x and y of the light control, offset out of the screen.
    Vector3 controlPosition = mLightSource.GetCurrentPosition();
    Vector3 lightPosition = Vector3( controlPosition.x, controlPosition.y, Stage::GetCurrent().GetSize().x * 2 );

    for( int i = 0; i < NUM_MESHES; ++i )
    {
      mModels[i].control.RegisterProperty( "uLightPosition", lightPosition, Property::ANIMATABLE );
    }
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    //Determine what is being panned.
    int panned;
    actor.GetProperty( actor.GetPropertyIndex( "Pan" ) ).Get( panned );

    if( panned == PAN_MODEL )
    {
      //Rotate the model.
      switch( gesture.state )
      {
        case Gesture::Started:
        {
          //Find out which model has been selected
          actor.GetProperty( actor.GetPropertyIndex( "Tag" ) ).Get( mSelectedModelIndex );

          //Pause current animation, as the gesture will be used to manually rotate the model
          mModels[mSelectedModelIndex].rotationAnimation.Pause();

          break;
        }
        case Gesture::Continuing:
        {
          //Rotate based off the gesture.
          mModels[mSelectedModelIndex].rotation.x -= gesture.displacement.y / X_ROTATION_DISPLACEMENT_FACTOR; // Y displacement rotates around X axis
          mModels[mSelectedModelIndex].rotation.y += gesture.displacement.x / Y_ROTATION_DISPLACEMENT_FACTOR; // X displacement rotates around Y axis
          Quaternion rotation = Quaternion( Radian( mModels[mSelectedModelIndex].rotation.x ), Vector3::XAXIS) *
                                Quaternion( Radian( mModels[mSelectedModelIndex].rotation.y ), Vector3::YAXIS);

          mModels[mSelectedModelIndex].control.SetOrientation( rotation );

          break;
        }
        case Gesture::Finished:
        {
          //Return to automatic animation
          if( !mPaused )
          {
            mModels[mSelectedModelIndex].rotationAnimation.Play();
          }

          break;
        }
        case Gesture::Cancelled:
        {
          //Return to automatic animation
          if( !mPaused )
          {
            mModels[mSelectedModelIndex].rotationAnimation.Play();
          }

          break;
        }
        default:
        {
          //We can ignore other gestures and gesture states.
          break;
        }
      }
    }
    else if( panned == PAN_LIGHT )
    {
      float currentX = actor.GetCurrentPosition().x;
      float currentY = actor.GetCurrentPosition().y;
      actor.SetPosition( currentX + 2 * gesture.displacement.x, currentY + 2 * gesture.displacement.y );
      UpdateLight();
    }
  }

  //Cycle through the list of models.
  bool OnChangeModelClicked( Toolkit::Button button )
  {
    ++mModelIndex %= 3;

    ReloadModel();

    return true;
  }

  //Cycle through the list of shaders.
  bool OnChangeShaderClicked( Toolkit::Button button )
  {
    ++mShaderIndex %= 3;

    ReloadModel();

    return true;
  }

  //Pause all animations, and keep them paused even after user panning.
  //This button is a toggle, so pressing again will start the animations again.
  bool OnPauseClicked( Toolkit::Button button )
  {
    //If animations are currently running, pause them and keep them paused.
    if( !mPaused )
    {
      mPaused = true;

      for( int i = 0; i < NUM_MESHES ; ++i )
      {
        mModels[i].rotationAnimation.Pause();
      }

      button.SetLabelText( " > ");
    }
    else //Unpause all animations again.
    {
      mPaused = false;

      for( int i = 0; i < NUM_MESHES ; ++i )
      {
        mModels[i].rotationAnimation.Play();
      }

      button.SetLabelText( " || ");
    }

    return true;
  }

  //If escape or the back button is pressed, quit the application (and return to the launcher)
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;

  //The models displayed on screen, including information about rotation.
  Model mModels[NUM_MESHES];
  Actor mContainers[NUM_MESHES];

  //Acts as a global light source.
  Control mLightSource;

  //Used to detect panning to rotate the selected model.
  PanGestureDetector mPanGestureDetector;

  int mModelIndex; //Index of model to load.
  int mShaderIndex; //Index of shader type to use.
  int mSelectedModelIndex; //Index of model selected on screen.
  bool mPaused; //If true, all animations are paused and should stay so.
};

void RunTest( Application& application )
{
  MeshRendererController test( application );

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
