#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/rendering/renderer.h>
#include <dali/public-api/object/property-map.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/control-renderer.h>
#include <dali-toolkit/devel-api/controls/renderer-factory/renderer-factory.h>
#include <dali-toolkit/devel-api/controls/slider/slider.h>

#include <iostream> //TODO: TMP

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  //Button icon image urls
  const char* SPHERE_IMAGE( DEMO_IMAGE_DIR "sphere.png" );
  const char* CONE_IMAGE( DEMO_IMAGE_DIR "cone.png" );
  const char* CONICAL_FRUSTRUM_IMAGE( DEMO_IMAGE_DIR "conical-frustrum.png" );
  const char* CYLINDER_IMAGE( DEMO_IMAGE_DIR "cylinder.png" );
  const char* CUBE_IMAGE( DEMO_IMAGE_DIR "cube.png" );
  const char* BEVELLED_CUBE_IMAGE( DEMO_IMAGE_DIR "bevelled-cube-low.png" );
  const char* OCTAHEDRON_IMAGE( DEMO_IMAGE_DIR "octahedron.png" );

  //Shape property defaults
  const int DEFAULT_SLICES = 32;
  const int DEFAULT_STACKS = 32;
  const float DEFAULT_SCALE_HEIGHT = 16.0f;
  const float DEFAULT_SCALE_BOTTOM_RADIUS = 8.0f;
  const float DEFAULT_SCALE_TOP_RADIUS = 4.0f;
  const float DEFAULT_SCALE_RADIUS = 8.0f;
  const float DEFAULT_BEVEL_PERCENTAGE = 0.3f;
  const float DEFAULT_BEVEL_SMOOTHNESS = 0.0f;

  const int NUM_MODELS = 7; //Total number of possible base shapes.
  const int MAX_PROPERTIES = 3; //Maximum number of properties a shape may require. (For displaying sliders.)
  const float X_ROTATION_DISPLACEMENT_FACTOR = 60.0f;
  const float Y_ROTATION_DISPLACEMENT_FACTOR = 60.0f;

} //End namespace

class PrimitiveShapesController : public ConnectionTracker
{
public:

  PrimitiveShapesController( Application& application )
  : mApplication( application ),
    mRotation( Vector2( 0.0f, 0.0f ) ),
    mColor( Vector4( 0.0f, 0.0f, 0.7f, 1.0f ) )
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
    //stage.GetRootLayer().TouchedSignal().Connect( this, &PrimitiveShapesController::OnTouch );
  }

  void LoadScene()
  {
    Stage stage = Stage::GetCurrent();

    //Set up 3D layer to place shape on.
    Layer layer = Layer::New();
    layer.SetParentOrigin( ParentOrigin::CENTER );
    layer.SetAnchorPoint( AnchorPoint::CENTER );
    layer.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    layer.SetBehavior( Layer::LAYER_3D );
    stage.Add( layer );

    //Set up model selection buttons.
    CreateButtons( layer );

    //Set up sliders to adjust model properties.
    for( int i = 0; i < MAX_PROPERTIES; i++ )
    {
      float offset = ( MAX_PROPERTIES - i - 1 ) * 0.1f; //Offset for the slider to position it on screen.

      Slider slider = Slider::New();
      slider.SetParentOrigin( Vector3( 0.5, 0.95 - offset, 0.5 ) ); //Offset from bottom center.
      slider.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      slider.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      slider.SetSizeModeFactor( Vector3( 0.95, 0.0, 0.0 ) );
      slider.ValueChangedSignal().Connect( this, &PrimitiveShapesController::SliderValueChanged );
      layer.Add( slider );

      mSliders.push_back( slider );
    }

    //Create a container to house the renderer-holding actor, to provide a constant hitbox.
    Actor container = Actor::New();
    container.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    container.SetSizeModeFactor( Vector3( 0.9, 0.30, 0.0 ) );
    container.SetParentOrigin( ParentOrigin::CENTER );
    container.SetAnchorPoint( AnchorPoint::CENTER );
    layer.Add( container );

    //Create actor to house the primitive shape renderer.
    mModelActor = Actor::New();
    mModelActor.SetParentOrigin( ParentOrigin::CENTER );
    mModelActor.SetAnchorPoint( AnchorPoint::CENTER);
    mModelActor.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    container.Add( mModelActor );

    //Load default shape.
    LoadCube();

    //Make actor spin to demonstrate 3D.
    mRotationAnimation = Animation::New(15.f);
    mRotationAnimation.AnimateBy( Property( mModelActor, Actor::Property::ORIENTATION ),
                                 Quaternion( Degree( 0.0f ), Degree( 360.0f ), Degree( 0.0f ) ) );
    mRotationAnimation.SetLooping(true);
    mRotationAnimation.Play();

    //Attach gesture detector to pan models when rotated.
    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.Attach( container );
    mPanGestureDetector.DetectedSignal().Connect( this, &PrimitiveShapesController::OnPan );
  }

  void ChangeModel()
  {
    if( mShapeRenderer )
    {
      mShapeRenderer.SetOffStage( mModelActor );
    }

    mShapeRenderer = Toolkit::RendererFactory::Get().GetControlRenderer( mRendererMap );
    mShapeRenderer.SetOnStage( mModelActor );
  }

  void LoadCube()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType" ] = "primitive";
    mRendererMap[ "color"        ] = mColor;
    mRendererMap[ "shape"        ] = "cube";

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      //We don't use any sliders for properties.
      mSliders.at( i ).SetVisible( false );
    }

    ChangeModel();
  }

  void LoadOctahedron()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType" ] = "primitive";
    mRendererMap[ "color"        ] = mColor;
    mRendererMap[ "shape"        ] = "octahedron";

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      //We don't use any sliders for properties.
      mSliders.at( i ).SetVisible( false );
    }

    ChangeModel();
  }

  void LoadSphere()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType" ] = "primitive";
    mRendererMap[ "color"        ] = mColor;
    mRendererMap[ "shape"        ] = "sphere";
    mRendererMap[ "slices"       ] = DEFAULT_SLICES;
    mRendererMap[ "stacks"       ] = DEFAULT_STACKS;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        //Set up slider for slices property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "slices" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 3.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SLICES ) );
        mSliders.at( i ).SetVisible( true );
      }
      else if( i == 1 )
      {
        //Set up slider for stacks property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "stacks" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 2.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_STACKS ) );
        mSliders.at( i ).SetVisible( true );
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  void LoadCone()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType"      ] = "primitive";
    mRendererMap[ "color"             ] = mColor;
    mRendererMap[ "shape"             ] = "cone";
    mRendererMap[ "scaleHeight"       ] = DEFAULT_SCALE_HEIGHT;
    mRendererMap[ "scaleBottomRadius" ] = DEFAULT_SCALE_BOTTOM_RADIUS;
    mRendererMap[ "slices"            ] = DEFAULT_SLICES;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        //Set up slider for scaleHeight property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "scaleHeight" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 1.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SCALE_HEIGHT ) );
        mSliders.at( i ).SetVisible( true );
      }
      else if( i == 1 )
      {
        //Set up slider for scaleBottomRadius property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "scaleBottomRadius" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 1.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SCALE_BOTTOM_RADIUS ) );
        mSliders.at( i ).SetVisible( true );
      }
      else if( i == 2 )
      {
        //Set up slider for slices property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "slices" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 3.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SLICES ) );
        mSliders.at( i ).SetVisible( true );
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  void LoadConicalFrustrum()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType"      ] = "primitive";
    mRendererMap[ "color"             ] = mColor;
    mRendererMap[ "shape"             ] = "conicalFrustrum";
    mRendererMap[ "scaleTopRadius"    ] = DEFAULT_SCALE_TOP_RADIUS;
    mRendererMap[ "scaleBottomRadius" ] = DEFAULT_SCALE_BOTTOM_RADIUS;
    mRendererMap[ "scaleHeight"       ] = DEFAULT_SCALE_HEIGHT;
    mRendererMap[ "slices"            ] = DEFAULT_SLICES;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        //Set up slider for scaleHeight property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "scaleHeight" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 1.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SCALE_HEIGHT ) );
        mSliders.at( i ).SetVisible( true );
      }
      else if( i == 1 )
      {
        //Set up slider for scaleBottomRadius property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "scaleBottomRadius" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 0.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SCALE_BOTTOM_RADIUS ) );
        mSliders.at( i ).SetVisible( true );
      }
      else if( i == 2 )
      {
        //Set up slider for scaleTopRadius property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "scaleTopRadius" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 0.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SCALE_TOP_RADIUS ) );
        mSliders.at( i ).SetVisible( true );
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  void LoadCylinder()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType" ] = "primitive";
    mRendererMap[ "color"        ] = mColor;
    mRendererMap[ "shape"        ] = "cylinder";
    mRendererMap[ "scaleHeight"  ] = DEFAULT_SCALE_HEIGHT;
    mRendererMap[ "scaleRadius"  ] = DEFAULT_SCALE_RADIUS;
    mRendererMap[ "slices"       ] = DEFAULT_SLICES;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        //Set up slider for scaleHeight property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "scaleHeight" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 1.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SCALE_HEIGHT ) );
        mSliders.at( i ).SetVisible( true );
      }
      else if( i == 1 )
      {
        //Set up slider for scaleRadius property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "scaleRadius" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 1.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SCALE_RADIUS ) );
        mSliders.at( i ).SetVisible( true );
      }
      else if( i == 2 )
      {
        //Set up slider for slices property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "slices" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 3.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 32.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_SLICES ) );
        mSliders.at( i ).SetVisible( true );
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  void LoadBevelledCube()
  {
    //Set up renderer properties.
    mRendererMap.Clear();
    mRendererMap[ "rendererType"    ] = "primitive";
    mRendererMap[ "color"           ] = mColor;
    mRendererMap[ "shape"           ] = "bevelledCube";
    mRendererMap[ "bevelPercentage" ] = DEFAULT_BEVEL_PERCENTAGE;
    mRendererMap[ "bevelSmoothness" ] = DEFAULT_BEVEL_SMOOTHNESS;

    //Set up sliders.
    for( unsigned i = 0; i < mSliders.size(); i++ )
    {
      if( i == 0 )
      {
        //Set up slider for bevelPercentage property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "bevelPercentage" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 0.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 1.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_BEVEL_PERCENTAGE ) );
        mSliders.at( i ).SetVisible( true );
      }
      else if( i == 1 )
      {
        //Set up slider for bevelSmoothness property.
        mSliders.at( i ).RegisterProperty( "rendererProperty", Property::Value( "bevelSmoothness" ), Property::READ_WRITE );
        mSliders.at( i ).SetProperty( Slider::Property::LOWER_BOUND, Property::Value( 0.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::UPPER_BOUND, Property::Value( 1.0f ) );
        mSliders.at( i ).SetProperty( Slider::Property::VALUE, Property::Value( DEFAULT_BEVEL_SMOOTHNESS ) );
        mSliders.at( i ).SetVisible( true );
      }
      else
      {
        //We don't use any more sliders for properties.
        mSliders.at( i ).SetVisible( false );
      }
    }

    ChangeModel();
  }

  void CreateButtons( Layer layer )
  {
    int numRows = 2;
    int numColumns = 4;
    int modelNumber = 0;

    TableView buttonGrid = TableView::New( numRows, numColumns );
    buttonGrid.SetParentOrigin( ParentOrigin::TOP_CENTER );
    buttonGrid.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    buttonGrid.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    buttonGrid.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::HEIGHT );
    buttonGrid.SetSizeModeFactor( Vector3( 0.0, 0.3, 0.0 ) );
    layer.Add( buttonGrid );

    for( int row = 0; row < numRows; row++ )
    {
      for( int column = 0; column < numColumns; column++, modelNumber++ )
      {
        Actor button = CreateButton( modelNumber );
        buttonGrid.AddChild( button, TableView::CellPosition( row, column ) );

        if( modelNumber == NUM_MODELS - 1)
        {
          break;
        }
      }
    }
  }

  Actor CreateButton( int modelNumber )
  {
    float padding = 2.0;

    Actor button = Actor::New();
    button.SetParentOrigin( ParentOrigin::CENTER );
    button.SetAnchorPoint( AnchorPoint::CENTER );
    button.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    button.SetPadding( Padding( padding, padding, padding, padding ) );
    button.RegisterProperty( "modelNumber", Property::Value( modelNumber ) );

    //Set button icon.
    ImageView icon = ImageView::New( SelectIconUrl( modelNumber) );
    icon.SetParentOrigin( ParentOrigin::CENTER );
    icon.SetAnchorPoint( AnchorPoint::CENTER );
    icon.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    icon.SetSizeScalePolicy( SizeScalePolicy::FILL_WITH_ASPECT_RATIO );
    button.Add( icon );

    Property::Map map;
    map[ "rendererType" ] = "border";
    map[ "borderColor"  ] = Color::BLUE;
    map[ "borderSize"   ] = 5.0f;

    ControlRenderer borderRenderer = Toolkit::RendererFactory::Get().GetControlRenderer( map );
    borderRenderer.SetOnStage( button );

    //button.ConnectSignal()
    button.TouchSignal().Connect( this, &PrimitiveShapesController::OnTouch );

    return button;
  }

  std::string SelectIconUrl( int modelNumber )
  {
    switch( modelNumber )
    {
      case 0: return SPHERE_IMAGE;
      case 1: return CONE_IMAGE;
      case 2: return CONICAL_FRUSTRUM_IMAGE;
      case 3: return CYLINDER_IMAGE;
      case 4: return CUBE_IMAGE;
      case 5: return BEVELLED_CUBE_IMAGE;
      case 6: return OCTAHEDRON_IMAGE;
      default: return "";
    }
  }

  //TODO: TMP
  bool OnTouch( Actor actor, const TouchData& touch )
  {
    PointState::Type pointState = touch.GetState( 0 );

    if( pointState == PointState::UP )
    {
      int modelNumber;
      actor.GetProperty( actor.GetPropertyIndex( "modelNumber" ) ).Get( modelNumber );

      switch( modelNumber )
      {
        case 0:
        {
          LoadSphere();
          break;
        }
        case 1:
        {
          LoadCone();
          break;
        }
        case 2:
        {
          LoadConicalFrustrum();
          break;
        }
        case 3:
        {
          LoadCylinder();
          break;
        }
        case 4:
        {
          LoadCube();
          break;
        }
        case 5:
        {
          LoadBevelledCube();
          break;
        }
        case 6:
        {
          LoadOctahedron();
          break;
        }
        default:
        {
          break;
        }
      }
    }

    return true;
  }

  bool SliderValueChanged( Slider slider, float value )
  {
    std::string rendererPropertyLabel;
    slider.GetProperty( slider.GetPropertyIndex( "rendererProperty" ) ).Get( rendererPropertyLabel );
    mRendererMap[ rendererPropertyLabel ] = value;

    ChangeModel();

    return true;
  }

  void OnPan( Actor actor, const PanGesture& gesture )
  {
    switch( gesture.state )
    {
      case Gesture::Started:
      {
        //Pause animation, as the gesture will be used to manually rotate the model
        mRotationAnimation.Pause();

        break;
      }
      case Gesture::Continuing:
      {
        //Rotate based off the gesture.
        mRotation.x -= gesture.displacement.y / X_ROTATION_DISPLACEMENT_FACTOR; // Y displacement rotates around X axis
        mRotation.y += gesture.displacement.x / Y_ROTATION_DISPLACEMENT_FACTOR; // X displacement rotates around Y axis
        Quaternion rotation = Quaternion( Radian( mRotation.x ), Vector3::XAXIS) *
                              Quaternion( Radian( mRotation.y ), Vector3::YAXIS);

        mModelActor.SetOrientation( rotation );

        break;
      }
      case Gesture::Finished:
      {
        //Return to automatic animation
        mRotationAnimation.Play();

        break;
      }
      case Gesture::Cancelled:
      {
        //Return to automatic animation
        mRotationAnimation.Play();

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

  //Holds the sliders that each shape needs to access.
  std::vector<Slider> mSliders;

  //For creating the 3D shape.
  Property::Map mRendererMap;
  ControlRenderer mShapeRenderer;
  Actor mModelActor;

  //Used to manually rotate the model through panning gestures.
  PanGestureDetector mPanGestureDetector;
  Animation mRotationAnimation;
  Vector2 mRotation;

  //Color to set all shapes.
  Vector4 mColor;
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
