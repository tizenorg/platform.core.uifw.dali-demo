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
#include <getopt.h>

#include <dali/integration-api/debug.h>
#include <dali-toolkit/dali-toolkit.h>
#include "../examples/shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* QUIT_IMAGE( DALI_IMAGE_DIR "exit-button.png" );
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "bg-1024x1024.jpg" );
const char* APPLICATION_TITLE( "Text Input Performance Test" );
static const Vector4 BACKGROUND_COLOR(Vector4(1.f,1.f,1.f,0.5f));
const unsigned int HEARTBEAT_TICK(50);  // This is the main tick interval in milliseconds that causes events to occur
size_t LONG_REST_DURATION(100);  // How many ticks to wait between states
size_t SHORT_REST_DURATION(50);  // How many ticks to wait between states
const unsigned int WORD_LENGTH(5); // Length a word should be
const unsigned int MAX_KEY_EVENTS(80); // Number of key events / characters to be added
const unsigned int MAX_KEY_EVENTS_LONG(400); // Number of key events / characters to be added

const Vector3 DEFAULT_TEXT_INPUT_SIZE( Vector3( 400.0f, 300.0f, 0.0f ));
const Vector3 LARGE_TEXT_INPUT_SIZE( Vector3( 400.0f, 600.0f, 0.0f ));

}

/******************************************************************************
 * Simply fills the stage with x rotating actors.
 * To see the performance metrics like frame rate,
 * uncomment the #defines in dali/core/rendering/performance-monitor.h
 ******************************************************************************/

class TextInputPerformanceTest : public ConnectionTracker
{
public:

  TextInputPerformanceTest( Application &application )
  : mApplication( application ),
    mView(),
    mToolBar(),
    mActorRoot(),
    mInputContainer(),
    mTextInputAlpha(),
    mTimer(),
    mToggle( false ),
    mKeyEventsToAdd( MAX_KEY_EVENTS ),
    mKeyEventsToAddLong( MAX_KEY_EVENTS_LONG ),
    mState(  KeyEventsUnique ),
    mTicksToConsume( 0 ),
    mWordArray(),
    mAnimations(),
    mAnimationPlaying( false )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextInputPerformanceTest::Create );

    srand(time(NULL));
  }

  void SetUpDefaultView()
  {
    Dali::Stage stage = Dali::Stage::GetCurrent();
    mActorRoot = Actor::New();
    mActorRoot.SetParentOrigin(ParentOrigin::CENTER);
    mActorRoot.SetAnchorPoint(AnchorPoint::CENTER);
    stage.Add (mActorRoot);

    // Creates a default view for demos.
    Layer contentLayer = DemoHelper::CreateView( mApplication,
                                                 mView,
                                                 mToolBar,
                                                 BACKGROUND_IMAGE,
                                                 TOOLBAR_IMAGE,
                                                 APPLICATION_TITLE );

    // Create a quit button, connect signal and add to tooblbar
    Image image = Image::New( QUIT_IMAGE );
    Toolkit::PushButton quitButton = Toolkit::PushButton::New();
    quitButton.SetBackgroundImage( image );
    quitButton.ClickedSignal().Connect( this, &TextInputPerformanceTest::OnQuitButtonClicked );
    mToolBar.AddControl( quitButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft );

    // Add an extra space on the right to center the title text.
    mToolBar.AddControl( Actor::New(), DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalRight );
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime

    SetUpDefaultView();

    CreateData();

    CreateTextInput();

    AddResizeAndRotateAnimations();

    SetUpAndStartTimer();
  }


public:

  enum StateOfProgress
  {
    KeyEventsUnique,
    Keyevents,
    KeyEventsLong,
    Scroll,
    Animate,
    End
  };


public:

  void NextState();

private:

  bool OnQuitButtonClicked( Toolkit::Button button );
  bool OnTick();

  void ConsumeTicks( size_t ticksToConsume);

  void CreateData();
  void CreateTextInput();
  void AddResizeAndRotateAnimations();
  void SetUpAndStartTimer();
  void AddKeyEvents();
  void AddKeyEventsLong();
  void AddKeyEventsUnique();
  void ScrollText();
  Animation CreateResizeAnimation(float targetWidth, float targetHeight, float duration );
  Animation CreateRotateAnimation(float rotationAngle, Vector3 axis, float duration );
  Animation CreateMoveAnimation( Vector3 displacement, float durationSeconds  );
  void AnimationEnded( Animation& source );
  void Animations();
  void EndTest();

private:
  Application&  mApplication;
  Toolkit::View mView;
  Toolkit::ToolBar mToolBar;
  Actor mActorRoot;
  ImageActor mInputContainer;

  TextInput mTextInputAlpha;
  Timer mTimer;

  bool mToggle;
  size_t mKeyEventsToAdd;
  size_t mKeyEventsToAddLong;

  StateOfProgress mState;

  size_t mTicksToConsume;

  std::vector<std::string> mWordArray;

  std::vector<Animation> mAnimations;

  bool mAnimationPlaying;

};

/* Declares order in which tests will be executed
 * Each state specifies the state that will follow */
void TextInputPerformanceTest::NextState( )
{
  switch ( mState )
  {
  case KeyEventsUnique:
  {
    mState = KeyEventsLong;
  }
  break;
  case KeyEventsLong:
  {
    mState = Keyevents;
  }
  break;
  case Keyevents:
  {
    mState = Scroll;
  }
  break;
  case Scroll:
  {
    mState = Animate;
  }
  break;
  case Animate:
  {
     mState = End;
  }
  break;
  case End:
  default:
  {
    mState = End;
  }
  break;
  }
}

bool TextInputPerformanceTest::OnQuitButtonClicked( Toolkit::Button button )
{
  // quit the application
  mTimer.Stop();
  mApplication.Quit();
  return true;
}

/**
 * On every tick mState is checked and the relevant test function is called
 * Each test function calls NextState() when complete which updates mState.
 */
bool TextInputPerformanceTest::OnTick()
{
  /**
   * A test function can call ConsumeTicks(ticksToConsume) to wait before next test function starts.
   */
  if ( mTicksToConsume > 0 )
  {
    mTicksToConsume--;
  }
  else
  {
    switch ( mState )
    {
    case Keyevents:
    {
      AddKeyEvents();
    }
    break;
    case KeyEventsLong:
    {
      AddKeyEventsLong();
    }
    break;
    case KeyEventsUnique:
    {
      AddKeyEventsUnique();
    }
    break;
    case Scroll:
    {
      ScrollText();
    }
    break;
    case Animate:
    {
      Animations();
    }
    break;
    case End:
    default:
    {
      EndTest();
    }
    break;
    }
  }
  return true;
}

void TextInputPerformanceTest::ConsumeTicks( size_t ticksToConsume)
{
  mTicksToConsume = ticksToConsume;
}

/* Puts valid ascii characters in an Array*/
void TextInputPerformanceTest:: CreateData()
{
  for ( unsigned int i=33; i <= 126; i++ )
  {
    char asciiChar(i);
    std::string s(1, asciiChar);
    mWordArray.push_back( s );
  }
}

/* Set up a text input*/
void TextInputPerformanceTest::CreateTextInput()
{
  mInputContainer = Toolkit::CreateSolidColorActor( BACKGROUND_COLOR, true ,Color::GREEN );
  mActorRoot.Add( mInputContainer );
  mInputContainer.SetSize( DEFAULT_TEXT_INPUT_SIZE );

  mTextInputAlpha = Toolkit::TextInput::New();
  mTextInputAlpha.SetParentOrigin(ParentOrigin::TOP_LEFT);
  mTextInputAlpha.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  mTextInputAlpha.SetSize( DEFAULT_TEXT_INPUT_SIZE );
  mTextInputAlpha.SetSizePolicy( Control::Fixed, Control::Fixed );

  mTextInputAlpha.SetPosition( Vector3( 0.0f ,0.0f ,0.0 ) );

  mInputContainer.Add( mTextInputAlpha );
}

/* Add animations to be performed on text input in Animate state*/
void TextInputPerformanceTest::AddResizeAndRotateAnimations()
{
  // Animations added in reverse order so can be popped off and used.
  mAnimations.push_back( CreateMoveAnimation( Vector3( 50.0, -100.0, 0.0), 0.5f ) );
  mAnimations.push_back( CreateMoveAnimation( Vector3(-50.0, -100.0, 0.0), 0.5f ) );
  mAnimations.push_back( CreateMoveAnimation( Vector3(-50.0,  100.0, 0.0), 0.5f ) );
  mAnimations.push_back( CreateMoveAnimation( Vector3( 50.0,  100.0, 0.0), 0.5f ) );
  mAnimations.push_back( CreateRotateAnimation(0.0f, Vector3::YAXIS, 1.0f ) );
  mAnimations.push_back( CreateRotateAnimation(-30.0f, Vector3::YAXIS, 2.0f ) );
  mAnimations.push_back( CreateRotateAnimation(30.0f, Vector3::YAXIS, 1.0f ) );
  mAnimations.push_back( CreateRotateAnimation(0.0f, Vector3::ZAXIS, 2.0f ) );
  mAnimations.push_back( CreateRotateAnimation(90.0f, Vector3::ZAXIS, 2.0f ) );
  mAnimations.push_back( CreateResizeAnimation( 400.0f, 300.0f, 1.5f ) );
  mAnimations.push_back( CreateResizeAnimation( 200.0f, 100.0f, 1.5f ) );
  mAnimations.push_back( CreateResizeAnimation( 480.0f, 500.0f, 1.5f ) );  // First animation to play.
}

void TextInputPerformanceTest::SetUpAndStartTimer()
{
  mTimer = Timer::New( HEARTBEAT_TICK );
  mTimer.TickSignal().Connect( this, &TextInputPerformanceTest::OnTick );
  ConsumeTicks( SHORT_REST_DURATION ); // consume some ticks so stage can be set up.
  mTimer.Start();
}

void TextInputPerformanceTest::AddKeyEvents()
{
  // Ensure text input is ready to receive input.
  if ( mTextInputAlpha.IsEditable() == false )
  {
    mTextInputAlpha.SetEditable( true );
    VirtualKeyboard::Hide();
  }

  // Starts edit mode
  if ( mKeyEventsToAdd > 0)
  {
    Dali::KeyEvent event( "commit", "brmmm ", 0, 0, 0, Dali::KeyEvent::Down ); // adds a average length word to text input many times
    Dali::EventFeeder::FeedKeyEvent( event );
    mKeyEventsToAdd--;
  }
  else
  {
    mTextInputAlpha.SetEditable( false );
    mKeyEventsToAdd = MAX_KEY_EVENTS;
    ConsumeTicks( LONG_REST_DURATION ); // wait
    NextState();
  }
}

void TextInputPerformanceTest::AddKeyEventsLong()
{
  // Ensure text input is ready to receive input.
  if ( mTextInputAlpha.IsEditable() == false )
  {
    mTextInputAlpha.SetEditable( true );
    VirtualKeyboard::Hide();
  }

  // Starts edit mode
  if ( ( mKeyEventsToAddLong > 0 ) && ( !mWordArray.empty() ))
  {
    if ( ( mKeyEventsToAddLong % WORD_LENGTH ) == 0 )
    {
      Dali::KeyEvent event( " ", "  ", 0, 0, 0, Dali::KeyEvent::Down );
      EventFeeder::FeedKeyEvent( event );
    }
    else
    {
      Dali::KeyEvent event( "A", "A", 0, 0, 0, Dali::KeyEvent::Down );
      EventFeeder::FeedKeyEvent( event );
    }

    mKeyEventsToAddLong--;
  }
  else
  {
    mTextInputAlpha.SetEditable( false );
    mTextInputAlpha.SetInitialText(""); // clear text input.
    mInputContainer.SetSize( DEFAULT_TEXT_INPUT_SIZE );
    ConsumeTicks( LONG_REST_DURATION ); // wait
    NextState();
  }
}

void TextInputPerformanceTest::AddKeyEventsUnique()
{
  // Ensure text input is ready to receive input.
  if ( mTextInputAlpha.IsEditable() == false )
  {
    mTextInputAlpha.SetEditable( true );
    VirtualKeyboard::Hide();
  }

  // Starts edit mode
  unsigned int wordArraySize = mWordArray.size();
  if (  wordArraySize > 0 )
  {

    if ( ( wordArraySize % WORD_LENGTH ) == 0 )
    {
      Dali::KeyEvent event( " ", " ", 0, 0, 0, Dali::KeyEvent::Down );
      EventFeeder::FeedKeyEvent( event );
    }

    std::string stringToAdd( mWordArray.back());
    mWordArray.pop_back();

    // Currently passes only 1 character at a time but as performance improves this can be put in a for loop for 5 cycles or more.
    Dali::KeyEvent event( stringToAdd, stringToAdd, 0, 0, 0, Dali::KeyEvent::Down );
    EventFeeder::FeedKeyEvent( event );
  }
  else
  {
    mTextInputAlpha.SetEditable( false );
    CreateData();  // Re-create data that was popped.
    mTextInputAlpha.SetInitialText(""); // clear text input.
    mInputContainer.SetSize( LARGE_TEXT_INPUT_SIZE );
    ConsumeTicks( LONG_REST_DURATION ); // wait
    NextState();
  }
}

void TextInputPerformanceTest::ScrollText()
{
  // currently unsupported in TextInput;
  NextState();
}

// Animation Creation functions

Animation TextInputPerformanceTest::CreateResizeAnimation(float targetWidth, float targetHeight, float duration )
{
  Animation animation = Animation::New( duration );
  animation.Resize( mInputContainer, targetWidth, targetHeight );

  return animation;
}

Animation TextInputPerformanceTest::CreateRotateAnimation(float rotationAngle, Vector3 axis, float duration )
{
  Animation rotateAnimation = Animation::New( duration );
  rotateAnimation.RotateTo(mInputContainer, Degree( rotationAngle ), axis );

  return rotateAnimation;
}

Animation TextInputPerformanceTest::CreateMoveAnimation( Vector3 displacement, float durationSeconds  )
{
  Animation moveAnimation = Animation::New( durationSeconds );
  moveAnimation.MoveBy(mInputContainer, displacement, AlphaFunctions::Linear );

  return moveAnimation;
}

/*
 * Callback for animation finished signal
 */
void TextInputPerformanceTest::AnimationEnded( Animation& source )
{
  mAnimationPlaying = false;
  Animations();
}

void TextInputPerformanceTest::Animations()
{
  if ( !mAnimationPlaying ) // Ensure no other animations are still running before starting next.
  {
    if ( !mAnimations.empty() )
    {
      mAnimations.back().FinishedSignal().Connect( this, &TextInputPerformanceTest::AnimationEnded );
      mAnimations.back().Play();
      mAnimations.pop_back();
      mAnimationPlaying = true;
    }
    else
    {
      ConsumeTicks( LONG_REST_DURATION ); // wait
      NextState();
    }
  }
}

void TextInputPerformanceTest::EndTest()
{
  mTimer.Stop();
  mApplication.Quit();
}

void RunTest( Application& application )
{
  TextInputPerformanceTest test( application );

  application.MainLoop();
}

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
