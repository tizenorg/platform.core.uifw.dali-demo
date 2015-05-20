//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <sstream>

#include <dirent.h>
#include <dali/integration-api/debug.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const float ARBITRARY_FIELD_OF_VIEW = Math::PI / 4.0f;

const char* DETAILS_TEXT("<font size='9'>Details</font>");
const float DETAILS_BAR_HEIGHT = 200.0f;
const Vector4 DETAILS_BAR_BACKGROUND_COLOR( 0.098f, 0.098f, 0.439f, 1.f );
const Vector4 TEST_BACKGROUND_COLOR( 1.0f, 0.0f, 0.0f, 1.f );

const char* BUTTON_IMAGE( DALI_IMAGE_DIR "button.png" );
const char* BUTTON_PRESSED_IMAGE( DALI_IMAGE_DIR "buttonPressed.png" );
const char* SMILEY_IMAGE( DALI_IMAGE_DIR "icon.png" );
const char* STARS_IMAGE( DALI_IMAGE_DIR "score.png" );
const float IMAGE_BORDER = 10.0f;
const float IMAGE_OFFSET = 10.0f;
const float ICON_BORDER = 17.5f;

const float BUY_BAR = 220.0f;
const float TEXT_HEIGHT = 450.0f;
const float BUTTON_BAR_HEIGHT = 100.0f;
const float INDICATOR_OFFSET = 15.0f;
const float TITLE_HEIGHT = 20.0f;

const float PRICE_WIDTH = 80.0f;
const float PRICE_HEIGHT = 30.0f;

const float BUY_BUTTON_OFFSET = 10.0f;
const Vector3 BUY_BUTTON_SIZE(240.0,90.0,1.0);

static const Vector4 COLORS[] = { Vector4( 1.f, 1.f, 1.f, 1.f ), Vector4(0.f,0.f,0.f,1.f), Vector4( 0.36f, 0.36f, 0.36f ,1.f) };

const char* IMAGE_PATHS[] = { DALI_IMAGE_DIR "screenshot1.png", DALI_IMAGE_DIR "screenshot2.png", DALI_IMAGE_DIR "screenshot3.png", DALI_IMAGE_DIR"screenshot4.jpg", DALI_IMAGE_DIR"screenshot5.jpg", DALI_IMAGE_DIR"screenshot6.jpg", };

unsigned int MAX_IMAGES = 6;

const char* TEXT[] = {
     "I wondered what might happened If I left this all behind Would the wind be at my back Would I get you off my mind This time The neon lights in bars And headlights from the cars Have started a symphony Inside of me The things I left behind Tonight the sky above reminds me of you Walking through wintertime Where the stars all shine The angle on the stairs Will tell you I was there Under the front porch light On a mystery night I've been sitting watching life pass from the sidelines Been waiting for a dream to seep in through my blinds     I wondered what might happend If I left this all behind Would the wind be at my back Would I get you off my mind This time The neon lights in bars And headlights from the cars Have started a symphony Inside of me The things I left behind",
     "<font size='9'>Baby Toy</font>",
     "<font size='6'>Ahmed Hassen</font>",
     "<font size='9'>W1,000</font>",
     "<font size='9' color='blue'>Description</font>",
};

} // end namespace

/******************************************************************************
 * Creates a scrolling panel with text and images
 ******************************************************************************/

class ScrollingPanelPerformanceTest : public ConnectionTracker
{
public:

  ScrollingPanelPerformanceTest( Application &application )
  : mApplication( application ),
    mToolBar(),
    mActorRoot(),
    mStageSize(0.0f, 0.0f, 0.0f)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ScrollingPanelPerformanceTest::Create );
  }

  void SetUpDefaultView()
  {
    mStage = Dali::Stage::GetCurrent();
    mStageSize = mStage.GetSize();

    printf("mStageSize[%f, %f]\n", mStageSize.width, mStageSize.height);

    mActorRoot = Actor::New();
    mActorRoot.SetParentOrigin(ParentOrigin::CENTER);
    mActorRoot.SetAnchorPoint(AnchorPoint::CENTER);
    mActorRoot.SetSize(mStageSize);

    mStage.Add (mActorRoot);

    Layer backgroundLayer = Layer::New();

    // Set background color
    Stage::GetCurrent().SetBackgroundColor( COLORS[1] );
  }

  // Only render the view
  void RenderView()
  {
    // Change the default render-task to only render the view (not the ImageActors)
    RenderTaskList taskList = mStage.GetRenderTaskList();
    RenderTask defaultTask = taskList.GetTask( 0u );
    if ( defaultTask )
    {
      defaultTask.SetSourceActor( mActorRoot );

      // Used for input handling, when rendering to a framebuffer
      defaultTask.SetScreenToFrameBufferFunction( RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION );
    }
  }

  // Create and place a camera for to render text, corresponding to its render target size
  CameraActor CreateCameraForOffScreenText(Vector2 targetSize)
  {
    const float cameraPosConstraintScale( 0.5f / tanf(ARBITRARY_FIELD_OF_VIEW * 0.5f) );

    CameraActor textCamera = CameraActor::New();
    textCamera.SetParentOrigin(ParentOrigin::CENTER);
    textCamera.SetFieldOfView(ARBITRARY_FIELD_OF_VIEW);
    textCamera.SetNearClippingPlane(1.0f);
    textCamera.SetAspectRatio(targetSize.width / targetSize.height);
    textCamera.SetPosition(0.0f, 0.0f, targetSize.height * cameraPosConstraintScale);

    mStage.Add( textCamera ); // camera to shoot the offscreen text

    return textCamera;
  }

  void Create( Application& application )
  {
    SetUpDefaultView();

    CreateDetailsBar( DETAILS_BAR_HEIGHT );
    mActorRoot.Add( mDetailsBar );

    CreateScrollView( mStageSize.height - DETAILS_BAR_HEIGHT );
    Layer scrollViewLayer = Layer::New();
    scrollViewLayer.SetSize( Vector2(mStageSize.width, mStageSize.height - DETAILS_BAR_HEIGHT ) );
    scrollViewLayer.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );
    scrollViewLayer.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );

    scrollViewLayer.SetClipping(true);
    scrollViewLayer.SetClippingBox( 0, 0, mStageSize.width, mStageSize.height - DETAILS_BAR_HEIGHT );

    scrollViewLayer.Add( mScrollView );
    mActorRoot.Add( scrollViewLayer );

    // Only render the view not other actors just added to stage
    RenderView();
  }

private:

  void CreateTextAndIconObjects();

  void CreateDetailsBar( int height );
  void CreateScrollView( int height );

private:
  Application&  mApplication;
  Toolkit::ToolBar mToolBar;
  Actor mActorRoot;

  Dali::Stage mStage;
  Vector3 mStageSize;
  unsigned int mNumberOfIcons;
  ScrollView mScrollView;
  Actor mDetailsBar;

  FrameBufferImage mFrameBuffer; // Destination to render TextViews which will then become ImageActors.
  RenderTask mTextExclusiveRenderTask;
};

void ScrollingPanelPerformanceTest::CreateDetailsBar( int height )
{
  mDetailsBar = CreateSolidColorActor( DETAILS_BAR_BACKGROUND_COLOR );
  mDetailsBar.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  mDetailsBar.SetParentOrigin( ParentOrigin::TOP_CENTER );
  mDetailsBar.SetPosition( 0.0f, 0.0f, 1.0f );

  mDetailsBar.SetSize( mStageSize.width, height );

  TextView detailsTitle = TextView::New( DETAILS_TEXT );
  detailsTitle.SetAnchorPoint( AnchorPoint::CENTER );
  detailsTitle.SetParentOrigin( ParentOrigin::CENTER );
  mDetailsBar.Add( detailsTitle );
}

void ScrollingPanelPerformanceTest::CreateScrollView( int height )
{
  // Create a ScrollView and add to stage.
  mScrollView = ScrollView::New();

  mScrollView.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
  mScrollView.SetParentOrigin( ParentOrigin::BOTTOM_LEFT );

  mScrollView.SetSize( mStageSize.width, height );

  // Restrict scrolling to y axis and not past last item
  RulerPtr rulerX = new DefaultRuler();
  RulerPtr rulerY = new DefaultRuler();
  rulerX->SetDomain(RulerDomain(0.0f, mStageSize.width, true));
  rulerY->SetDomain(RulerDomain(0.0f, (mStageSize.height+IMAGE_OFFSET)*MAX_IMAGES + INDICATOR_OFFSET + BUY_BAR + TEXT_HEIGHT + BUTTON_BAR_HEIGHT, true));
  mScrollView.SetRulerX(rulerX);
  mScrollView.SetRulerY(rulerY);

  // Add Content

  Image buttonImage = Image::New( BUTTON_IMAGE );
  Image pressedImage = Image::New( BUTTON_PRESSED_IMAGE );

  // Add 'buy' bar which shows title, icon, description heading and app info.
  Actor buyBar = Actor::New();
  buyBar.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buyBar.SetParentOrigin( ParentOrigin::TOP_LEFT );
  buyBar.SetSize( mStageSize.width, BUY_BAR );
  buyBar.SetPosition( IMAGE_OFFSET, INDICATOR_OFFSET );

  // child of 'buy' bar
  TextView buyTitle = TextView::New(TEXT[1]);
  buyTitle.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buyTitle.SetParentOrigin( ParentOrigin::TOP_LEFT );
  buyBar.Add( buyTitle );

  // child of 'buy' bar
  Image smileyImage = Image::New( SMILEY_IMAGE );
  ImageActor buyIcon = ImageActor::New( smileyImage );
  buyIcon.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buyIcon.SetParentOrigin( ParentOrigin::TOP_LEFT );
  buyIcon.SetPosition( 0.0f, INDICATOR_OFFSET + TITLE_HEIGHT + ICON_BORDER/2);
  buyBar.Add( buyIcon );

  // app info shows author name, price of app and rating
  Actor appInfo = Actor::New();
  appInfo.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  appInfo.SetParentOrigin( ParentOrigin::TOP_RIGHT );
  appInfo.SetPosition( ICON_BORDER , 0.0f );
  buyIcon.Add ( appInfo );

  TextView buyAuthor = TextView::New(TEXT[2]);
  buyAuthor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buyAuthor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  appInfo.Add( buyAuthor );

  Image stars = Image::New( STARS_IMAGE );
  ImageActor rating = ImageActor::New( stars );
  rating.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  rating.SetParentOrigin( ParentOrigin::TOP_LEFT );
  rating.SetPosition( 0.0, TITLE_HEIGHT*2.0);
  appInfo.Add( rating );

  TextView buyPrice = TextView::New(TEXT[3]);
  buyPrice.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buyPrice.SetParentOrigin( ParentOrigin::TOP_LEFT );
  buyPrice.SetPosition( 0.0, TITLE_HEIGHT*3.0);
  buyPrice.SetSize( PRICE_WIDTH, PRICE_HEIGHT );
  buyPrice.SetWidthExceedPolicy(TextView::ShrinkToFit);
  appInfo.Add( buyPrice );

  // child of 'buy' bar
  TextView buyDescription = TextView::New(TEXT[4]);
  buyDescription.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buyDescription.SetParentOrigin( ParentOrigin::TOP_LEFT );
  buyDescription.SetPosition( 0.0f , INDICATOR_OFFSET + TITLE_HEIGHT*6.5 );
  buyBar.Add( buyDescription );

  PushButton buyButton = PushButton::New();
  buyButton.SetLabelText("Buy");
  buyButton.SetBackgroundImage( buttonImage );
  buyButton.SetAnchorPoint( AnchorPoint::CENTER_RIGHT );
  buyButton.SetParentOrigin( ParentOrigin::CENTER_RIGHT );
  buyButton.SetPosition( 0.0 - BUY_BUTTON_OFFSET, 0.0 );
  buyButton.SetSize( BUY_BUTTON_SIZE );
  buyBar.Add(buyButton);

  mScrollView.Add( buyBar );

  // Add text
  Vector2 textSize = Vector2( mStageSize.width - IMAGE_OFFSET, TEXT_HEIGHT );

  Dali::ImageActor textBackgroundImage = CreateSolidColorActor( COLORS[2]);
  textBackgroundImage.SetSize( textSize );
  textBackgroundImage.SetAnchorPoint( AnchorPoint::TOP_CENTER );
  textBackgroundImage.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  buyBar.Add(textBackgroundImage);

  TextView text = TextView::New(TEXT[0]);
  text.SetMultilinePolicy(TextView::SplitByWord);
  text.SetWidthExceedPolicy(TextView::ShrinkToFit);

  text.SetParentOrigin( ParentOrigin::CENTER );

  text.SetSize( textSize );
  text.SetPosition( 0.0, 0.0, 1.0 ); // ensure text is in-front of

  textBackgroundImage.Add(text);

  // Add images
  for ( unsigned int index = 0; index < MAX_IMAGES; index++)
  {
    Image imageSource = Image::New( IMAGE_PATHS[index] );
    ImageActor image = ImageActor::New(imageSource);

    image.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    image.SetParentOrigin( ParentOrigin::TOP_LEFT );

    image.SetSize(mStageSize.width - IMAGE_BORDER, mStageSize.height);
    image.SetPosition( ( IMAGE_BORDER) *0.5, TEXT_HEIGHT + BUY_BAR + INDICATOR_OFFSET +( index*( mStageSize.height + IMAGE_OFFSET) ) );
    mScrollView.Add(image);
  }

  // Add button bar
  Actor buttonBar = Actor::New();
  buttonBar.SetSize(mStageSize.width, BUTTON_BAR_HEIGHT);
  buttonBar.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  buttonBar.SetParentOrigin( ParentOrigin::TOP_LEFT );

  PushButton commentButton = PushButton::New();
  commentButton.SetLabelText("no comments");
  commentButton.SetBackgroundImage( buttonImage );
  commentButton.SetPressedImage( pressedImage );
  commentButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  commentButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  commentButton.SetSize( mStageSize.width - IMAGE_OFFSET, BUTTON_BAR_HEIGHT/2.5 );

  buttonBar.Add(commentButton);

  PushButton boughtButton = PushButton::New();
  boughtButton.SetLabelText("Customers also bought");
  boughtButton.SetBackgroundImage( buttonImage );
  boughtButton.SetPressedImage( pressedImage );
  boughtButton.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
  boughtButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
  boughtButton.SetSize( mStageSize.width - IMAGE_OFFSET, BUTTON_BAR_HEIGHT/2.5 );

  buttonBar.Add(boughtButton);

  buttonBar.SetPosition(IMAGE_OFFSET*0.5, mStageSize.height*MAX_IMAGES + IMAGE_OFFSET*MAX_IMAGES + TEXT_HEIGHT + BUY_BAR + INDICATOR_OFFSET );

  mScrollView.Add(buttonBar);
}

void RunTest( Application& application )
{
  ScrollingPanelPerformanceTest test( application );

  application.MainLoop();
}

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
