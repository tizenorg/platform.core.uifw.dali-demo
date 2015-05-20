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
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "hot-pink-bg-1024x1024.jpg" );
const char* IMAGE_FOLDER( PERFORMANCE_TEXT_IMAGE_DIR );

const Vector3 DEFAULT_TEXT_SIZE(250.0f,80.0f, 1.0);
const Vector3 DEFAULT_ICON_SIZE(15.0f,15.0f, 1.0);

static const Vector4 COLORS[] = { Vector4( 1.f, 1.f, 1.f, 1.f ), Vector4(0.f,0.f,0.f,1.f), Vector4( (1.0f/255.0f), (1.0f/20.0f), 1.0f ,1.f) };

static const char* FONT_FACE = "Vera-Bold";

const unsigned int TEXT_ITEM_COUNT = 25; // number of text-views to display

const float TEXT_SPEED[] = { 3.6, 4.1, 6.0, 5.0, 3.4, 5.7, 3.7, 4.4, 3.6, 4.3, 6.0 };
const unsigned int SPEED_COUNT = 11; // number of different text speeds

const float LINEAR_DURATION = 38.4;
const float SINE_DURATION = 34.4;
const float SCALE_DURATION = 5.0;

const unsigned int MIN_VALUE_TO_REDUCE_DURATION = 6.0; // Increasing this value closer to the SCALE_DURATION speeds up scaling. (should not be equal).

const char* TEXT_TEXT[] = {
     "Tonight the sky above reminds me of you",
     "Walking through wintertime",
     "Where the stars all shine",
     "The angle on the stairs",
     "Will tell you I was there",
     "Under the front porch light",
     "On a mystery night",
     "I've been sitting watching life pass from the sidelines",
     "Been waiting for a dream to seep in through my blinds",
     "I wondered what might happend",
     "If I left this all behind",
     "Would the wind be at my back",
     "Could I get you off my mind",
     "This time",
     "The neon lights in bars",
     "And headlights from the cars",
     "Have started a symphony",
     "Inside of me",
     "The things I left behind",
     "Have melted in my mind",
     "And now there's a purity",
     "Inside of me",
     "I've been sitting watching life pass from the sidelines",
     "Been waiting for a dream to seep in through my blinds",
     "I wondered what might happend",
};

} // end namespace

/******************************************************************************
 * Simply fills the stage with textinput actor with a child icon
 * Then aninmates them in position and scale
 * To see the performance metrics like frame rate and power consumption.
 ******************************************************************************/

class TextViewPerformanceTest : public ConnectionTracker
{
public:

  TextViewPerformanceTest( Application &application )
  : mApplication( application ),
    mView(),
    mToolBar(),
    mActorRoot(),
    mStageSize(0.0f, 0.0f, 0.0f)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextViewPerformanceTest::Create );
  }

  void SetUpDefaultView()
  {
    Dali::Stage stage = Dali::Stage::GetCurrent();
    mStageSize = stage.GetSize();

    mActorRoot = Actor::New();
    mActorRoot.SetParentOrigin(ParentOrigin::CENTER);
    mActorRoot.SetAnchorPoint(AnchorPoint::CENTER);
    mActorRoot.SetSize(mStageSize);

    stage.Add (mActorRoot);

    Layer backgroundLayer = Layer::New();

    // Set background image.
    Dali::Image backgroundImage = Dali::Image::New( BACKGROUND_IMAGE );
    Dali::ImageActor backgroundImageActor = Dali::ImageActor::New( backgroundImage );
    backgroundImageActor.SetAnchorPoint( AnchorPoint::CENTER );
    backgroundImageActor.SetParentOrigin( ParentOrigin::CENTER );
    backgroundImageActor.SetSize(mStageSize);
    backgroundLayer.SetAnchorPoint( AnchorPoint::CENTER );
    backgroundLayer.SetParentOrigin( ParentOrigin::CENTER );
    backgroundLayer.SetSize(mStageSize);
    backgroundLayer.Add( backgroundImageActor );

    // Add background
    mActorRoot.Add( backgroundLayer );

    // Drop the background layer
    backgroundLayer.LowerToBottom();
  }

  void Create( Application& application )
  {
    SetUpDefaultView();

    LoadImages();

    CreateTextAndIconObjects();
  }

private:

  void LoadImages();

  void CreateTextAndIconObjects();

private:
  Application&  mApplication;
  Toolkit::View mView;
  Toolkit::ToolBar mToolBar;
  Actor mActorRoot;
  Vector3 mStageSize;
  unsigned int mNumberOfIcons;
  std::vector<Image> mImageList;
};

void TextViewPerformanceTest::LoadImages()
{
  // Get all the file paths and file names in the image resource folder
  DIR *dir;
  struct dirent *ent;
  std::stringstream fileName;
  std::vector<std::string> imageFileList;
  if ((dir = opendir (IMAGE_FOLDER)) != NULL)
  {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL)
    {
      if(ent->d_name[0] == '.') continue;
      fileName << IMAGE_FOLDER <<"/"<< ent->d_name;
      imageFileList.push_back(fileName.str());
      fileName.str( "" );
    }
    closedir (dir);
  } else
  {
    /* could not open directory */
    std::cout << "failed to open images" << std::endl;
    return;
  }

  // Load images, and keep them in a vector (mImageList)
  mNumberOfIcons = imageFileList.size();
  Image image;
  for(unsigned int i = 0; i<mNumberOfIcons; i++)
  {
    image = Image::New(imageFileList[i]);
    mImageList.push_back(image);
  }
  imageFileList.clear();
}

/* Puts valid ascii characters in an Array*/
void TextViewPerformanceTest::CreateTextAndIconObjects()
{
  for ( unsigned int i=0; i < TEXT_ITEM_COUNT; i++ )
  {
    // Create Text objects
    TextView textViewAlpha = Toolkit::TextView::New();
    textViewAlpha.SetText(TEXT_TEXT[i]);
    textViewAlpha.SetParentOrigin( ParentOrigin::TOP_LEFT );
    textViewAlpha.SetAnchorPoint( AnchorPoint::BOTTOM_LEFT );
    textViewAlpha.SetWidthExceedPolicy(TextView::ShrinkToFit);

    // Add offset so text objects do not all start at the same place
    //float xPositionOffSet = std::max( mStageSize.x/20,  (mStageSize.x/5) * (i%3) ); // Add a changing offset.
    float xPositionOffSet =  mStageSize.x/10 + ( (mStageSize.x/5) * (i%4) ); // Add a persistent offset of 1/10 of the stage width plus a changing offset.
    float yPositionOffSet = (mStageSize.y/7) * (i%3);

    // Set Initial size and Position
    textViewAlpha.SetPosition( Vector3( xPositionOffSet, mStageSize.y+yPositionOffSet, 0.0f ) );
    textViewAlpha.SetSize( DEFAULT_TEXT_SIZE );

    // Add icons
    int index = i%mNumberOfIcons;
    ImageActor icon  = ImageActor::New(mImageList[index]);
    icon.SetParentOrigin(ParentOrigin::CENTER_RIGHT);
    icon.SetAnchorPoint( AnchorPoint::CENTER_LEFT);
    icon.SetSize( DEFAULT_ICON_SIZE );
    textViewAlpha.Add(icon);

    // Apply Colour
    TextStyle style;
    style.SetTextColor( COLORS[i%2] );
    style.SetFontName( FONT_FACE );
    textViewAlpha.SetStyleToCurrentText( style,  static_cast<TextStyle::Mask>(TextStyle::COLOR | TextStyle::FONT));

    // add to scene
    mActorRoot.Add( textViewAlpha );

    // Set up animations to move and scale

    switch (i%2)
    {
      case 0:
      {
        Animation linearAnimation = Animation::New( LINEAR_DURATION/TEXT_SPEED[i%SPEED_COUNT] );
        linearAnimation.SetLooping( true );
        linearAnimation.MoveTo( textViewAlpha, Vector3( xPositionOffSet , 0.0f, 0.0f ), AlphaFunctions::Linear );
        linearAnimation.Play();
        break;
      }
      case 1:
      {
        Animation sineAnimation = Animation::New( SINE_DURATION/TEXT_SPEED[i%SPEED_COUNT] );
        sineAnimation.SetLooping( true );
        sineAnimation.MoveTo( textViewAlpha, Vector3( xPositionOffSet , 0.0f, 0.0f ), AlphaFunctions::Linear );
        sineAnimation.Play();
        break;
      }
    }
    // Scale some textviews with a slightly different duration
    Animation scaleAnimation = Animation::New( SCALE_DURATION/std::min( MIN_VALUE_TO_REDUCE_DURATION,( (i+1)%8)) ); // Ensure scale duration is not lower than min value and index not 0;
    scaleAnimation.SetLooping( true );
    scaleAnimation.ScaleBy( textViewAlpha, Vector3( 0.80f , 0.60f, 0.0f ), AlphaFunctions::Bounce );
    scaleAnimation.ScaleBy( icon, Vector3( 0.20f , 0.20f, 0.0f ), AlphaFunctions::Bounce );
    scaleAnimation.Play();
  }
}

void RunTest( Application& application )
{
  TextViewPerformanceTest test( application );

  application.MainLoop();
}

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
