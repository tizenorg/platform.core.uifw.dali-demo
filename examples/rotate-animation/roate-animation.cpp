#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/debug.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

using namespace Dali;
using namespace Dali::Toolkit;

const double PI = 3.14159265358979;

// This example shows how to create and display Hello World using a simple TextLabel
//
class RotateAnimationSample : public ConnectionTracker
{
public:

  RotateAnimationSample( Application& application )
  : mApplication( application ),
    mStageWidth(0.0f),
    mStageHeight(0.0f),
    randomSeed(0)
  {
    // Connect to the Application's init signal
    mApplication.InitSignal().Connect( this, &RotateAnimationSample::Create );
  }

  ~RotateAnimationSample()
  {
    // Nothing to do here
  }

  // The init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );
    mStageWidth = stage.GetSize().width;
    mStageHeight = stage.GetSize().height;
    mImageDirectory = DEMO_IMAGE_DIR;

    Control wheel = MakeWheelOfFortune(20);
    wheel.SetParentOrigin(ParentOrigin::CENTER);
    wheel.SetAnchorPoint(AnchorPoint::CENTER);
    wheel.SetPosition(0,0);

    Animation spinAnimation = Animation::New( 4.0f );
    spinAnimation.AnimateBy( Property( wheel, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree (8100.0f)), Vector3(0,0,1)), AlphaFunction::EASE_OUT_BACK );
    spinAnimation.Play();

    // Connect to touch & key event signals
    stage.KeyEventSignal().Connect( this, &RotateAnimationSample::OnKeyEvent );
  }


  Control MakeWheelOfFortune( int sliceNumber )
  {
    Control wheelLayer = Control::New();
    wheelLayer.SetSize( mStageWidth, mStageHeight );
    wheelLayer.SetPosition( mStageWidth / 2.0f, mStageHeight/ 2.0f );

    float angle = 360.0f / ( float )sliceNumber;

    for( int idx = 0 ; idx < sliceNumber ; idx++ )
    {
      float rotateAngle = ( idx*angle );
      ImageView slice = MakeSlice( idx+1, angle );
      slice.RotateBy( Degree( rotateAngle ), Vector3(0,0,1) );
      wheelLayer.Add( slice );
    }

    stage.Add( wheelLayer );

    return wheelLayer;
  }

  ImageView MakeSlice( int idx, float angle )
  {
    ImageView imgObj = ImageView::New( mImageDirectory + "triangle.png");
    imgObj.SetSize( 1000.0f*( float )sin( angle*PI/360 ), 500.0f*( float )cos( angle*PI/360 ) );
    imgObj.SetParentOrigin( ParentOrigin::CENTER );
    imgObj.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    imgObj.SetProperty( ImageView::Property::IMAGE, MakeShaderEffect() );
    imgObj.RegisterProperty( "trianglColor", MakeRandomNumber() );

    char str[1000];
    sprintf(str, "%d", idx);

    TextLabel textObj = TextLabel::New( str );
    textObj.SetSize( 1000.0f*( float )sin( angle*PI/360 ), 500.0f*( float )cos( angle*PI/360 ) );
    textObj.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textObj.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    textObj.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    textObj.SetProperty( TextLabel::Property::POINT_SIZE, 8.0f );

    imgObj.Add( textObj );
    return imgObj;
  }

  Vector3 MakeRandomNumber()
  {
    Vector3 randColor;

    srand( randomSeed+=100 );
    randColor.x =( ( float )( rand()%1001 ) ) / 1000.0f;
    randColor.y =( ( float )( rand()%1001 ) ) / 1000.0f;
    randColor.z =( ( float )( rand()%1001 ) ) / 1000.0f;

    return randColor;
  }

  Property::Value MakeShaderEffect()
  {
    std::string vertexShaderSource =
      "attribute mediump vec2 aPosition;\n"
      "uniform mediump mat4 uMvpMatrix;\n"
      "uniform mediump vec3 uSize;\n"
      "varying mediump vec2 vTexCoord;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  mediump vec4 position = vec4( aPosition, 0.0, 1.0 );\n"
      "  position.xyz *= uSize;\n"
      "  gl_Position = uMvpMatrix * position;\n"
      "  vTexCoord = aPosition + vec2( 0.5 );\n"
      "}\n";
    std::string fragmentShaderSource =
      "varying mediump vec2 vTexCoord;\n"
      "uniform lowp vec4 uColor;\n"
      "uniform sampler2D sTexture;\n"
      "uniform lowp vec3 trianglColor;\n"
      "\n"
      "void main()\n"
      "{\n"
      "  lowp vec4 color = texture2D( sTexture, vTexCoord ) * uColor;\n"
      "  if( color.a != 0.0)\n"
      "  {\n"
      "    gl_FragColor = vec4( trianglColor, color.a);\n"
      "  }\n"
      "}\n";

    Property::Map map;

    Property::Map customShader;
    customShader[ "vertexShader" ] = vertexShaderSource;
    customShader[ "fragmentShader" ] = fragmentShaderSource;

    map[ "shader" ] = customShader;

    return map;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, DALI_KEY_ESCAPE ) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }


private:
  Application&  mApplication;
  Stage stage;
  float mStageWidth;
  float mStageHeight;
  int randomSeed;
  std::string mImageDirectory;
};

// Entry point for Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  RotateAnimationSample test( application );
  application.MainLoop();
  return 0;
}

