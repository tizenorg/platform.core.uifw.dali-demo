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

// Uncomment to take input from X
//#define X11_DIRECT

// If not using X: Uncomment to use ScrollView demo instead of Touch events on root demo
#define SCROLLVIEW_DEMO

// EXTERNAL INCLUDES
#ifdef X11_DIRECT
#include <X11/Xresource.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XI2.h>
#include <X11/XF86keysym.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/utsname.h>
#include <algorithm>
#else
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
using namespace Dali;
using namespace Dali::Toolkit;
#endif

#include <iostream>

using namespace std;

#ifdef X11_DIRECT
static unsigned int GetCurrentMilliSeconds( void )
{
  struct timeval tv;
  struct timespec tp;
  static clockid_t clockid;
  if( !clockid )
  {
#ifdef CLOCK_MONOTONIC_COARSE
    if( clock_getres(CLOCK_MONOTONIC_COARSE, &tp ) == 0 &&
      ( tp.tv_nsec / 1000) <= 1000 && clock_gettime( CLOCK_MONOTONIC_COARSE, &tp ) == 0)
    {
      clockid = CLOCK_MONOTONIC_COARSE;
    }
    else
#endif
    if( clock_gettime( CLOCK_MONOTONIC, &tp ) == 0 )
    {
      clockid = CLOCK_MONOTONIC;
    }
    else
    {
      clockid = ~0L;
    }
  }
  if( clockid != ~0L && clock_gettime( clockid, &tp ) == 0 )
  {
    return ( tp.tv_sec * 1000 ) + ( tp.tv_nsec / 1000000L );
  }
  gettimeofday(&tv, NULL);
  return ( tv.tv_sec * 1000 ) + ( tv.tv_usec / 1000 );
}

void GetTouchDevices( Display* display, std::vector<int>& touchDevices )
{
  int numberOfDevices = 0;
  XIDeviceInfo *info = XIQueryDevice( display, XIAllDevices, &numberOfDevices );
  XIDeviceInfo *device = info;

  for( int i = 0; i < numberOfDevices; ++i, ++device )
  {
    switch( device->use )
    {
      case XIMasterPointer:
      {
        std::cout << "xtest: Touch Input Device: Using XIMasterPointer Device: " << device->name << ", id: " << device->deviceid << std::endl;
        touchDevices.push_back( device->deviceid );
        break;
      }
      case XISlavePointer:
      {
        // Check to see whether we are already hooked up to this device through a master
        // device that we may have added previously
        std::vector<int>::iterator iterator = std::find( touchDevices.begin(), touchDevices.end(), device->attachment );
        // Add if we have not
        if( iterator == touchDevices.end() )
        {
          std::cout << "xtest: Touch Input Device: Using XISlavePointer Device: " << device->name << ", id: " << device->deviceid << std::endl;
          touchDevices.push_back( device->deviceid );
        }
        break;
      }
      case XIFloatingSlave:
      {
        // Slaves can be any type, we are only interested in XIButtonClass types
        if( ( *( device->classes ) )->type == XIButtonClass )
        {
          // Check to see whether we are already hooked up to this device through a master
          // device that we may have added previously
          std::vector<int>::iterator iterator = std::find( touchDevices.begin(), touchDevices.end(), device->attachment );
          // Add if we have not
          if( iterator == touchDevices.end() )
          {
            std::cout << "xtest: Touch Input Device: Using XIFloatingSlave Device: " << device->name << ", id: " << device->deviceid << std::endl;
            touchDevices.push_back( device->deviceid );
          }
        }
        break;
      }
      default:
      {
        // Do Nothing
        break;
      }
    }
  }

  XIFreeDeviceInfo( info );
}

void ProcessEventX2Event( XGenericEventCookie* cookie )
{
  XIDeviceEvent* deviceEvent = static_cast< XIDeviceEvent* >( cookie->data );

  bool requiresProcessing = true; //PreProcessEvent( deviceEvent );//TODO: for now, force
  if( !requiresProcessing )
  {
    return;
  }
  Time time( deviceEvent->time ); // X is using uint32 for time field ( see XI2proto.h )

  switch( cookie->evtype )
  {
    case XI_TouchUpdate:
    case XI_Motion:
    {
      unsigned int msNow = GetCurrentMilliSeconds();
      int msDiff = msNow - time;
      //if( msDiff > 8 )
      if( 1 )
      {
        std::cout << "xtest:ProcessEventX2: XI_Motion: " << deviceEvent->event_x << "," << deviceEvent->event_y << "," << time << "," << msDiff << std::endl;
      }
      break;
    }
    case XI_TouchBegin:
    case XI_ButtonPress:
    {
      std::cout << "xtest:ProcessEventX2: XI_TouchBegin" << std::endl;
      break;
    }
    case XI_TouchEnd:
    case XI_ButtonRelease:
    {
      std::cout << "xtest:ProcessEventX2: XI_TouchEnd" << std::endl;
      break;
    }
    case XI_FocusIn:
    {
      std::cout << "xtest:ProcessEventX2: XI_FocusIn" << std::endl;
      break;
    }
    case XI_FocusOut:
    {
      std::cout << "xtest:ProcessEventX2: XI_FocusOut" << std::endl;
      break;
    }
    case XI_KeyPress:
    {
      std::cout << "xtest:ProcessEventX2: XI_KeyPress" << std::endl;
      break;
    }
    default:
    {
      break;
    }
  }
}

#else // Use DALi instead of X

#ifdef SCROLLVIEW_DEMO // DALi demo type: Basic ScrollView

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
    m_MainLayer.SetParentOrigin(ParentOrigin::TOP_LEFT);
    m_MainLayer.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    m_MainLayer.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    Stage::GetCurrent().Add(m_MainLayer);

    Dali::Toolkit::ScrollView m_ScrollView = Dali::Toolkit::ScrollView::New();
    m_ScrollView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    m_ScrollView.SetParentOrigin( ParentOrigin::TOP_LEFT );
    m_ScrollView.SetResizePolicy(ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS);
    m_ScrollView.SetPosition(0.0f, 0.0f, 0.0f);
    m_ScrollView.SetWrapMode(false);


    Dali::Toolkit::FixedRuler* m_FixedRuler = new Dali::Toolkit::FixedRuler( stageSize.width);
    m_FixedRuler->SetDomain(Dali::Toolkit::RulerDomain(0.0f, stageSize.width*3, false));

    Dali::Toolkit::RulerPtr disabledRuler = new Dali::Toolkit::DefaultRuler();
    disabledRuler->Disable();


    m_ScrollView.SetRulerX(m_FixedRuler );
    m_ScrollView.SetRulerY(disabledRuler);
    m_ScrollView.SetWrapMode(true);
    //m_ScrollView.SetOvershootEnabled(true);


    Actor mScrollViewContainer = Actor::New();
    mScrollViewContainer.SetName("ScrollViewContainer");
    mScrollViewContainer.SetParentOrigin(ParentOrigin::TOP_CENTER);
    mScrollViewContainer.SetAnchorPoint(AnchorPoint::TOP_CENTER);
    mScrollViewContainer.SetSize(Vector2( stageSize.width*4,stageSize.height));

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


    m_ScrollView.Add( mScrollViewContainer );
    m_MainLayer.Add(m_ScrollView);

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

#else // DALi demo type: Touch events on root

class HelloWorldController : public ConnectionTracker
{
public:
  HelloWorldController( Application& application ): mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  void Create( Application& application )
  {
    std::cout << "HelloWorldController::Create" << std::endl;
    Stage::GetCurrent().GetRootLayer().TouchedSignal().Connect( this, &HelloWorldController::OnTouch );
    Stage::GetCurrent().KeyEventSignal().Connect(this, &HelloWorldController::OnKeyEvent);

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    TextLabel textLabel = TextLabel::New( "Hello World" );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textLabel.SetName( "hello-world-label" );
    stage.Add( textLabel );
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    return true;
  }

private:
  Application&  mApplication;
};
#endif // DALi demo type

#endif // Use DALi instead of X

int main( int argc, char **argv )
{
#ifdef X11_DIRECT
  Display* display = XOpenDisplay( NULL );
  if( display == NULL )
  {
    fprintf( stderr, "Cannot open display\n" );
    exit( 1 );
  }

  int screen = DefaultScreen( display );
  XID window = XCreateSimpleWindow( display, RootWindow( display, screen ), 0, 0, 720, 1280, 0, BlackPixel( display, screen ), WhitePixel( display, screen ) );

  // Select normal input.
  XSelectInput( display, window, ExposureMask | KeyPressMask );

  // We need XI (X2) to get touch events.
  int extensionId = 0, extensionEvent = 0, extensionError = 0;
  if( !XQueryExtension( display, "XInputExtension", &extensionId, &extensionEvent, &extensionError ) )
  {
    fprintf( stderr, "XInputExtension not supported\n" );
  }

  std::vector<int> touchDevices;
  GetTouchDevices( display, touchDevices );

  unsigned char mask[1] = { 0 };
  XISetMask( mask, XI_ButtonPress );
  XISetMask( mask, XI_ButtonRelease );
  XISetMask( mask, XI_Motion );
  int numberOfDevices = touchDevices.size();
  XIEventMask *eventMasks = new XIEventMask[numberOfDevices];
  XIEventMask *eventMasksPointer = eventMasks;
  for( std::vector<int>::iterator iterator = touchDevices.begin(); iterator != touchDevices.end(); ++iterator, ++eventMasksPointer )
  {
    eventMasksPointer->deviceid = *iterator;
    eventMasksPointer->mask_len = sizeof( mask );
    eventMasksPointer->mask = mask;
  }
  XISelectEvents( display, window, eventMasks, numberOfDevices );
  XFlush( display );

  // Continue window setup.
  XMapWindow( display, window );

  Atom WM_DELETE_WINDOW = XInternAtom( display, "WM_DELETE_WINDOW", False );
  XSetWMProtocols( display, window, &WM_DELETE_WINDOW, 1 );

  std::cout << "xtest:Running  Press back-button to exit" << std::endl;

  // Main loop.
  bool running = true;
  XEvent event;
  while( running )
  {
    // Get an X event.
    XNextEvent( display, &event );

    // Handle generic X events.
    if( event.type == KeyPress )
    {
      char buf[128] = { 0 };
      KeySym keySym;
      XLookupString( &event.xkey, buf, sizeof( buf ), &keySym, NULL );
      std::cout << "xtest:KeyPress: " << keySym << std::endl;
      // Exit on "Back" button.
      if( keySym == XF86XK_Back )
      {
        running = false;
      }
    }
    //else if( event.type == MotionNotify )
    //{
      //unsigned int curtime = GetCurrentMilliSeconds();
      //XMotionEvent * evt = (XMotionEvent*)&event;
      //cout << "x11 direct x:" << evt->x << " y:" << evt->y 
        //<< " event-time:" << evt->time << " current-time:" << curtime
        //<< " time-diff:" << curtime - evt->time << endl;
    //}
    else
    {
      // Handle XI2 events.
      // Cookie data pointer is undefined until XGetEventData is called.
      XGenericEventCookie* cookie = &event.xcookie;

      if( XGetEventData( display, cookie ) )
      {
        if( cookie->extension == extensionId )
        {
          ProcessEventX2Event( cookie );
        }
        else
        {
          std::cout << "xtest:Not an XI2 event: " << event.type << std::endl;
        }
        XFreeEventData( display, cookie );
      }
    }
  }

  // Clean up.
  XDestroyWindow( display, window );
  XCloseDisplay( display );
#else
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);
  HelloWorldController test( app );
  app.MainLoop();
#endif
}
