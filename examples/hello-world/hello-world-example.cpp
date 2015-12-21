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
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali/integration-api/debug.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "comptex1.astc";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "comptex1.ktx";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "tx-etc1.ktx";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "tx-astc-4x4-linear.ktx";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "tx-astc-4x4-srgb.ktx";
//const char* IMAGE_FILENAME = DALI_IMAGE_DIR "comptex1-wrapped-astc-nm.ktx";

const char* IMAGE_FILENAME_ETC         = DALI_IMAGE_DIR "tx-etc1.ktx";
const char* IMAGE_FILENAME_ASTC_LINEAR = DALI_IMAGE_DIR "tx-astc-4x4-linear.ktx";
const char* IMAGE_FILENAME_ASTC_SRGB   = DALI_IMAGE_DIR "tx-astc-4x4-srgb.ktx";


//const char* IMAGE_FILENAME_TEST   = DALI_IMAGE_DIR "com.samsung.dali-demo.ico";
const char* IMAGE_FILENAME_TEST    = DALI_IMAGE_DIR "pointer.ico";

const char* IMAGE_FILENAME_ASTC_NATIVE   = DALI_IMAGE_DIR "people-medium-1.astc";

#if 1
const char* IMAGE_FILENAME_TEST1   = DALI_IMAGE_DIR "frac-32bpp-32x32.ico";
const char* IMAGE_FILENAME_TEST2   = DALI_IMAGE_DIR "frac-24bpp-32x32.ico";
const char* IMAGE_FILENAME_TEST3   = DALI_IMAGE_DIR "frac-8bpp-32x32.ico";
const char* IMAGE_FILENAME_TEST4   = DALI_IMAGE_DIR "frac-4bpp-32x32.ico";
const char* IMAGE_FILENAME_TEST5   = DALI_IMAGE_DIR "frac-1bpp-32x32.ico";

const char* IMAGE_FILENAME_TEST6   = DALI_IMAGE_DIR "pointer2.ico";
#endif






#if 0

class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }
  ImageActor CreateTestSolidColorActor()
  {
    ImageActor itemActor =  ImageActor::New();
    Vector4 color;
    Dali::Toolkit::Control control = Dali::Toolkit::Control::New();
    control.SetBackgroundColor(Vector4((float)(rand()%10)/10, (float)(rand()%10)/10, (float)(rand()%10)/10, 0.5f));
    control.SetParentOrigin( ParentOrigin::CENTER );
  //  control.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    Constraint constraint = Constraint::New<Vector3>( control, Actor::Property::SIZE, EqualToConstraint() );
    constraint.AddSource( ParentSource(Actor::Property::SIZE) );
    constraint.Apply();

    itemActor.Add ( control );
    itemActor.SetParentOrigin( ParentOrigin::CENTER );
    itemActor.SetAnchorPoint( AnchorPoint::CENTER );
    //itemActor.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    return itemActor;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    Layer m_MainLayer = Layer::New();
    m_MainLayer.SetParentOrigin(ParentOrigin::CENTER);
    m_MainLayer.SetAnchorPoint(AnchorPoint::CENTER);
    m_MainLayer.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    //m_MainLayer.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
    //m_MainLayer.SetSize( 200, 200 );
    Stage::GetCurrent().Add(m_MainLayer);

    Dali::Toolkit::ScrollView m_ScrollView = Dali::Toolkit::ScrollView::New();
    m_ScrollView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    m_ScrollView.SetParentOrigin( ParentOrigin::TOP_LEFT );
    //m_ScrollView.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    m_ScrollView.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
    m_ScrollView.SetSize( 200, 200 );
    m_ScrollView.SetPosition(0.0f, 0.0f, 0.0f);
    m_ScrollView.SetWrapMode(false);


    //Dali::Toolkit::FixedRuler* m_FixedRuler = new Dali::Toolkit::FixedRuler( stageSize.width);
    //m_FixedRuler->SetDomain(Dali::Toolkit::RulerDomain(0.0f, stageSize.height*1, false));

    Dali::Toolkit::RulerPtr defaultRuler = new Dali::Toolkit::DefaultRuler();
    Dali::Toolkit::RulerPtr disabledRuler = new Dali::Toolkit::DefaultRuler();
    disabledRuler->Disable();


    //m_ScrollView.SetRulerX(m_FixedRuler );
    //m_ScrollView.SetRulerY(disabledRuler);

    m_ScrollView.SetRulerY(defaultRuler );
    m_ScrollView.SetRulerX(disabledRuler);

    m_ScrollView.SetWrapMode(false);
    //m_ScrollView.SetOvershootEnabled(true);


    Actor mScrollViewContainer = Actor::New();
    mScrollViewContainer.SetName("ScrollViewContainer");
    mScrollViewContainer.SetParentOrigin(ParentOrigin::TOP_CENTER);
    mScrollViewContainer.SetAnchorPoint(AnchorPoint::TOP_CENTER);
    mScrollViewContainer.SetSize(Vector2( stageSize.width * 1.0f, stageSize.height ) );

#if 0
    ImageActor test1 = CreateTestSolidColorActor();
    test1.SetSize(stageSize.width,stageSize.height );
    test1.SetPosition(0.0f,0.0f,0.0f);
    mScrollViewContainer.Add( test1 );
    ImageActor test2 = CreateTestSolidColorActor();
    test2.SetSize( stageSize.width,stageSize.height );
    test2.SetPosition(stageSize.width,0.0f,0.0f);
    mScrollViewContainer.Add( test2);
    ImageActor test3 = CreateTestSolidColorActor();
    test3.SetSize( stageSize.width,stageSize.height );
    test3.SetPosition(stageSize.width*2,0.0f,0.0f);
    mScrollViewContainer.Add( test3);
#else
    ImageActor test1 = ImageActor::New( ResourceImage::New( DALI_IMAGE_DIR "animation-picture-5.png" ) );
    test1.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    test1.SetParentOrigin( ParentOrigin::TOP_LEFT );
    test1.SetSize( stageSize.width, stageSize.height );
    test1.SetPosition( 0.0f, 0.0f, 0.0f );
    mScrollViewContainer.Add( test1 );

#if 0
    ImageActor test1 = ImageActor::New( ResourceImage::New( DALI_IMAGE_DIR "animation-picture-5.png" ) );
    ImageActor test2 = ImageActor::New( ResourceImage::New( DALI_IMAGE_DIR "animation-picture-5.png" ) );
    ImageActor test3 = ImageActor::New( ResourceImage::New( DALI_IMAGE_DIR "animation-picture-5.png" ) );
    test1.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    test1.SetParentOrigin( ParentOrigin::TOP_LEFT );
    test2.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    test2.SetParentOrigin( ParentOrigin::TOP_LEFT );
    test3.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    test3.SetParentOrigin( ParentOrigin::TOP_LEFT );
    test1.SetSize( stageSize.width, stageSize.height );
    test2.SetSize( stageSize.width, stageSize.height );
    test3.SetSize( stageSize.width, stageSize.height );
    test1.SetPosition( 0.0f, 0.0f, 0.0f );
    test2.SetPosition( stageSize.width, 0.0f, 0.0f );
    test3.SetPosition( stageSize.width * 2.0f ,0.0f, 0.0f );
    mScrollViewContainer.Add( test1 );
    mScrollViewContainer.Add( test2 );
    mScrollViewContainer.Add( test3 );
#endif
#endif

    //todor
    m_ScrollView.Add( mScrollViewContainer );//todor add to scrollview
    //m_MainLayer.Add(m_ScrollView);


#if 0
    ImageActor test = Dali::Toolkit::CreateSolidColorActor( Color::GREEN );
    test.SetParentOrigin(ParentOrigin::CENTER);
    test.SetAnchorPoint(AnchorPoint::CENTER);
    test.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
    test.SetSize( 200, 200 );
    test.SetSortModifier( 10 );
#endif

    Layer layer = Layer::New();
    layer.SetParentOrigin(ParentOrigin::CENTER);
    layer.SetAnchorPoint(AnchorPoint::CENTER);
    layer.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
    layer.SetSize( 200, 200 );
    layer.SetDepthTestDisabled( false );


    ImageActor stencil = Dali::Toolkit::CreateSolidColorActor( Color::RED );
    stencil.SetDrawMode( DrawMode::STENCIL );
    stencil.SetParentOrigin(ParentOrigin::CENTER);
    stencil.SetAnchorPoint(AnchorPoint::CENTER);
    stencil.SetVisible( true );
    stencil.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    stencil.SetParentOrigin( ParentOrigin::CENTER );


    layer.Add( stencil );
    layer.Add( m_ScrollView );

    //test.Add( layer );



    DALI_LOG_ERROR( "blah\n" );

    Toolkit::Popup popup = Toolkit::Popup::New();

    popup.SetProperty( Toolkit::Popup::Property::ANIMATION_DURATION, 5 );
    popup.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
    popup.SetTitle( TextLabel::New( "title" ) );
    //popup.SetContent( m_ScrollView );
    popup.SetContent( layer );
    //popup.SetContent( layer );

    m_MainLayer.Add( popup );



    popup.SetDisplayState( Dali::Toolkit::Popup::SHOWN );










    // Respond to a click anywhere on the stage
 //   stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
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

int main( int argc, char **argv )
{
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);
  HelloWorldController test( app );
  app.MainLoop();
}




#endif























#if 1



// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    TextLabel textLabel = TextLabel::New( "Hello World" );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_CENTER );
    textLabel.SetName( "helloWorldLabel" );
    stage.Add( textLabel );

#if 0
    Toolkit::ImageView imageView = Toolkit::ImageView::New( IMAGE_FILENAME_ETC );
    imageView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    imageView.SetParentOrigin( ParentOrigin::TOP_LEFT );
    stage.Add( imageView );

#if 1
    imageView = Toolkit::ImageView::New( IMAGE_FILENAME_ASTC_LINEAR );
    imageView.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    imageView.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    stage.Add( imageView );
#endif


    imageView = Toolkit::ImageView::New( IMAGE_FILENAME_ASTC_SRGB );
    imageView.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    imageView.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    stage.Add( imageView );
#if 0
    imageView = Toolkit::ImageView::New( IMAGE_FILENAME_ASTC_NATIVE );
    imageView.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    imageView.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    stage.Add( imageView );
#endif
#endif


#if 1
    float xOffset = 64.0f;
    float x = 0.0f;

    //Image image4 = ResourceImage::New( IMAGE_FILENAME_TEST1 );
    Toolkit::ImageView imageViewIco;
#if 1
    imageViewIco = Toolkit::ImageView::New( IMAGE_FILENAME_TEST1 );
    imageViewIco.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    imageViewIco.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    stage.Add( imageViewIco );
    x += xOffset;

    imageViewIco = Toolkit::ImageView::New( IMAGE_FILENAME_TEST2 );
    imageViewIco.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    imageViewIco.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    imageViewIco.SetPosition( x, 0.0f );
    stage.Add( imageViewIco );

    x += xOffset;

    imageViewIco = Toolkit::ImageView::New( IMAGE_FILENAME_TEST3 );
    imageViewIco.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    imageViewIco.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    imageViewIco.SetPosition( x, 0.0f );
    stage.Add( imageViewIco );

    x += xOffset;

    imageViewIco = Toolkit::ImageView::New( IMAGE_FILENAME_TEST4 );
    imageViewIco.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    imageViewIco.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    imageViewIco.SetPosition( x, 0.0f );
    stage.Add( imageViewIco );

    x += xOffset;

    imageViewIco = Toolkit::ImageView::New( IMAGE_FILENAME_TEST5 );
    imageViewIco.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    imageViewIco.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    imageViewIco.SetPosition( x, 0.0f );
    stage.Add( imageViewIco );
    x += xOffset;
#endif
#if 1
    x = 5;
    imageViewIco = Toolkit::ImageView::New( IMAGE_FILENAME_TEST );
    imageViewIco.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    imageViewIco.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    imageViewIco.SetPosition( x, 0.0f );
    stage.Add( imageViewIco );

    x += xOffset;

    imageViewIco = Toolkit::ImageView::New( IMAGE_FILENAME_TEST6 );
    imageViewIco.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    imageViewIco.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    imageViewIco.SetPosition( x, 0.0f );
    stage.Add( imageViewIco );
#endif
#endif

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
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
  HelloWorldController test( application );

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
#endif
