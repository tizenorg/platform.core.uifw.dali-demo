#include "shared/view.h"
#include <fstream>
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include <dali-toolkit/devel-api/builder/json-parser.h>

#include <iostream>
#include <sys/time.h>
#include <dali/integration-api/debug.h>

#define TOKEN_STRING(x) #x

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

std::string GetFileContents(const std::string &fn)
{
  std::ifstream t(fn.c_str());
  return std::string((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
}

unsigned long long getMS()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  unsigned long long millisecondsSinceEpoch =
    (unsigned long long)(tv.tv_sec) * 1000 +
    (unsigned long long)(tv.tv_usec) / 1000;
  return millisecondsSinceEpoch;
}

class HelloWorldController : public ConnectionTracker
{
  public:

    HelloWorldController( Application& application )
      : mApplication( application )
    {
      mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
    }

    ~HelloWorldController()
    {
      // Nothing to do here;
    }

    void Create( Application& application )
    {
      cout << endl << "(time-logger)Begin Create: " << getMS() << endl;
      DALI_LOG_ERROR("(time-logger)Begin Create: %llu  ", getMS());

      Stage stage = Stage::GetCurrent();

      Vector2 stageSize = stage.GetSize();
      Actor actor = Actor::New();
      actor.SetSize( 400,600,.0f );
      actor.SetParentOrigin( ParentOrigin::CENTER );
      actor.SetKeyboardFocusable( false );
      actor.SetScale( stageSize.x / 400., stageSize.y / 600., 1.0f);
      stage.Add( actor );

      stage.GetRootLayer().SetBehavior(Layer::LAYER_3D);
      Builder builder = Builder::New();

      Property::Map defaultDirs;
      defaultDirs[ TOKEN_STRING(DALI_IMAGE_DIR) ]  = DALI_IMAGE_DIR;
      defaultDirs[ TOKEN_STRING(DALI_MODEL_DIR) ]  = DALI_MODEL_DIR;
      defaultDirs[ TOKEN_STRING(DALI_SCRIPT_DIR) ] = DALI_SCRIPT_DIR;

      builder.AddConstants( defaultDirs );
		    
      std::string json_data( GetFileContents( DALI_SCRIPT_DIR "project.json" ) );
      builder.LoadFromString(json_data);
      builder.AddActors( actor );


      cout << endl << "(time-logger)End__ Create: " << getMS() << endl;
      DALI_LOG_ERROR("(time-logger)End__ Create: %llu  ", getMS());
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
