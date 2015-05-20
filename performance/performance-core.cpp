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

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>

#include <sstream>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../examples/shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
enum ActorType
{
  ImageType,
  TextType,
  MeshType,
  MixedType
};

// list the enum values according to the order of the steps taking place
enum OperatingStep
{
  AddToStage,
  AnimateWithoutConstraits,
  ApplyConstraints,
  AnimateWithConstraints,
  TakeOffStage,
  PutBackStage,
  SetInvisible,
  SetVisible,
  CleanUp,
  QuitApp
};

const char* APPLICATION_TITLE( "Performance test - core" );
const char* IMAGE_FOLDER( DALI_IMAGE_DIR ); // the folder with all the image resources for testing
const char* MODEL_FILES[] =
{
  DALI_MODEL_DIR "DynamicsShapes.dae"
};
const unsigned int NUM_MODEL( sizeof( MODEL_FILES ) / sizeof( char* ) );

const float SHAPE_SIZE(50.0f);
const float IMAGE_ACTOR_SIZE(128.0f);

const unsigned int NUM_ACTORS_MEDIUM( 50 );
const unsigned int NUM_ACTORS_LARGE( 500 );

const float ANIMATION_TIME ( 5.0f ); // animation length in seconds

const char* TEXT_DATA[] =
{
    "안녕하세요", "SLPSansKorean",                // Korean
  "Hello Dali", "HelveticaNeue",            // English
  "简体中文", "UDGRu",                        // Simplified Chinese
  "繁體中文", "UDGRu",                        // Traditional Chinese Taiwan (Mandarin)
  "標準粵語", "SLPSansFallback",              // Traditional Chinese HongKong (Cantonese)
  "zeeëend", "HelveticaNeue",                // Dutch
  "caissière", "HelveticaNeue",              // French
  "Großer Bär", "HelveticaNeue",             // German
  "Ηειιο", "HelveticaNeue",                  // Greek :)
  "perché", "HelveticaNeue",                 // Italian
  "宮崎 駿", "UDGRu",                         // Japanese
  "coração", "HelveticaNeue",                // Portugese
  "здравствуйте", "HelveticaNeue",           // Russian
  "¿Habla Español?", "HelveticaNeue",        // Spanish
  "Kimler gülmüş", "HelveticaNeue",          // Turkish
  "Väinämöinen", "HelveticaNeue",            // Finnish
  "Árvíztűrőtükörfúrógép", "HelveticaNeue"   // Hungarian
};

const unsigned int NUM_TEXTS( 0.5f * sizeof( TEXT_DATA ) / sizeof( char* ) );

const float ZIGZAG_X(40.0f); // X offset distance of the constrained layout
const float ZIGZAG_Y(-3.0f); // Y offset distance of the constrained layout
const float ZIGZAG_Z(-5.0f); // Z offset distance of the constrained layout
struct TestLayoutPosition  // Zigzag pattern layout of actor position
{
  TestLayoutPosition( float offset )
  : mOffset( offset )
  {
  }

  Vector3 operator()(const Vector3& current, const PropertyInput& layoutPositionProperty, const PropertyInput& layoutIndexProperty)
  {
    const Vector3 layoutPosition = layoutPositionProperty.GetVector3();
    float offset_YZ = mOffset + layoutIndexProperty.GetFloat();
    float offset_X = ( offset_YZ/20.0f - static_cast<float>( floor(offset_YZ/20.0f) ) < 0.5f ) ?
                     ( offset_YZ/10.0f - static_cast<float>( floor(offset_YZ/10.0f) ) ) :
                     ( 1.0f - offset_YZ/10.0f + static_cast<float>( floor(offset_YZ/10.0f) ) );
    offset_X *= 10.0f;
    Vector3 offsetVector = Vector3( ZIGZAG_X*offset_X, ZIGZAG_Y*offset_YZ, ZIGZAG_Z*offset_YZ);
    return layoutPosition + offsetVector;
  }

  float mOffset;
};

struct TestLayoutSize
{
  Vector3 operator()(const Vector3& current, const PropertyInput& layoutSizeProperty)
  {
    return layoutSizeProperty.GetVector3();
  }
};

struct TestLayoutRotation
{
  Quaternion operator()(const Quaternion& current, const PropertyInput& layoutRotationProperty)
  {
    float angle = Math::PI  / 10.0f;
    return layoutRotationProperty.GetQuaternion() * Quaternion(angle, Vector3::YAXIS);
  }
};

}

/******************************************************************************
 * fills the stage with ImageActors,TextActors and MeshActors
 * To see the performance metrics like frame rate,.....
 *
 * Performance test app
 * - Fully automated, no user interaction, run from command line
 * - All resources included in the application package
 * - Proper size images, each actor using different image
 * - Multiple languages of text (multiple fonts as well)
 * - Multiple meshes
 * - Starts up, creates Application and from Initialized signal creates a timer to run (tick) the tests
 *
 * Initial test
 * - Load images (time how long from create to image loaded signal called)
 * - Load fonts (time how long from create to text available signal called) // no signal emit to do this?
 * - Load models (time how long from create to model available signal called)
 * Outer Test loop
 * Inner Test loop
 * - Test plain actors displaying (no constraints, no animations) – 5s
 * - Animate actor properties (position, size, color, rotation) – 5s
 * - Apply constraints on properties (no animations) – 5s
 * - Animate actors while constraints are in place – 5s
 * - Take actors off stage and put back on stage – 2s+2s
 * - Show / Hide actors – 2s+2s
 * - Clean the scene - 2s
 * Run the same loop with Image, Text, Mesh actors and finally with a combination of all three actors
 * Run the outer test loop first with medium amount of actors (50) and then large (500)
 ******************************************************************************/

class PerformanceTest : public ConnectionTracker
{
public:

  PerformanceTest( Application &application );
  void OnInit( Application& application );

  void LoadImages();
  void OnImageLoadingFinished(Image image);
  void LoadModels();
  void OnModelLoadingFinished(Model model);
  void LoadFonts();

  bool OnTick();

  // the parameter indicates how many actors to add
  void AddImageActors (unsigned int number);
  void AddTextActors (unsigned int number);
  void AddMeshActors (unsigned int number);

  // Animate actor properties (position, size, color, rotation); without constraint; animate each actor
  void AnimateActors();
  // Apply constraints on properties (position, size, rotation); no animations
  void ConstrainActors();
  // Animate actor properties (position, size, rotation); apply animation on the first actor, other actors will animate with constraints.
  void AnimateConstrainedActors();
  // Take actors off stage
  void RemoveActorsFromStage();
  // Put actors back on stage
  void AddActorsToStage();
  // Hide actors
  void SetActorsInvisible();
  // Show actors
  void SetActorsVisible();
  // Removes actors from stage and reset them
  void CleanScene();

private:
  Application&                mApplication;
  Toolkit::View               mView;
  Vector2                     mStageSize;
  Layer                       mContent;

  Actor                       mActorRoot;
  std::vector<Actor>          mActorList;

  std::vector<Image>          mImageList;
  std::vector<Font>           mFontList;
  std::vector<Model>          mModelList;

  Timer                       mTimer;
  ActorType                   mActorType;
  OperatingStep               mOperatingStep;

  int                         mPropertyLayoutIndex; //The index of a new property registered to indicate the layout offset

  unsigned int                mNumActors; //NUM_ACTORS_MEDIUM or NUM_ACTORS_LARGE
  unsigned int                mNumImages; //How many images in the image resource folder
  unsigned int                mNumImagesLoaded; //To record how many images already loaded during loading
  unsigned int                mNumModelsLoaded; //To record how many models already loaded during loading

  timespec                    mImageLoadingStartTime;
  timespec                    mModelLoadingStartTime;
};

PerformanceTest::PerformanceTest( Application &application )
: mApplication( application ),
  mView(),
  mActorType(ImageType),
  mOperatingStep(AddToStage),
  mNumActors(NUM_ACTORS_MEDIUM),
  mNumImagesLoaded(0),
  mNumModelsLoaded(0)
{
  // Connect to the Application's Init signal
  mApplication.InitSignal().Connect( this, &PerformanceTest::OnInit );
}

void PerformanceTest::OnInit( Application& application )
{
  // The Init signal is received once (only) during the Application lifetime

  Stage stage = Stage::GetCurrent();
  mStageSize = stage.GetSize();

  // Creates a default view with a default tool bar.
  // The view is added to the stage.
  Toolkit::ToolBar toolBar;
  mContent = DemoHelper::CreateView( application,
                                     mView,
                                     toolBar,
                                     std::string(),
                                     "",
                                     APPLICATION_TITLE );

   // Add a light to illuminate the scene
  Light light = Light::New("PointLight");
  LightActor keyLight = LightActor::New();
  keyLight.SetParentOrigin(ParentOrigin::CENTER);
  keyLight.SetPosition(-200.0f, -300.0f, 300.0f);
  mContent.Add(keyLight);
  keyLight.SetLight(light);

  mActorRoot = Actor::New();
  mActorRoot.SetParentOrigin(ParentOrigin::CENTER);
  mActorRoot.SetAnchorPoint(AnchorPoint::CENTER);
  stage.Add (mActorRoot);

  // Load Resources
  std::cout<<std::endl<<"============resources loading============"<<std::endl;
  LoadImages();
  LoadModels();
  LoadFonts();

  // Create and start the timer
  // 10 seconds, leave enough time for the resources loading
  mTimer = Timer::New(10000.0f);
  mTimer.TickSignal().Connect( this, &PerformanceTest::OnTick );
  mTimer.Start();
}

void PerformanceTest::LoadImages()
{
  // Get all the file paths and file names in the image resource folder
  DIR *dir;
  struct dirent *ent;
  std::stringstream fileName;
  std::vector<std::string> imageFileList;
  if ((dir = opendir (IMAGE_FOLDER)) != NULL)
  {
    while ((ent = readdir (dir)) != NULL)
    {
      // Remove . and ..
      if(ent->d_name[0] == '.') continue;

      fileName << IMAGE_FOLDER << ent->d_name;  // IMAGE_FOLDER already contains "/"

      // Only try and load regular files.
      struct stat statInfo;
      if( stat( fileName.str().c_str(), &statInfo ) == 0 )
      {
        if( S_ISREG( statInfo.st_mode ) )
        {
          imageFileList.push_back(fileName.str());
        }
      }
      fileName.str( "" );
    }
    closedir (dir);
  } else
  {
    /* could not open directory */
    return;
  }

  // Load images, and keep them in a vector (mImageList)
  mNumImages = imageFileList.size();
  clock_gettime(CLOCK_MONOTONIC, &mImageLoadingStartTime);
  Image image;
  for(unsigned int i = 0; i<mNumImages; i++)
  {
    image = Image::New(imageFileList[i]);
    image.LoadingFinishedSignal().Connect(this, &PerformanceTest::OnImageLoadingFinished);
    mImageList.push_back(image);
  }
  imageFileList.clear();
}

void PerformanceTest::OnImageLoadingFinished( Image image )
{
  DALI_ASSERT_ALWAYS( image.GetLoadingState() == ResourceLoadingSucceeded );

  mNumImagesLoaded++;

  // Record the time used for loading the first mNumImages/10 images
  if(mNumImagesLoaded == NUM_ACTORS_MEDIUM)
  {
    timespec loadingEndTime;
    clock_gettime(CLOCK_MONOTONIC, &loadingEndTime);
    float delta = loadingEndTime.tv_sec  - mImageLoadingStartTime.tv_sec  +
                ( loadingEndTime.tv_nsec - mImageLoadingStartTime.tv_nsec ) / 1.e9;
    std::cout<<"image loading time ("<< NUM_ACTORS_MEDIUM << " images) :" << delta << " seconds"<<std::endl;
  }
  // Record the time used for loading all the images in the image resource folder
  if(mNumImagesLoaded == mNumImages)
  {
    timespec loadingEndTime;
    clock_gettime(CLOCK_MONOTONIC, &loadingEndTime);
    float delta = loadingEndTime.tv_sec  - mImageLoadingStartTime.tv_sec +
                ( loadingEndTime.tv_nsec - mImageLoadingStartTime.tv_nsec ) / 1.e9;
    std::cout<<"image loading time ("<< mNumImages << " images) :" << delta << " seconds"<<std::endl;
  }
}

void PerformanceTest::LoadModels()
{
  // Load models, and keep them in a vector (mModelList)
  Model model;
  clock_gettime(CLOCK_MONOTONIC, &mModelLoadingStartTime);
  for(unsigned int i = 0; i<NUM_MODEL; i++)
  {
    model = Model::New(MODEL_FILES[i]);
    model.LoadingFinishedSignal().Connect(this, &PerformanceTest::OnModelLoadingFinished);
    mModelList.push_back(model);
  }
}

void PerformanceTest::OnModelLoadingFinished(Model model)
{
  DALI_ASSERT_ALWAYS(model.GetLoadingState() == ResourceLoadingSucceeded );
  mNumModelsLoaded++;

  // Record the time used for loading all the models
  if(mNumModelsLoaded == NUM_MODEL)
  {
    timespec loadingEndTime;
    clock_gettime(CLOCK_MONOTONIC, &loadingEndTime);
    float delta = loadingEndTime.tv_sec  - mModelLoadingStartTime.tv_sec +
                ( loadingEndTime.tv_nsec - mModelLoadingStartTime.tv_nsec ) / 1.e9;
    std::cout<<"model loading time ("<< NUM_MODEL << " models) :" << delta << " seconds"<<std::endl;
  }
}

void PerformanceTest::LoadFonts()
{
  std::vector<std::string>    fontFamilyList;
  std::vector<std::string>    fontStyleList;
  fontFamilyList = Font::GetInstalledFonts();

  fontStyleList.push_back("Book");
  fontStyleList.push_back("Bold");
  fontStyleList.push_back("Italic");
  fontStyleList.push_back("medium");
  fontStyleList.push_back("Oblique");
  fontStyleList.push_back("Regular");

  // Load fonts, and keep them in a vector (mFontList)
  for(unsigned int i = 0; i<fontFamilyList.size(); i++)
    for(unsigned int j = 0; j<fontStyleList.size(); j++)
   {
     Font font = Font::New(  FontParameters(fontFamilyList[i], fontStyleList[j], PointSize(10)) );
     mFontList.push_back(font);
   }
}

bool PerformanceTest::OnTick()
{
  // when the timer ticking
  switch(mOperatingStep)
  {
    case AddToStage:
    {
      mTimer.SetInterval(5000.0f);
      switch(mActorType)
      {
        case ImageType: // add *image* actors and display them
        {
          AddImageActors(mNumActors);
          break;
        }
        case TextType:// add  *text* actors and display them
        {
          AddTextActors(mNumActors);
          break;
        }
        case MeshType: // add *mesh* actors and display them
        {
          AddMeshActors(mNumActors);
          break;
        }
        case MixedType:// add *a combination of all three actors* and display them
        {
          unsigned int num = (mNumActors+1)/3;
          AddImageActors(num);
          AddTextActors(num);
          AddMeshActors(num-1);
          break;
        }
        default:
        {
        }
      }
      break;
    }
    //-- Animate actor properties (position, size, color, rotation) – 5s
    case AnimateWithoutConstraits:
    {
      AnimateActors();
      break;
    }
    //--  Apply constraints on properties (no animations) – 5s
    case ApplyConstraints:
    {
      ConstrainActors();
      break;
    }
    //-- Animate actors while constraints are in place – 5s
    case AnimateWithConstraints:
    {
      AnimateConstrainedActors();
      break;
    }
    //-- Take actors off stage – 2s
    case TakeOffStage:
    {
      mTimer.SetInterval(2000.0f); // reset the timer interval to be 2 seconds
      RemoveActorsFromStage();
      break;
    }
    //-- Put actors back on stage – 2s
    case PutBackStage:
    {
      AddActorsToStage();
      break;
    }
    //-- Hide actors – 2s
    case SetInvisible:
    {
      SetActorsInvisible();
      break;
    }
    //-- Show actors – 2s
    case SetVisible:
    {
      SetActorsVisible();
      break;
    }
    //-- Removes actors from stage and reset them – 2s
    case CleanUp:
    {
      CleanScene();
      if( mActorType == MixedType )
      {
        if( mNumActors != NUM_ACTORS_LARGE )
        {
          mNumActors = NUM_ACTORS_LARGE;
          mActorType = ImageType;
        }
        else
        {
          mOperatingStep = QuitApp;
        }
      }
      else
      {
        if( mActorType == ImageType && mNumActors == NUM_ACTORS_LARGE)
        {
          mImageList.resize((mNumActors+1)/3);
        }
        // update to the next actor type
        mActorType = ActorType((int(mActorType)+1));
      }
      break;
    }
    case QuitApp:
    default: // finally quit the application after all the above steps
    {
      mImageList.clear();
      mFontList.clear();
      mModelList.clear();
      mApplication.Quit();
      //exit(0);
      break;
    }
  }
  // update to the next step
  if(mOperatingStep != CleanUp)
  {
    mOperatingStep = OperatingStep(int(mOperatingStep)+1);
  }
  else
  {
    mOperatingStep = AddToStage;
  }
  return true;
}

void PerformanceTest::AddImageActors(unsigned int number)
{
  ImageActor imageActor;
  for(unsigned int i = 0; i<number; i++)
  {
    int index = i%mNumImages;
    imageActor = ImageActor::New(mImageList[index]);
    float ratio = std::min(3.0f, static_cast<float>(mImageList[index].GetHeight()) / static_cast<float>(mImageList[index].GetWidth()));
    imageActor.SetSize(IMAGE_ACTOR_SIZE,IMAGE_ACTOR_SIZE*ratio);
    imageActor.SetParentOrigin(ParentOrigin::CENTER);
    imageActor.SetAnchorPoint(AnchorPoint::CENTER);
    imageActor.SetPosition(Random::Range(-0.4f*mStageSize.x, 0.4f*mStageSize.x),
                           Random::Range(-0.4f*mStageSize.y, 0.4f*mStageSize.y),
                           -5.0f*static_cast<float>(i));
    mActorRoot.Add(imageActor);

    mActorList.push_back(imageActor);
  }
}

void PerformanceTest::AddTextActors ( unsigned int number )
{
  TextActor textActor;
  unsigned int fontNumber = mFontList.size();
  for(unsigned int i = 0; i<number; i++)
  {
    TextActor textActor = TextActor::New(TEXT_DATA[(i%NUM_TEXTS)*2], mFontList[i%fontNumber]);
    textActor.SetParentOrigin(ParentOrigin::CENTER);
    textActor.SetAnchorPoint(AnchorPoint::CENTER);
    textActor.SetPosition(Random::Range(-0.4f*mStageSize.x, 0.4f*mStageSize.x),
                          Random::Range(-0.4f*mStageSize.y, 0.4f*mStageSize.y),
                          -5.0f*static_cast<float>(i));
    textActor.SetColor(Vector4(Random::Range(0.33f, 1.0f), Random::Range(0.33f, 1.0f), Random::Range(0.33f, 1.0f), 1.0f));
    mActorRoot.Add(textActor);

    mActorList.push_back(textActor);
  }
}

void PerformanceTest::AddMeshActors(unsigned int number)
{
  Vector3 shapeSize( SHAPE_SIZE, SHAPE_SIZE, SHAPE_SIZE);
  Actor actor;
  for(unsigned int i = 0; i<number; i++)
  {
    switch( i % 5 )
    {
      case 0:
      {
        actor = ModelActorFactory::BuildActorTree(mModelList[0], "Capsule");
        actor.SetSize( shapeSize );
        break;
      }
      case 1:
      {
        actor = ModelActorFactory::BuildActorTree(mModelList[0], "Cone");
        actor.SetSize( shapeSize );
        break;
      }
      case 2:
      {
        actor = ModelActorFactory::BuildActorTree(mModelList[0], "Cube");
        actor.SetSize( shapeSize );
        break;
      }
      case 3:
      {
        actor = ModelActorFactory::BuildActorTree(mModelList[0], "Cylinder");
        actor.SetSize( shapeSize );
        break;
      }
      case 4:
      default:
      {
        actor = ModelActorFactory::BuildActorTree(mModelList[0], "Sphere");
        actor.SetSize( shapeSize );
        break;
      }
    }
    actor.SetParentOrigin(ParentOrigin::CENTER);
    actor.SetAnchorPoint(AnchorPoint::CENTER);
    actor.SetPosition(Random::Range(-0.4f*mStageSize.x, 0.4f*mStageSize.x),
                      Random::Range(-0.4f*mStageSize.y, 0.4f*mStageSize.y),
                      -5.0f*static_cast<float>(i));
    actor.SetColor(Vector4(Random::Range(0.33f, 1.0f), Random::Range(0.33f, 1.0f), Random::Range(0.33f, 1.0f), 1.0f));
    mActorRoot.Add(actor);

    mActorList.push_back(actor);
  }
}

void PerformanceTest::AnimateActors()
{
  // for every actor on the stage
  for(std::vector<Actor>::const_iterator iter = mActorList.begin(),end = mActorList.end(); iter != end; iter++)
  {
    Animation animation = Animation::New(ANIMATION_TIME);
    //animate rotation
    animation.RotateBy( *iter, Degree(720.0f), Vector3( 1.0f, 1.0f, 1.0f ) );
    //animate position
    animation.MoveBy( *iter, Vector3( mStageSize.x*0.3f, mStageSize.y*0.3f, 500.0f ), AlphaFunctions::Linear, 0.0f, ANIMATION_TIME*0.5f );
    animation.MoveBy( *iter, Vector3( -mStageSize.x*0.3f, -mStageSize.y*0.3f, -500.0f ), AlphaFunctions::Linear, ANIMATION_TIME*0.5f, ANIMATION_TIME*0.5f );
    //animate size
    Vector3 size = iter->GetCurrentSize();
    animation.Resize( *iter, size*0.1f, AlphaFunctions::Linear, 0.0f, ANIMATION_TIME*0.5f);
    animation.Resize( *iter, size, AlphaFunctions::Linear, ANIMATION_TIME*0.5f, ANIMATION_TIME*0.5f);
    //animate color
    Vector4 color = iter->GetCurrentColor();
    animation.ColorTo(*iter, Vector4(1.0f,1.0f,1.0f,1.0f), AlphaFunctions::Linear, 0.0f, ANIMATION_TIME*0.5f);
    animation.ColorTo(*iter, color, AlphaFunctions::Linear, ANIMATION_TIME*0.5f, ANIMATION_TIME*0.5f);
    //start the animation
    animation.Play();
  }
}

void PerformanceTest::ConstrainActors()
{
  float offset = 0.0f;
  Actor firstActor = mActorList[0];
  // set the first actor to the bottom left corner
  firstActor.SetPosition( -mStageSize.x*0.4f, mStageSize.y*0.55f, 100.0f);
  firstActor.SetRotation( Quaternion(0.0f,Vector3::YAXIS) );
  // register a new peoperty 'LAYOUT_INDEX' which will be used to animate in the later step
  // this property affects the actor position
  mPropertyLayoutIndex = firstActor.RegisterProperty("LAYOUT_INDEX",Property::Value(0.0f));
  Constraint constraint;
  float offsetStep;
  if(mNumActors == NUM_ACTORS_MEDIUM)
  {
    offsetStep = 9.0f;
  }
  else
  {
    offsetStep = 0.9f;
  }
  for(std::vector<Actor>::iterator iter = mActorList.begin()+1,end = mActorList.end(); iter != end; iter++)
  {
    offset += offsetStep;
    // apply constraints on position, size and rotation
    constraint = Constraint::New<Vector3>( Actor::POSITION,
                                           Source( firstActor, Actor::POSITION ),
                                           Source( firstActor, mPropertyLayoutIndex),
                                           TestLayoutPosition( offset ) );
    iter->ApplyConstraint( constraint );
    constraint = Constraint::New<Vector3>( Actor::SIZE,
                                           Source(firstActor, Actor::SIZE),
                                           TestLayoutSize() );

    iter->ApplyConstraint( constraint );
    constraint = Constraint::New<Quaternion> ( Actor::ROTATION,
                                               Source( *(iter-1), Actor::ROTATION ),
                                               TestLayoutRotation() );
    iter->ApplyConstraint( constraint );
  }
}

void PerformanceTest::AnimateConstrainedActors()
{
  Actor firstActor = mActorList[0];
  Animation animation = Animation::New(ANIMATION_TIME);
  animation.SetDestroyAction(Animation::Discard);
  animation.AnimateTo(Property(firstActor, mPropertyLayoutIndex), 30.0f);
  animation.RotateBy(firstActor, Radian(Math::PI*2.0f), Vector3(1.0f,1.0f,1.0f));
  Vector3 size = firstActor.GetCurrentSize();
  animation.Resize(firstActor, size*0.1f, AlphaFunctions::Linear, 0.0f, ANIMATION_TIME*0.5f);
  animation.Resize(firstActor, size*1.0f, AlphaFunctions::Linear, ANIMATION_TIME*0.5f, ANIMATION_TIME*0.5f);
  animation.Play();
}

void PerformanceTest::RemoveActorsFromStage()
{
  for(std::vector<Actor>::iterator iter = mActorList.begin(),end = mActorList.end(); iter != end; iter++)
  {
    mActorRoot.Remove(*iter);
  }
}
void PerformanceTest::AddActorsToStage()
{
  for(std::vector<Actor>::iterator iter = mActorList.begin(),end = mActorList.end(); iter != end; iter++)
  {
    mActorRoot.Add(*iter);
  }
}

void PerformanceTest::SetActorsInvisible()
{
  for(std::vector<Actor>::iterator iter = mActorList.begin(),end = mActorList.end(); iter != end; iter++)
  {
    iter->SetVisible(false);
  }
}

void PerformanceTest::SetActorsVisible()
{
  for(std::vector<Actor>::iterator iter = mActorList.begin(),end = mActorList.end(); iter != end; iter++)
  {
    iter->SetVisible(true);
  }
}

void PerformanceTest::CleanScene()
{
  for(std::vector<Actor>::iterator iter = mActorList.begin(),end = mActorList.end(); iter != end; iter++)
  {
    iter->RemoveConstraints();
    mActorRoot.Remove(*iter);
    iter->Reset();
  }
  mActorList.clear();

  DALI_ASSERT_ALWAYS(mActorRoot.GetChildCount() == 0);
  DALI_ASSERT_ALWAYS(mActorList.size()==0);
}

void RunTest( Application& application )
{
  PerformanceTest test( application );

  application.MainLoop();
}

int main( int argc, char **argv )
{
  Application application = Application::New(&argc, &argv);
  RunTest( application );

  return 0;
}
