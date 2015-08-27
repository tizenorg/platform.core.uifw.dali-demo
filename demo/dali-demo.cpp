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

#define DEMO_TESTALGO
//#define DEMO_SCROLLVIEW
//#define DEMO_X11DIRECT
//#define DEMO_TOUCHROOT


#ifdef DEMO_TESTALGO
#include <stdio.h>
#include <set>
#include <limits>
#include <iostream>
#include <vector>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
//todor
#include <sys/time.h>

//#include <dali/public-api/events/gesture.h>
//#include "../shared/view.h"

//#define TEST_DRAW_PIXEL_ONLY

static unsigned int GetMs( void )
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


//todor
// ####################################################################################################

enum
{
  TEST_PREDICTION_CODE_NONE,
  TEST_PREDICTION_CODE_DALI_DEFAULT,
  TEST_PREDICTION_CODE_TEST_1,
};

static bool               TEST_SIMULATE_INPUT_STARVATION_AT_ZERO_VELOCITY = false;
static const unsigned int DEFAULT_TEST_PREDICTION_CODE = TEST_PREDICTION_CODE_TEST_1;
//static const unsigned int DEFAULT_TEST_PREDICTION_CODE = TEST_PREDICTION_CODE_DALI_DEFAULT;
static const unsigned int TEST_COMPARE_PAST_PREDICTION_TIME =        45u;
static const unsigned int TEST_COMPARE_PAST_PREDICTION_TIME_WINDOW = 13u;
static const unsigned int PAN_GESTURE_HISTORY =                      30u;
#define TEST_OPTS_USELOCAL
#define TEST_OPTS_90HZ_INPUT

// ####################################################################################################

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

struct Info
{
  Vector2 velocity;
  Vector2 displacement;
  Vector2 position;
};
struct PanInfo
{
  unsigned int time;
  Gesture::State state;
  Info local;
  Info screen;
  volatile bool read;
  PanInfo()
  : time( 0u ),
    state( Gesture::Clear ),
    read( true )
  {
  }
};
typedef std::vector<PanInfo> PanInfoHistory;
typedef PanInfoHistory::iterator PanInfoHistoryIter;
typedef PanInfoHistory::reverse_iterator PanInfoHistoryReverseIter;
typedef PanInfoHistory::const_iterator PanInfoHistoryConstIter;
typedef std::vector<TouchEvent> GestureStore;

enum PredictionMode
{
  PREDICTION_NONE = 0,
  PREDICTION_1
};
enum SmoothingMode
{
  SMOOTHING_NONE,           // no smoothing
  SMOOTHING_LAST_VALUE,     // smooth between last value and latest value
};

#if 0
// Emulate gesture state enum.
namespace Dali
{
namespace Gesture
{
enum State
{
  Clear,      ///< There is no state associated with this gesture.
  Started,    ///< The touched points on the screen have moved enough to be considered a gesture.
  Continuing, ///< The gesture is continuing.
  Finished,   ///< The user has lifted a finger or touched an additional point on the screen.
  Cancelled,  ///< The gesture has been cancelled.
  Possible    ///< A gesture is possible.
};
}
}
#endif

namespace
{
#if 0
const char* BACKGROUND_IMAGE( DALI_IMAGE_DIR "desktop_background.png" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* QUIT_IMAGE( DALI_IMAGE_DIR "exit-button.png" );
const char* APPLICATION_TITLE( "Drawing Example" );
const char* CLEAR_IMAGE( DALI_IMAGE_DIR "view-swap.png" );
#endif
/**
 * HistoryPair
 * represents a key-value element in the HistoryContainer
 */
template<class T>
struct HistoryPairType
{
public:
  HistoryPairType(float firstValue)
  : first(firstValue)
  {
  }
  HistoryPairType(float firstValue, T secondValue)
  : first(firstValue),
    second(secondValue)
  {
  }
  bool operator<(const HistoryPairType& rhs) const
  {
    return first < rhs.first;
  }
  float first;
  T second;
};
/**
 * History container.
 * This container is used for keeping a list of element pairs while
 * providing an API that can generate interpolated values of requested
 * elements that lie between two stored elements.
 *
 * e.g. stored values:
 *
 * 1.0 - 10
 * 2.0 - 30
 * 3.0 - 50
 *
 * Requesting value at key 1.5 will use the adjacent stored keys (1.0 and 2.0) to
 * return an interpolated value of 20.0 (i.e. 0.5 of the way between 10 and 30).
 *
 * Requesting value at key 2.9 will use the adjacent stored keys (2.0 and 3.0) to
 * return an interpolated value of 48.0 (i.e. 0.9 of the way between 30 and 50)
 */
template<class T>
class History
{
  typedef HistoryPairType<T> HistoryPair;
  typedef std::set<HistoryPair> HistoryContainer;
  typedef typename HistoryContainer::iterator HistoryContainerIter;

public:

  /**
   * History constructor
   */
  History()
  : mMaxSize(std::numeric_limits<size_t>::max())
  {
  }

  /**
   * History destructor
   */
  ~History()
  {
  }

  /**
   * Sets the maximum size of the history container
   * in terms of elements stored, default is no limit.
   * @param[in] maxSize The maximum number of elements stored in container
   */
  void SetMaxSize(size_t maxSize)
  {
    mMaxSize = maxSize;

    if(mHistory.size() > mMaxSize)
    {
      // determine reduction in history size, and remove these elements
      size_t reduction = mHistory.size() - mMaxSize;

      while(reduction--)
      {
        mHistory.erase(mHistory.begin() );
      }
    }
  }

  void Clear()
  {
    mHistory.clear();
  }

  /**
   * Adds an element (y) to the container at position (x)
   * @param[in] x Key position value to add
   * @param[in] y Value to add at Key.
   */
  void Add(float x, const T& y)
  {
    if(mHistory.size() >= mMaxSize)
    {
      RemoveTail();
    }

    mHistory.insert(HistoryPair(x,y));
  }

  /**
   * Removes first element in the container
   */
  void RemoveTail()
  {
    mHistory.erase(mHistory.begin());
  }

  /**
   * Retrieves value from the history using key (x).
   * If the requested key (x) lies between two
   * points, a linearly interpolated value between
   * the two points is returned.
   * @param[in] x Key position to retrieve
   * @return The interpolated Value is returned for this position.
   */
  T Get(float x)
  {
    HistoryContainerIter i = mHistory.lower_bound(x);

    if(i == mHistory.end())
    {
      --i;
    }

    // For samples based on first point, just return position.
    if(i == mHistory.begin())
    {
      return i->second;
    }

    // within begin() ... end() range
    float x2 = i->first;
    T y2 = i->second;

    --i;
    float x1 = i->first;
    T y1 = i->second;

    // For samples based on first 2 points, just use linear interpolation
    // TODO: Should really perform quadratic interpolation whenever there are 3+
    // points.
    if(i == mHistory.begin())
    {
      return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
    }

    // For samples based elsewhere, always use quadratic interpolation.
    --i;
    float x0 = i->first;
    T y0 = i->second;

    // Quadratic equation:

    // y = ax^2 + bx + c
    // by making touches relative to x0, y0 (i.e. x0 = 0, y0 = 0)
    // we get c = 0, and equation becomes:
    // y = ax^2 + bx
    // 1) Y1 = a . X1^2 + b X1
    // 2) Y2 = a . X2^2 + b X2
    // solving simulatenous equations gets:

    // make time (x) & position (y) relative to x0, y0
    y1 -= y0;
    y2 -= y0;
    x1 -= x0;
    x2 -= x0;
    x -= x0;

    T a = ( y1 - (y2 * x1) / x2 ) / (x1 * (x1 - x2) );
    T b = ( y1 / x1 ) - (a * x1);

    //printf("%f:%f [p1: %f,%f p2: %f,%f\n", a.x, b.x, x1, y1.x, x2, y2.x);

    return a * x * x + b * x + y0;
  }

  /**
   * Retrieves a value from the history relative to the
   * head.
   * @note If the Keys (x) in the history decrease in value the further
   * back you go. Then a -ve deltaX value should be supplied to refer
   * to these keys relative to the head key.
   * @param[in] deltaX Key position to retrieve relative to head key
   * @return The interpolated Value is returned for this relative position.
   */
  T GetRelativeToHead(float deltaX)
  {
    HistoryContainerIter i = mHistory.end();
    --i;
    return Get(i->first + deltaX);
  }

private:

HistoryContainer mHistory;                ///< History container
size_t mMaxSize;                          ///< Current maximum size of container

};

/**
 * Convert a Vector4 color (R,G,B,A) into
 * a 32-bit unsigned int RGBA (R=MSB, A=LSB)
 * CP.TODO: move to Dali-core
 * @param[in] color Vector4 representation of color
 * @return 32-bit unsigned int RGBA representation of color
 */
unsigned int ColorVector4ToRGBA(const Vector4& color)
{
  return static_cast<int>(color.r * 255.0f) |
        (static_cast<int>(color.g * 255.0f) << 8) |
        (static_cast<int>(color.b * 255.0f) << 16) |
        (static_cast<int>(color.a * 255.0f) << 24);
}
const unsigned int RGBA_BLACK(ColorVector4ToRGBA(Color::BLACK));
}

// This example shows how to create and display Hello World! using a simple TextActor
//
class ExampleController : public ConnectionTracker
{
private:

  // ####################################################################################################
  // ####################################################################################################
  // ####################################################################################################
  // ####################################################################################################

  PanInfo PredictionCodeNone( unsigned int lastVSyncTime, unsigned int nextVSyncTime )
  {
    PanInfo info;
    info.local.position = mTouchPositionHistory.Get( ( nextVSyncTime - mTouchStartTime ) + 55 );
    info.screen.position = info.local.position;
    return info;
  }

  // ####################################################################################################

  // NOTE: Defines used by TEST_PREDICTION_CODE_DALI_DEFAULT:
  struct FrameGestureInfo
  {
    PanInfo frameGesture;
    float acceleration;
    unsigned int eventsThisFrame;
    bool justStarted;
    bool justFinished;

    FrameGestureInfo()
    : acceleration( 0.0f ),
      eventsThisFrame( 0 ),
      justStarted( false ),
      justFinished( false )
    {
    }
  };
  static const int MAX_GESTURE_AGE = 200;                                    ///< maximum age of a gesture before disallowing its use in algorithm TODO: Possibly make this configurable.
  static const float ACCELERATION_THRESHOLD = 0.1f;                          ///< minimum pan velocity change to trigger dynamic change of prediction amount
  static const unsigned int DEFAULT_PREDICTION_INTERPOLATION = 5;            ///< how much to interpolate pan position and displacement from last vsync time (in milliseconds)
  static const unsigned int DEFAULT_MAX_PREDICTION_INTERPOLATION = 32;       ///< the upper bound of the range to clamp the prediction interpolation
  static const unsigned int DEFAULT_MIN_PREDICTION_INTERPOLATION = 0;        ///< the lower bound of the range to clamp the prediction interpolation
  static const unsigned int DEFAULT_PREDICTION_INTERPOLATION_ADJUSTMENT = 2; ///< the amount of prediction interpolation to adjust (in milliseconds) each time when pan velocity changes
  static const float DEFAULT_SMOOTHING_AMOUNT = 0.25f;                       ///< how much to interpolate pan position and displacement from last vsync time
  // NOTE: TEST_PREDICTION_CODE_DALI_DEFAULT:
  PanInfo PredictionCodeDaliDefault( unsigned int lastVSyncTime, unsigned int nextVSyncTime )
  {
    //std::cout << "todordel: gh1" << std::endl;
    PanInfo TEST_finalResult;
    if( !mInGesture )
    {
      // clear current pan history
      mPanHistory.clear();
      //todor notused mPredictionHistory.clear();
    }

    FrameGestureInfo frameInfo;
    bool updateProperties = false;

    // Read input data.
    // If we are using a form of prediction, read all the input as-is.
    if( mPredictionMode != PREDICTION_NONE )
    {
      // Read input required for prediction algorithms.
      //updateProperties = ReadGestures( frameInfo, lastVSyncTime );
      // NOTE: WAS: bool PanGesture::ReadGestures( FrameGestureInfo& info, unsigned int currentTimestamp )
      FrameGestureInfo info;
      unsigned int previousReadPosition = 0;
      bool eventFound = false;
      info.frameGesture = mLastUnmodifiedGesture;

      while( mReadPosition != mWritePosition )
      {
        // Copy the gesture first
        PanInfo currentGesture( mGestures[mReadPosition] );

        //std::cout << "todordel: gh2" << std::endl;
        //if( mProfiling )
        //{
        //  mProfiling->mRawData.push_back( PanGestureProfiling::Position( currentGesture.time, currentGesture.screen.position, currentGesture.screen.displacement, currentGesture.screen.velocity, currentGesture.state ) );
        //}
        info.frameGesture.local.position = currentGesture.local.position;
        info.frameGesture.local.velocity = currentGesture.local.velocity;
        info.frameGesture.screen.position = currentGesture.screen.position;
        info.frameGesture.screen.velocity = currentGesture.screen.velocity;

        if( info.eventsThisFrame > 0 )
        {
          info.acceleration = currentGesture.screen.velocity.Length() - mGestures[previousReadPosition].screen.velocity.Length();
        }

        if( !eventFound )
        {
          info.frameGesture.local.displacement = currentGesture.local.displacement;
          info.frameGesture.screen.displacement = currentGesture.screen.displacement;
          eventFound = true;
        }
        else
        {
          info.frameGesture.local.displacement += currentGesture.local.displacement;
          info.frameGesture.screen.displacement += currentGesture.screen.displacement;
        }
        info.frameGesture.time = currentGesture.time;

        // add event to history
        mPanHistory.push_back( currentGesture );
        if( currentGesture.state == Gesture::Started )
        {
          info.justStarted = true;
          // clear just finished as we have started new pan
          info.justFinished = false;
        }
        info.justFinished |= ( currentGesture.state == Gesture::Finished || currentGesture.state == Gesture::Cancelled );

        // Update our read position.
        previousReadPosition = mReadPosition;
        ++info.eventsThisFrame;
        ++mReadPosition;
        mReadPosition %= PAN_GESTURE_HISTORY;
      }
      // This code does not determine if the data will be used.
      //return false;
      frameInfo = info;
    }
    else
    {
      // NOTE:WE FORCE PREDICTION ON IN THIS TEST APP.
      // Read and resample input.
      //updateProperties = ReadAndResampleGestures( frameInfo, lastVSyncTime );
    }

    //std::cout << "todordel: gh3" << std::endl;
    PanInfo frameGesture = frameInfo.frameGesture;
    PanInfo unmodifiedGesture = frameGesture;

    // Process input data.
    mInGesture |= frameInfo.justStarted;
    if( mInGesture )
    {
      // Profiling.
      //if( mProfiling )
      //{
      //  mProfiling->mLatestData.push_back( PanGestureProfiling::Position( lastVSyncTime, frameGesture.screen.position,
      //      frameGesture.screen.displacement, frameGesture.screen.velocity, frameGesture.state ) );
      //}

      // Perform prediction.
      switch( mPredictionMode )
      {
        case PREDICTION_NONE:
        {
          break;
        }
        case PREDICTION_1:
        {
          // Dynamically change the prediction amount according to the pan velocity acceleration.
          if( !frameInfo.justStarted )
          {
            if( frameInfo.eventsThisFrame <= 1 )
            {
              frameInfo.acceleration = frameGesture.screen.velocity.Length() - mLastUnmodifiedGesture.screen.velocity.Length();
            }

            // Ignore tiny velocity fluctuation to avoid unnecessary prediction amount change
            if( fabsf( frameInfo.acceleration ) > ACCELERATION_THRESHOLD )
            {
              mCurrentPredictionAmount += mPredictionAmountAdjustment * ( frameInfo.acceleration > Math::MACHINE_EPSILON_0 ? 1.0f : -1.0f );
              if( mCurrentPredictionAmount > mMaxPredictionAmount + mPredictionAmountAdjustment ) // Guard against unsigned int overflow
              {
                mCurrentPredictionAmount = 0;
              }
            }
          }
          else
          {
            mCurrentPredictionAmount = mPredictionAmount; // Reset the prediction amount for each new gesture
          }

          //std::cout << "todordel: gh4" << std::endl;
          mCurrentPredictionAmount = std::max( mMinPredictionAmount, std::min( mCurrentPredictionAmount, mMaxPredictionAmount ) );

          // Calculate the delta of positions before the prediction
          Vector2 deltaPosition = frameGesture.screen.position - mLastUnmodifiedGesture.screen.position;

          // Make latest gesture equal to current gesture before interpolation
          PredictiveAlgorithm1( frameInfo.eventsThisFrame, frameGesture, mPanHistory, lastVSyncTime, nextVSyncTime );

          // Calculate the delta of positions after the prediction.
          Vector2 deltaPredictedPosition = frameGesture.screen.position - mLastGesture.screen.position;

          // If the change in the prediction has a different sign than the change in the actual position,
          // there is overshot (i.e. the current prediction is too large). Return the previous prediction
          // to give the user's finger a chance to catch up with where we have panned to.
          bool overshotXAxis = false;
          bool overshotYAxis = false;
          if( (deltaPosition.x > Math::MACHINE_EPSILON_0 && deltaPredictedPosition.x < Math::MACHINE_EPSILON_0 )
           || (deltaPosition.x < Math::MACHINE_EPSILON_0 && deltaPredictedPosition.x > Math::MACHINE_EPSILON_0 ) )
          {
            overshotXAxis = true;
            frameGesture.screen.position.x = mLastGesture.screen.position.x;
          }

          if( (deltaPosition.y > Math::MACHINE_EPSILON_0 && deltaPredictedPosition.y < Math::MACHINE_EPSILON_0 )
           || (deltaPosition.y < Math::MACHINE_EPSILON_0 && deltaPredictedPosition.y > Math::MACHINE_EPSILON_0 ) )
          {
            overshotYAxis = true;
            frameGesture.screen.position.y = mLastGesture.screen.position.y;
          }

          // If there is overshot in one axis, reduce the possible overshot in the other axis,
          // and reduce the prediction amount so that it doesn't overshoot as easily next time.
          if(overshotXAxis || overshotYAxis)
          {
            mCurrentPredictionAmount -= mPredictionAmountAdjustment;
            if( mCurrentPredictionAmount > mMaxPredictionAmount + mPredictionAmountAdjustment ) // Guard against unsigned int overflow
            {
              mCurrentPredictionAmount = 0;
            }
            mCurrentPredictionAmount = std::max( mMinPredictionAmount, std::min( mCurrentPredictionAmount, mMaxPredictionAmount ) );

            if( overshotXAxis && !overshotYAxis )
            {
              frameGesture.screen.position.y = ( mLastGesture.screen.position.y + frameGesture.screen.position.y ) * 0.5f;
            }

            if( overshotYAxis && !overshotXAxis )
            {
              frameGesture.screen.position.x = ( mLastGesture.screen.position.x + frameGesture.screen.position.x ) * 0.5f;
            }
          }

          updateProperties = true;
          break;
        }
      }

      //std::cout << "todordel: gh5" << std::endl;
      // Perform smoothing.
      switch( mSmoothingMode )
      {
        case SMOOTHING_NONE:
        {
          // No smoothing
          break;
        }
        case SMOOTHING_LAST_VALUE:
        {
          SmoothingAlgorithm1( frameInfo.justStarted, frameGesture, lastVSyncTime );
          break;
        }
      }

      if( updateProperties )
      {
        TEST_finalResult = frameGesture; // TEST: Use this as the result.
        // only update properties if event received
        // set latest gesture to raw pan info with unchanged time
        //mPanning.Set( mInGesture & !frameInfo.justFinished );
        //mScreenPosition.Set( frameGesture.screen.position );
        //mScreenDisplacement.Set( frameGesture.screen.displacement );
        //mScreenVelocity.Set( frameGesture.screen.velocity );
        //mLocalPosition.Set( frameGesture.local.position );
        //mLocalDisplacement.Set( frameGesture.local.displacement );
        //mLocalVelocity.Set( frameGesture.local.velocity );
      }

      //if( mProfiling )
      //{
      //  mProfiling->mAveragedData.push_back( PanGestureProfiling::Position( frameGesture.time, frameGesture.screen.position,
      //      frameGesture.screen.displacement, frameGesture.screen.velocity, frameGesture.state ) );
      //}
    }

    mLastGesture = frameGesture;
    mLastUnmodifiedGesture = unmodifiedGesture;

    mInGesture &= ~frameInfo.justFinished;
    //if( mProfiling && frameInfo.justFinished )
    //{
    //  mProfiling->PrintData();
    //  mProfiling->ClearData();
    //}
    //return updateProperties;
    //std::cout << "todordel: gh6" << std::endl;
    return TEST_finalResult;
  }
  // Functions used by TEST_PREDICTION_CODE_DEFAULT:
  void RemoveOldHistory(PanInfoHistory& panHistory, unsigned int currentTime, unsigned int maxAge, unsigned int minEvents)
  {
    PanInfoHistoryConstIter endIter = panHistory.end();
    PanInfoHistoryIter iter = panHistory.begin();
    while( iter != endIter && panHistory.size() > minEvents)
    {
      PanInfo currentGesture = *iter;
      if( currentTime < currentGesture.time + maxAge )
      {
        break;
      }
      iter = panHistory.erase(iter);
      endIter = panHistory.end();
    }

    // dont want more than 5 previous predictions for smoothing
    iter = mPredictionHistory.begin();
    while( mPredictionHistory.size() > 1 && iter != mPredictionHistory.end() )
    {
      iter = mPredictionHistory.erase(iter);
    }
  }
  void PredictiveAlgorithm1(int eventsThisFrame, PanInfo& gestureOut, PanInfoHistory& panHistory, unsigned int lastVSyncTime, unsigned int nextVSyncTime)
  {
    RemoveOldHistory(panHistory, lastVSyncTime, MAX_GESTURE_AGE, 0);
    size_t panHistorySize = panHistory.size();
    if( panHistorySize == 0 )
    {
      // cant do any prediction without a history
      return;
    }

    PanInfoHistoryConstIter endIter = panHistory.end();
    PanInfoHistoryIter iter = panHistory.begin();
    Vector2 screenVelocity = gestureOut.screen.velocity;
    Vector2 localVelocity = gestureOut.local.velocity;
    Vector2 screenDisplacement = gestureOut.screen.displacement;
    Vector2 localDisplacement = gestureOut.local.displacement;

    bool havePreviousAcceleration = false;
    bool previousVelocity = false;
    float previousAccel = 0.0f;
    unsigned int lastTime(0);

    unsigned int interpolationTime = lastVSyncTime + mCurrentPredictionAmount;

    if( interpolationTime > gestureOut.time ) // Guard against the rare case when gestureOut.time > (lastVSyncTime + mCurrentPredictionAmount)
    {
      interpolationTime -= gestureOut.time;
    }
    else
    {
      interpolationTime = 0u;
    }

    while( iter != endIter )
    {
      PanInfo currentGesture = *iter;
      if( !previousVelocity )
      {
        // not yet set a previous velocity
        screenVelocity = currentGesture.screen.velocity;
        previousVelocity = true;
        lastTime = currentGesture.time;
        ++iter;
        continue;
      }
      float previousValueWeight = (float)(MAX_GESTURE_AGE - (lastVSyncTime - lastTime)) / (float)MAX_GESTURE_AGE;
      float velMag = currentGesture.screen.velocity.Length();
      float velDiff = velMag - screenVelocity.Length();
      float acceleration = 0.0f;

      float time(0.f);
      if (currentGesture.time > lastTime) // Guard against invalid timestamps
      {
        time = static_cast<float>( currentGesture.time - lastTime );
      }
      if( time > Math::MACHINE_EPSILON_1 )
      {
        acceleration = velDiff / time;
      }

      float newVelMag = 0.0f;
      int currentInterpolation = interpolationTime;
      if( !havePreviousAcceleration )
      {
        newVelMag =  velMag;
        havePreviousAcceleration = true;
      }
      else
      {
        newVelMag = velMag + (((acceleration * (1.0f - previousValueWeight)) + (previousAccel * previousValueWeight)) * currentInterpolation);
      }
      float velMod = 1.0f;
      if( velMag > Math::MACHINE_EPSILON_1 )
      {
        velMod = newVelMag / velMag;
      }
      gestureOut.screen.velocity = currentGesture.screen.velocity * velMod;
      gestureOut.local.velocity = currentGesture.local.velocity * velMod;
      screenDisplacement = gestureOut.screen.displacement + (gestureOut.screen.velocity * interpolationTime);
      localDisplacement = gestureOut.local.displacement + (gestureOut.local.velocity * interpolationTime);
      screenVelocity = currentGesture.screen.velocity;
      localVelocity = currentGesture.local.velocity;
      previousAccel = acceleration;
      ++iter;
    }
    // gestureOut's position is currently equal to the last event's position and its displacement is equal to last frame's total displacement
    // add interpolated distance and position to current
    // work out interpolated velocity
    gestureOut.screen.position = (gestureOut.screen.position - gestureOut.screen.displacement) + screenDisplacement;
    gestureOut.local.position = (gestureOut.local.position - gestureOut.local.displacement) + localDisplacement;
    gestureOut.screen.displacement = screenDisplacement;
    gestureOut.local.displacement = localDisplacement;
    gestureOut.time += interpolationTime;
  }
  void SmoothingAlgorithm1(bool justStarted, PanInfo& gestureOut, unsigned int lastVSyncTime)
  {
    if( justStarted )
    {
      gestureOut.screen.displacement = Vector2::ZERO;
      gestureOut.screen.velocity = Vector2::ZERO;
      gestureOut.local.displacement = Vector2::ZERO;
      gestureOut.local.velocity = Vector2::ZERO;
    }
    else
    {
      gestureOut.screen.position -= (gestureOut.screen.position - mLastGesture.screen.position) * 0.5f * (1.0f - mSmoothingAmount);
      gestureOut.local.position -= (gestureOut.local.position - mLastGesture.local.position) * 0.5f * (1.0f - mSmoothingAmount);
      // make current displacement relative to previous update-frame now.
      gestureOut.screen.displacement = gestureOut.screen.position - mLastGesture.screen.position;
      gestureOut.local.displacement = gestureOut.local.position - mLastGesture.local.position;
      // calculate velocity relative to previous update-frame
      unsigned int timeDiff( gestureOut.time - mLastGesture.time );
      gestureOut.screen.velocity = gestureOut.screen.displacement / (float)timeDiff;
      gestureOut.local.velocity = gestureOut.local.displacement / (float)timeDiff;
    }
  }
  // ####################################################################################################

  Vector2 CapVector( Vector2 input, float cap )
  {
    Vector2 output;
    if( output.x > cap )
    {
      output.x = cap;
    }
    if( output.x < -cap )
    {
      output.x = -cap;
    }
    if( output.y > cap )
    {
      output.y = cap;
    }
    if( output.y < -cap )
    {
      output.y = -cap;
    }
    return output;
  }
  bool SmoothingAlgorithm3( PanInfoHistory& history, unsigned int currentTime, unsigned int targetTime, unsigned int range,
        PanInfo& outPoint, bool eraseUnused, Vector2& outAccelLocal, Vector2& outAccelScreen )
  {
    unsigned int discardInputBufferTime = targetTime + range;//currentTime - mInputBufferingCompensation;
    //unsigned int targetTime = discardInputBufferTime - mPredictionDelayBuffer;
    unsigned int maxPredictionHistoryTime = targetTime - range;
    PanInfo targetPoint;
    targetPoint.time = targetTime;

    float divisor = 0.0f;
    float accelerationDivisor = 0.0f;
    unsigned int tapsUsed = 0;
    unsigned int accelerationTapsUsed = 0;
    PanInfoHistoryIter historyBegin = history.begin();
    PanInfoHistoryIter lastIt = history.end();
    Vector2 accelerationLocal;
    Vector2 accelerationScreen;
    bool pointGenerated = false;

    for( PanInfoHistoryIter it = historyBegin; it != history.end(); )
    {
      unsigned int gestureTime = it->time;

      if( gestureTime < maxPredictionHistoryTime )
      {
        if( eraseUnused )
        {
          it = history.erase( it );
        }
        else
        {
          ++it;
          continue;
        }
      }
      else
      {
        if( gestureTime > discardInputBufferTime )
        {
          // Discard.
          std::cout << "todor:######################################### ERROR: input discarded" << std::endl;
          break;
        }

        float timeDelta( abs( int( targetTime - gestureTime ) ) );

        // Handle zero time deltas.
        if( timeDelta < 1.0f )
        {
          //timeDelta = 0.01f;
          timeDelta = 1.0f;
        }

        //std::cout << "todor: maxPR:" << maxPredictionRange << ", timeDelta:" << timeDelta << std::endl;


        //std::cout << "todor: ========================= smoothing sub point: timeDelta:" << timeDelta << std::endl;

        targetPoint.local.position += it->local.position / timeDelta;
        targetPoint.screen.position += it->screen.position / timeDelta;

        targetPoint.local.velocity += it->local.velocity / timeDelta;
        targetPoint.screen.velocity += it->screen.velocity / timeDelta;

        if( lastIt != history.end() )
        {
          //std::cout << "todor:2.VEL:" << it->local.velocity.y << "," << lastIt->local.velocity.y << "," <<
          //    it->local.velocity.y - lastIt->local.velocity.y << std::endl;

          //float timeDifference = 11.9f;//todor hardcoded to test if errors come from time
          //todor PUTBACK float timeDifference( it->time - lastIt->time );
          float timeDifference = 1000.0f / 60.0f;//todor hardcoded to test if errors come from time todorscnowf
          //if( timeDifference < 0.1f )
          //{
          //  timeDifference = 0.1f;
          //}
          if( timeDifference < 1.0f )
          {
            timeDifference = 1.0f;
          }

          accelerationLocal += ( ( it->local.velocity - lastIt->local.velocity ) / timeDifference ) / timeDelta;
          accelerationScreen += ( ( it->screen.velocity - lastIt->screen.velocity ) / timeDifference ) / timeDelta;

          accelerationDivisor += 1.0f / timeDelta;

          accelerationTapsUsed++;
        }

        divisor += 1.0f / timeDelta;

        tapsUsed++;

        lastIt = it;
        ++it;
      }
    }

    Vector2 currentAccelerationLocal( outAccelLocal );
    Vector2 currentAccelerationScreen( outAccelScreen );

    if( tapsUsed > 0 )
    {
      if( divisor > 0.0f )
      {
        targetPoint.local.position /= divisor;
        targetPoint.screen.position /= divisor;

        targetPoint.local.velocity /= divisor;
        targetPoint.screen.velocity /= divisor;

        if( accelerationDivisor > 0.0f )
        {
          accelerationLocal /= accelerationDivisor;
          accelerationScreen /= accelerationDivisor;

#if 0
          //todor acceleration smoothing
          float accelerationSmoothing = 0.28f;//0.15f;//todorw 0.3f
          currentAccelerationLocal = ( mLastAccelerationLocal * accelerationSmoothing ) + ( accelerationLocal * ( 1.0f - accelerationSmoothing ) );
          currentAccelerationScreen = ( mLastAccelerationScreen * accelerationSmoothing ) + ( accelerationScreen * ( 1.0f - accelerationSmoothing ) );
#endif
        }

        //mLastAccelerationLocal = targetPoint.local.velocity - mLastGestureResult.local.velocity;
        //mLastAccelerationScreen = targetPoint.screen.velocity - mLastGestureResult.screen.velocity;
      }

      //std::cout << "todor:FINAL:div:" << divisor << ", POS:" << targetPoint.local.position.x << ", difflast:" <<
      //    ( targetPoint.local.position.x - mLastGestureResult.local.position.x ) << std::endl;
      //mTMPlast = targetPoint.local.position;

      targetPoint.local.displacement = targetPoint.local.position - mLastGestureResult.local.position;
      targetPoint.screen.displacement = targetPoint.screen.position - mLastGestureResult.screen.position;

      //float frameTime = 1000.0f / 60.0f;
      //float frameTime = nextVSyncTime - lastVSyncTime;
      //targetPoint.local.velocity = targetPoint.local.displacement / frameTime;
      //targetPoint.screen.velocity = targetPoint.screen.displacement / frameTime;


      pointGenerated = true;//todor mod
      //mStarvationCount = 0;
    }
    //else
    //{
#if 0
      if ( mStarvationCount++ < mStarvationMaximum )
      {
        std::cout << "todor:tmp:.....................................................STARVATION:" << mStarvationCount << std::endl;
        float updateDelta( nextVSyncTime - lastVSyncTime );

        float accelerationDegrade = 2.0f;
        currentAccelerationLocal /= accelerationDegrade;
        currentAccelerationScreen /= accelerationDegrade;

        targetPoint.local.velocity = mLastAccelerationLocal * updateDelta;
        targetPoint.local.displacement = targetPoint.local.velocity * updateDelta;
        targetPoint.local.position = mLastGesture.local.position + targetPoint.local.displacement;

        targetPoint.screen.velocity = mLastAccelerationScreen * updateDelta;
        targetPoint.screen.displacement = targetPoint.screen.velocity * updateDelta;
        targetPoint.screen.position = mLastGesture.screen.position + targetPoint.screen.displacement;


#if 0
        targetPoint.local.velocity = mLastGesture.local.velocity * 0.10f;
        targetPoint.local.displacement = targetPoint.local.velocity * updateDelta;
        targetPoint.local.position = mLastGesture.local.position + targetPoint.local.displacement;

        targetPoint.screen.velocity = mLastGesture.screen.velocity * 0.10f;
        targetPoint.screen.displacement = targetPoint.screen.velocity * updateDelta;
        targetPoint.screen.position = mLastGesture.screen.position + targetPoint.screen.displacement;
#endif
        pointGenerated = true;//todor mod
      }
#endif
      else
      {
        std::cout << "todor: NO INPUT" << std::endl;
        //targetPoint = mLastGesture;
        pointGenerated = false;
      }
    //}
    outPoint = targetPoint;
    outAccelLocal = currentAccelerationLocal;
    outAccelScreen = currentAccelerationScreen;
    return pointGenerated;
  }
  //........
  bool GetRSPoint( unsigned int currentTime, unsigned int targetTime, unsigned int range,
      PanInfo& outPoint, Vector2& outAccelLocal, Vector2& outAccelScreen )
  {
    unsigned int discardInputBufferTime = targetTime + range;//currentTime - mInputBufferingCompensation;
    //unsigned int targetTime = discardInputBufferTime - mPredictionDelayBuffer;
    unsigned int maxPredictionHistoryTime = targetTime - range;
    PanInfo targetPoint;
    targetPoint.time = targetTime;

    float divisor = 0.0f;
    float accelerationDivisor = 0.0f;
    unsigned int tapsUsed = 0;
    unsigned int accelerationTapsUsed = 0;
    PanInfoHistoryIter historyBegin = mPanHistory.begin();
    PanInfoHistoryIter lastIt = mPanHistory.end();
    Vector2 accelerationLocal;
    Vector2 accelerationScreen;
    bool pointGenerated = false;

    for( PanInfoHistoryIter it = historyBegin; it != mPanHistory.end(); )
    {
      unsigned int gestureTime = it->time;

      if( gestureTime < maxPredictionHistoryTime )
      {
        ++it;
        continue;
        //it = mPanHistory.erase( it );
      }
      else
      {
        if( gestureTime > discardInputBufferTime )
        {
          // Discard.
          std::cout << "todor:######################################### ERROR: input discarded" << std::endl;
          break;
        }

        float timeDelta( abs( int( targetTime - gestureTime ) ) );

        // Handle zero time deltas.
#if 0
        if( timeDelta > -0.1f && timeDelta < 0.1f )
        {
          timeDelta = 0.01f;
        }
#else
        if( timeDelta < 1.0f )
        {
          timeDelta = 1.0f;
        }
#endif

        //std::cout << "todor: maxPR:" << maxPredictionRange << ", timeDelta:" << timeDelta << std::endl;

        targetPoint.local.position += it->local.position / timeDelta;
        targetPoint.screen.position += it->screen.position / timeDelta;

        targetPoint.local.velocity += it->local.velocity / timeDelta;
        targetPoint.screen.velocity += it->screen.velocity / timeDelta;

        if( lastIt != mPanHistory.end() )
        {
          //std::cout << "todor:2.VEL:" << it->local.velocity.y << "," << lastIt->local.velocity.y << "," <<
          //    it->local.velocity.y - lastIt->local.velocity.y << std::endl;

          //float timeDifference = 11.9f;//todor hardcoded to test if errors come from time
          float timeDifference = 1000.0f / 60.0f;//todor hardcoded to test if errors come from time todorscnowf
          //todor PUTBACK float timeDifference( it->time - lastIt->time );
          //if( timeDifference < 0.1f )
          //{
          //  timeDifference = 0.1f;
          //}
          if( timeDifference < 1.0f )
          {
            timeDifference = 1.0f;
          }

          accelerationLocal += ( ( it->local.velocity - lastIt->local.velocity ) / timeDifference ) / timeDelta;
          accelerationScreen += ( ( it->screen.velocity - lastIt->screen.velocity ) / timeDifference ) / timeDelta;

          accelerationDivisor += 1.0f / timeDelta;

          accelerationTapsUsed++;
        }

        divisor += 1.0f / timeDelta;

        tapsUsed++;

        lastIt = it;
        ++it;
      }
    }

    Vector2 currentAccelerationLocal( outAccelLocal );
    Vector2 currentAccelerationScreen( outAccelScreen );

    if( tapsUsed > 0 )
    {
      if( divisor > 0.0f )
      {
        targetPoint.local.position /= divisor;
        targetPoint.screen.position /= divisor;

        targetPoint.local.velocity /= divisor;
        targetPoint.screen.velocity /= divisor;

        if( accelerationDivisor > 0.0f )
        {
          accelerationLocal /= accelerationDivisor;
          accelerationScreen /= accelerationDivisor;

#if 1
          //todor acceleration smoothing
          //float accelerationSmoothing = 0.28f;//0.15f;//todorw 0.3f
          float accelerationSmoothing = 0.3f;//0.15f;//todorw 0.3f
          currentAccelerationLocal = ( mLastAccelerationLocal * accelerationSmoothing ) + ( accelerationLocal * ( 1.0f - accelerationSmoothing ) );
          currentAccelerationScreen = ( mLastAccelerationScreen * accelerationSmoothing ) + ( accelerationScreen * ( 1.0f - accelerationSmoothing ) );
#endif
        }

        //mLastAccelerationLocal = targetPoint.local.velocity - mLastGestureResult.local.velocity;
        //mLastAccelerationScreen = targetPoint.screen.velocity - mLastGestureResult.screen.velocity;
      }

      //std::cout << "todor:FINAL:div:" << divisor << ", POS:" << targetPoint.local.position.x << ", difflast:" <<
      //    ( targetPoint.local.position.x - mLastGestureResult.local.position.x ) << std::endl;
      //mTMPlast = targetPoint.local.position;

      targetPoint.local.displacement = targetPoint.local.position - mLastGestureResult.local.position;
      targetPoint.screen.displacement = targetPoint.screen.position - mLastGestureResult.screen.position;

      //float frameTime = 1000.0f / 60.0f;
      //float frameTime = nextVSyncTime - lastVSyncTime;
      //targetPoint.local.velocity = targetPoint.local.displacement / frameTime;
      //targetPoint.screen.velocity = targetPoint.screen.displacement / frameTime;


      pointGenerated = true;//todor mod
      //mStarvationCount = 0;
    }
    //else
    //{
#if 0
      if ( mStarvationCount++ < mStarvationMaximum )
      {
        std::cout << "todor:tmp:.....................................................STARVATION:" << mStarvationCount << std::endl;
        float updateDelta( nextVSyncTime - lastVSyncTime );

        float accelerationDegrade = 2.0f;
        currentAccelerationLocal /= accelerationDegrade;
        currentAccelerationScreen /= accelerationDegrade;

        targetPoint.local.velocity = mLastAccelerationLocal * updateDelta;
        targetPoint.local.displacement = targetPoint.local.velocity * updateDelta;
        targetPoint.local.position = mLastGesture.local.position + targetPoint.local.displacement;

        targetPoint.screen.velocity = mLastAccelerationScreen * updateDelta;
        targetPoint.screen.displacement = targetPoint.screen.velocity * updateDelta;
        targetPoint.screen.position = mLastGesture.screen.position + targetPoint.screen.displacement;


#if 0
        targetPoint.local.velocity = mLastGesture.local.velocity * 0.10f;
        targetPoint.local.displacement = targetPoint.local.velocity * updateDelta;
        targetPoint.local.position = mLastGesture.local.position + targetPoint.local.displacement;

        targetPoint.screen.velocity = mLastGesture.screen.velocity * 0.10f;
        targetPoint.screen.displacement = targetPoint.screen.velocity * updateDelta;
        targetPoint.screen.position = mLastGesture.screen.position + targetPoint.screen.displacement;
#endif
        pointGenerated = true;//todor mod
      }
#endif
      else
      {
        std::cout << "todor: NO INPUT2" << std::endl;
        //targetPoint = mLastGesture;
        pointGenerated = false;
      }
    //}
    outPoint = targetPoint;
    outAccelLocal = currentAccelerationLocal;
    outAccelScreen = currentAccelerationScreen;
    return pointGenerated;
  }
  //........
  PanInfo PredictionCodeTest1( unsigned int lastVSyncTime, unsigned int nextVSyncTime )
  {
    PanInfo TEST_finalResult;
    //std::cout << "todor:UpdateProperties" << std::endl;
    unsigned int eventsThisFrame;
    {
      //todor PUTBACK Dali::Mutex::ScopedLock lock( mMutex );
      for( eventsThisFrame = 0; mReadPosition != mWritePosition; ++eventsThisFrame )
      {
        mReadGestures[ eventsThisFrame ] = mGestures[ mReadPosition ];
        ++mReadPosition;
        mReadPosition %= PAN_GESTURE_HISTORY;
      }
    }

    PanInfo lastReadGesture;
    bool justStarted = false;
    bool justFinished = false;


    //todor TEST RESAMPLE ==================================================================================================
    //Dali::Mutex::ScopedLock lock( mMutex );
    //unsigned int virtualTime = nextVSyncTime;

    PanInfo interpolatedGesture;
    PanInfo readGesture;
    PanInfo firstReadGesture;
    unsigned int eventsKeptThisFrame = 0;
    for( unsigned int readPosition = 0; readPosition < eventsThisFrame; ++readPosition )
    {
      // Copy the gesture first
      readGesture = mReadGestures[ readPosition ];
      mLastReadTemp = readGesture;//todor del

      //if( mProfiling )
      //{
      //  mProfiling->mRawData.push_back( PanGestureProfiling::Position( readGesture.time, readGesture.screen.position,
      //      readGesture.screen.displacement, readGesture.screen.velocity, readGesture.state ) );
      //}


      if( readGesture.state == Gesture::Started )
      {
        // Clear just finished as we have started new pan.
        justFinished = false;
        justStarted = true;
        mInGesture = true;
        mGestureState = IN_GESTURE;//todor delete

        eventsKeptThisFrame = 0;

        // Clear current pan history
        std::cout << "BUILD:100:todor:got start gesture" << std::endl;
        mPanHistory.clear();
        mLastAccelerationLocal = Vector2::ZERO;
        mLastAccelerationScreen = Vector2::ZERO;
        mLastAccelerationLocalPast = Vector2::ZERO;
        mLastAccelerationScreenPast = Vector2::ZERO;

        interpolatedGesture.screen.position = Vector2::ZERO;
        interpolatedGesture.local.position = Vector2::ZERO;
        interpolatedGesture.screen.velocity = Vector2::ZERO;
        interpolatedGesture.local.velocity = Vector2::ZERO;

        interpolatedGesture.time = 0;

        firstReadGesture = readGesture;

        PanInfo startInfo;//todor setup start gesture
        mLastGesture = startInfo;
        mLastGestureResult = startInfo;
        mLastNonPredictedResult = startInfo;
        //mLastPoint = Vector2::ZERO;
        //mLastPointPredict = Vector2::ZERO;
        mLastReadTemp = startInfo;
        mNotAtTarget = false;
        mStarvationCount = 0;
        //mStatsPredictionHistory.clear();
        mPredictionHistory.clear();

        //mTESTOverrideValueInt = 0;
        // NOTE: not used:...
      }
      else
      {
        justFinished |= ( readGesture.state == Gesture::Finished || readGesture.state == Gesture::Cancelled );
      }

      interpolatedGesture.screen.position += readGesture.screen.position;
      interpolatedGesture.local.position += readGesture.local.position;
      interpolatedGesture.screen.velocity += readGesture.screen.velocity;
      interpolatedGesture.local.velocity += readGesture.local.velocity;

      interpolatedGesture.time += readGesture.time;

      // Add event to history
      //mPanHistory.push_back( lastReadGesture );
      ++eventsKeptThisFrame;
    }

    //bool updateProperties = false;
    bool storeGesture = false;
    std::cout << "todor: pro: eventsKeptThisFrame: " << eventsKeptThisFrame << std::endl;
    if( eventsKeptThisFrame > 0 )
    {
      // Some events were read this frame.
      if( eventsKeptThisFrame > 1 )
      {
        interpolatedGesture.screen.position /= (float)eventsKeptThisFrame;
        interpolatedGesture.local.position /= (float)eventsKeptThisFrame;
        interpolatedGesture.screen.velocity /= (float)eventsKeptThisFrame;
        interpolatedGesture.local.velocity /= (float)eventsKeptThisFrame;

        //interpolatedGesture.time /= (float)eventsKeptThisFrame;//todor
        //interpolatedGesture.time = readGesture.time;

        mTargetGesture = readGesture;
        mNotAtTarget = true;
      }
      else
      {
        //interpolatedGesture.screen.displacement = readGesture.screen.displacement;
        //interpolatedGesture.local.displacement = readGesture.local.displacement;
        //interpolatedGesture.time = readGesture.time;

        mNotAtTarget = false;
      }

      interpolatedGesture.time = nextVSyncTime;

      if( justStarted )
      {
        interpolatedGesture.screen.displacement = interpolatedGesture.screen.position - firstReadGesture.screen.position;
        interpolatedGesture.local.displacement = interpolatedGesture.local.position - firstReadGesture.local.position;
      }
      else
      {
        interpolatedGesture.screen.displacement = interpolatedGesture.screen.position - mLastFrameReadGesture.screen.position;
        interpolatedGesture.local.displacement = interpolatedGesture.local.position - mLastFrameReadGesture.local.position;
      }

      //interpolatedGesture.time = virtualTime;

      //updateProperties = true;
      storeGesture = true;
    }
    else
    {
      // 0 Events this frame.
      if( mNotAtTarget )
      {
        mNotAtTarget = false;
        interpolatedGesture = mTargetGesture;
        storeGesture = true;
        //updateProperties = true;
      }
      else
      {
        // optional: ?
        //interpolatedGesture = mLastGesture;
      }
    }

    if( eventsKeptThisFrame > 0 )
    {
      mLastFrameReadGesture = readGesture;
    }

    if( storeGesture )
    {
      std::cout << "todor: pro: storeGesture: L:" << interpolatedGesture.local.position << ", S:" << interpolatedGesture.screen.position <<
          ", VL:" << interpolatedGesture.local.velocity << ", VS:" << interpolatedGesture.screen.velocity <<
          ", DL:" << interpolatedGesture.local.displacement << ", DS:" << interpolatedGesture.screen.displacement <<std::endl;
      mPanHistory.push_back( interpolatedGesture );
    }

    //todor TEST RESAMPLE end ==============================================================================================





    //todor OLD READ CODE
  #if 0
    //PanInfo lastReadGesture;
    for( unsigned int readPosition = 0; readPosition < eventsThisFrame; ++readPosition )
    {
      // Copy the gesture first
      lastReadGesture = mReadGestures[ readPosition ];
      //todor PUTBACK if( mProfiling )
      //{
      //  mProfiling->mRawData.push_back( PanGestureProfiling::Position( lastReadGesture.time, lastReadGesture.screen.position,
      //      lastReadGesture.screen.displacement, lastReadGesture.screen.velocity, lastReadGesture.state ) );
      //}

      // Add event to history
      mPanHistory.push_back( lastReadGesture );

#if 0
      std::cout << "todor:READG: pos:" << lastReadGesture.local.position.x <<
          ", diffpos:" << lastReadGesture.local.position.x - mLastReadTemp.local.position.x <<
          ", vel:" << lastReadGesture.local.velocity.x <<
          ", calcvel:" << ( lastReadGesture.local.position.x - mLastReadTemp.local.position.x ) / ( 1000.0f / 90.0f ) << std::endl;
#endif

      mLastReadTemp = lastReadGesture;//todor del

      if( lastReadGesture.state == Gesture::Started )
      {
        // Clear just finished as we have started new pan.
        justFinished = false;
        justStarted = true;

        // Clear current pan history
        std::cout << "todor:got start gesture" << std::endl;
        mPanHistory.clear();
        mLastAccelerationLocal = Vector2::ZERO;
        mLastAccelerationScreen = Vector2::ZERO;
        mLastAccelerationLocalPast = Vector2::ZERO;
        mLastAccelerationScreenPast = Vector2::ZERO;

        PanInfo startInfo;//todor setup start gesture
        mLastGesture = startInfo;
        mLastGestureResult = startInfo;
        mLastNonPredictedResult = startInfo;
        mLastPoint = Vector2::ZERO;
        mLastPointPredict = Vector2::ZERO;
        mLastReadTemp = startInfo;
        mNotAtTarget = false;
        mStarvationCount = 0;
        mStatsPredictionHistory.clear();
        mPredictionHistory.clear();

        // NOTE: not used:...
      }
      else
      {
        justFinished |= ( lastReadGesture.state == Gesture::Finished || lastReadGesture.state == Gesture::Cancelled );
      }
    }
#endif
    //todor OLD READ CODE end ==============================================================================================




    bool performUpdate = false;

    //unsigned int mInputBufferingCompensation = 3 * 12;
    unsigned int mInputBufferingCompensation = 0;
    //unsigned int mInputBufferingCompensation = 60.0f * 12.0f * 4.0f;
    //mStarvationMaximum = 60 * 4;//todor 3
    mStarvationMaximum = 2;//todor 3
    unsigned int mPredictionTapRange = 15 * 12; //todor 8?
    //unsigned int mPredictionDelayBuffer = 2.0f * 12.0f; //todor
    unsigned int mPredictionDelayBuffer = 0;
    mSmoothingMode = SMOOTHING_LAST_VALUE;//todor SMOOTHING_MULTITAP;
    mSmoothingAmount = 0.5f;
    //mPredictionAmount = 80;//todor tmp: use?
    //mPredictionAmount = 40;//todor tmp: use?

    //std::cout << "todor:v:" << mInputBufferingCompensation << "," << mStartvationFrames << "," << mPredictionTapRange << "," <<
    //    mPredictionDelayBuffer << "," << mMaximumHistoryTime << "," << std::endl;

    //std::cout << "todor:##### gh1" << std::endl;
    if( mGestureState != IDLE || justStarted )
    {
      //std::cout << "todor:##### gh2" << std::endl;
      performUpdate = true; // Force an update while we are in gesture.

      unsigned int currentTime = nextVSyncTime;
      unsigned int discardInputBufferTime = currentTime - mInputBufferingCompensation;
      unsigned int targetTime = discardInputBufferTime - mPredictionDelayBuffer;
      unsigned int maxPredictionHistoryTime = targetTime - mPredictionTapRange;
      PanInfo targetPoint;
      targetPoint.time = targetTime;

      float divisor = 0.0f;
      float accelerationDivisor = 0.0f;
      unsigned int tapsUsed = 0;
      unsigned int accelerationTapsUsed = 0;
      PanInfoHistoryIter historyBegin = mPanHistory.begin();
      PanInfoHistoryIter lastIt = mPanHistory.end();
      Vector2 accelerationLocal;
      Vector2 accelerationScreen;

      for( PanInfoHistoryIter it = historyBegin; it != mPanHistory.end(); )
      {
        unsigned int gestureTime = it->time;

        if( gestureTime < maxPredictionHistoryTime )
        {
          it = mPanHistory.erase( it );
        }
        else
        {
          if( gestureTime > discardInputBufferTime )
          {
            // Discard.
            std::cout << "todor:######################################### ERROR: input discarded2" << std::endl;
            break;
          }

          float timeDelta( abs( int( targetTime - gestureTime ) ) );

          // Handle zero time deltas.
          if( timeDelta < 1.0f )
          {
            timeDelta = 1.0f; //todorscnow was 0.01f
          }

          //std::cout << "todor: maxPR:" << maxPredictionRange << ", timeDelta:" << timeDelta << std::endl;

          targetPoint.local.position += it->local.position / timeDelta;
          targetPoint.screen.position += it->screen.position / timeDelta;

          targetPoint.local.velocity += it->local.velocity / timeDelta;
          targetPoint.screen.velocity += it->screen.velocity / timeDelta;

          if( lastIt != mPanHistory.end() )
          {
            //std::cout << "todor:VEL:" << it->local.velocity.y << "," << lastIt->local.velocity.y << "," <<
            //    it->local.velocity.y - lastIt->local.velocity.y << std::endl;

            float timeDifference = 1000.0f / 60.0f;//todor hardcoded to test if errors come from time
            //todor PUTBACK float timeDifference( it->time - lastIt->time );
            if( timeDifference < 0.1f )//todorscnow soon
            {
              timeDifference = 0.1f;
            }

            accelerationLocal += ( ( it->local.velocity - lastIt->local.velocity ) / timeDifference ) / timeDelta;
            accelerationScreen += ( ( it->screen.velocity - lastIt->screen.velocity ) / timeDifference ) / timeDelta;

            accelerationDivisor += 1.0f / timeDelta;

            accelerationTapsUsed++;
          }

          divisor += 1.0f / timeDelta;

          tapsUsed++;

          lastIt = it;
          ++it;
        }
      }

      Vector2 currentAccelerationLocal( mLastAccelerationLocal );
      Vector2 currentAccelerationScreen( mLastAccelerationScreen );

      if( tapsUsed > 0 )
      {
        if( divisor > 0.0f )
        {
          targetPoint.local.position /= divisor;
          targetPoint.screen.position /= divisor;

          targetPoint.local.velocity /= divisor;
          targetPoint.screen.velocity /= divisor;

          if( accelerationDivisor > 0.0f )
          {
            accelerationLocal /= accelerationDivisor;
            accelerationScreen /= accelerationDivisor;

            //todorMAINCHANGE (in/out?)
#if 1
            //todorscnow
            //todor acceleration smoothing
            float accelerationSmoothing = 0.3f;//0.15f;//todorw 0.3f
            currentAccelerationLocal = ( mLastAccelerationLocal * accelerationSmoothing ) + ( accelerationLocal * ( 1.0f - accelerationSmoothing ) );
            currentAccelerationScreen = ( mLastAccelerationScreen * accelerationSmoothing ) + ( accelerationScreen * ( 1.0f - accelerationSmoothing ) );
#endif
#if 0
            currentAccelerationLocal = accelerationLocal;
            currentAccelerationScreen = accelerationScreen;
#endif
          }

          //mLastAccelerationLocal = targetPoint.local.velocity - mLastGestureResult.local.velocity;
          //mLastAccelerationScreen = targetPoint.screen.velocity - mLastGestureResult.screen.velocity;
        }

        //std::cout << "todor:FINAL:div:" << divisor << ", POS:" << targetPoint.local.position.x << ", difflast:" <<
        //    ( targetPoint.local.position.x - mLastGestureResult.local.position.x ) << std::endl;
        //mTMPlast = targetPoint.local.position;

        targetPoint.local.displacement = targetPoint.local.position - mLastGestureResult.local.position;
        targetPoint.screen.displacement = targetPoint.screen.position - mLastGestureResult.screen.position;

        //float frameTime = 1000.0f / 60.0f;
        //float frameTime = nextVSyncTime - lastVSyncTime;
        //targetPoint.local.velocity = targetPoint.local.displacement / frameTime;
        //targetPoint.screen.velocity = targetPoint.screen.displacement / frameTime;


        performUpdate = true;//todor mod
        mStarvationCount = 0;
      }
      else
      {
  #if 1
        if ( mStarvationCount++ < mStarvationMaximum )
        {
          std::cout << "todor:tmp:.....................................................STARVATION:" << mStarvationCount << std::endl;
          float updateDelta( nextVSyncTime - lastVSyncTime );

          float accelerationDegrade = 2.0f;
          currentAccelerationLocal /= accelerationDegrade;
          currentAccelerationScreen /= accelerationDegrade;

          targetPoint.local.velocity = mLastAccelerationLocal * updateDelta;
          targetPoint.local.displacement = targetPoint.local.velocity * updateDelta;
          targetPoint.local.position = mLastGesture.local.position + targetPoint.local.displacement;

          targetPoint.screen.velocity = mLastAccelerationScreen * updateDelta;
          targetPoint.screen.displacement = targetPoint.screen.velocity * updateDelta;
          targetPoint.screen.position = mLastGesture.screen.position + targetPoint.screen.displacement;


  #if 0
          targetPoint.local.velocity = mLastGesture.local.velocity * 0.10f;
          targetPoint.local.displacement = targetPoint.local.velocity * updateDelta;
          targetPoint.local.position = mLastGesture.local.position + targetPoint.local.displacement;

          targetPoint.screen.velocity = mLastGesture.screen.velocity * 0.10f;
          targetPoint.screen.displacement = targetPoint.screen.velocity * updateDelta;
          targetPoint.screen.position = mLastGesture.screen.position + targetPoint.screen.displacement;
  #endif
          performUpdate = true;//todor mod
        }
  #endif
        else
        {
          std::cout << "todor: NO INPUT3" << std::endl;
          targetPoint = mLastGesture;
          performUpdate = false;
        }
      }


      // Store last interpolated result.
      mLastGestureResult = targetPoint;
      PanInfo lastNonPredictedResult = targetPoint;



      // Prediction.
  #if 1
      PanInfo predictedPoint;

      mPredictionAmount = 40;
      //mPredictionAmount = 32;//todorMAINCHANGE (with scalar only)



      unsigned int timeRange = ( mPredictionAmount * 1.0f );//todor exclusive to PA
      unsigned int pastInterpolateTime = nextVSyncTime - timeRange;//todor exclusive to PA
      PanInfo outPoint;

      // ................................................. 2 point prediction ...............................................

      Vector2 useAccelerationLocalPast( mLastAccelerationLocalPast );//todor avg
      Vector2 useAccelerationScreenPast( mLastAccelerationScreenPast );

      //bool GetRSPoint( unsigned int currentTime, unsigned int targetTime, unsigned int range, PanInfo& outPoint )
      if( !GetRSPoint( nextVSyncTime, pastInterpolateTime, timeRange, outPoint, useAccelerationLocalPast, useAccelerationScreenPast ) )
      {
        //todor
        outPoint = mLastNonPredictedResult;
        std::cout << "todor: no prev point ................................................................" << std::endl;
      }

      //float accelCap = 0.00005f;
      //float accelCap = 0.0005f;

      //float accelCap = 0.0005f;
      float accelCap = 0.0005f;


      // Dynamic acceleration limiter
      //todorscnow

#if 0
      //float accelCap = 0.0000f;
      Vector2 TMPaccDiff( currentAccelerationLocal - mLastAccelerationLocal );
      if( fabs( TMPaccDiff.x ) > accelCap || fabs( TMPaccDiff.y ) > accelCap )
      {
        std::cout << "todor:::::::::::::::::::::::::: cur acc local diff: " << TMPaccDiff << std::endl;
      }
#endif
      currentAccelerationLocal = mLastAccelerationLocal + CapVector( Vector2( currentAccelerationLocal - mLastAccelerationLocal ), accelCap );
      currentAccelerationScreen = mLastAccelerationScreen + CapVector( Vector2( currentAccelerationScreen - mLastAccelerationScreen ), accelCap );






      mLastAccelerationLocalPast = useAccelerationLocalPast;
      mLastAccelerationScreenPast = useAccelerationScreenPast;

#if 0
      Vector2 usePositionLocal( outPoint.local.position );
      Vector2 usePositionScreen( outPoint.screen.position );
#else

#if 1
      Vector2 usePositionLocal( targetPoint.local.position );
      Vector2 usePositionScreen( targetPoint.screen.position );
#else
      Vector2 usePositionLocal( mLastGesture.local.position );
      Vector2 usePositionScreen( mLastGesture.screen.position );
#endif

#endif

//#if 1
      Vector2 useVelocityLocalPast( outPoint.local.velocity );
      Vector2 useVelocityScreenPast( outPoint.screen.velocity );
//#else
      Vector2 useVelocityLocalCurrent( targetPoint.local.velocity );
      Vector2 useVelocityScreenCurrent( targetPoint.screen.velocity );
//#endif

//#if 0
      //Vector2 useAccelerationLocalPast( mLastAccelerationLocal );//todor avg
      //Vector2 useAccelerationScreenPast( mLastAccelerationScreen );
//#else
      Vector2 useAccelerationLocalCurrent( currentAccelerationLocal );
      Vector2 useAccelerationScreenCurrent( currentAccelerationScreen );
//#endif

      float predictionDelta( mInputBufferingCompensation + mPredictionDelayBuffer + ( mPredictionAmount * 1.0f ) );

      // final vals.
#if 1
      Vector2 useVelocityLocal( ( useVelocityLocalPast + useVelocityLocalCurrent ) / 2.0f );
      Vector2 useVelocityScreen( ( useVelocityScreenPast + useVelocityScreenCurrent ) / 2.0f );
#else
      Vector2 useVelocityLocal( ( mLastNonPredictedResult.local.velocity += mLastNonPredictedResult.local.velocity * currentAccelerationLocal * 1 ) );
      Vector2 useVelocityScreen( ( mLastNonPredictedResult.screen.velocity += mLastNonPredictedResult.screen.velocity + currentAccelerationScreen * 1 ) );
#endif




      Vector2 useAccelerationLocal( ( useAccelerationLocalPast + useAccelerationLocalCurrent ) / 2.0f );
      Vector2 useAccelerationScreen( ( useAccelerationScreenPast + useAccelerationScreenCurrent ) / 2.0f );




      //predictionDelta += ( mPredictionAmount / 2.0f );

#if 1
      float mult = 24.0f;//todorMAINCHANGE
#else
      float mult = 40.0f;
#endif
      predictedPoint.local.position = usePositionLocal + ( useVelocityLocal * mult );
      predictedPoint.screen.position = usePositionScreen + ( useVelocityScreen * mult );

#if 1
      PanInfo integratedPoint;

#if 0
      integratedPoint.local.position = usePositionLocal + ( targetPoint.local.velocity * predictionDelta ) +
          ( useAccelerationLocalCurrent * ( predictionDelta * predictionDelta * 0.5f ) );
      integratedPoint.screen.position = usePositionScreen + ( targetPoint.screen.velocity * predictionDelta ) +
          ( useAccelerationScreenCurrent * ( predictionDelta * predictionDelta * 0.5f ) );
#else
      integratedPoint.local.position = usePositionLocal + ( useVelocityLocal * predictionDelta ) +
          ( useAccelerationLocalCurrent * ( predictionDelta * predictionDelta * 0.5f ) );
      integratedPoint.screen.position = usePositionScreen + ( useVelocityScreen * predictionDelta ) +
          ( useAccelerationScreenCurrent * ( predictionDelta * predictionDelta * 0.5f ) );
#endif

#if 0
      predictedPoint.local.position = ( predictedPoint.local.position + integratedPoint.local.position ) / 2.0f;//todor check
      predictedPoint.screen.position = ( predictedPoint.screen.position + integratedPoint.screen.position ) / 2.0f;//todor check
#else
      predictedPoint.local.position = integratedPoint.local.position;
      predictedPoint.screen.position = integratedPoint.screen.position;
#endif

      //std::cout << "todor:##### gh3: L:" << predictedPoint.local.position << std::endl;


#endif

#if 0
      //todor tech w
      predictedPoint.local.position = usePositionLocal + ( useVelocityLocal * predictionDelta ) +
          ( useAccelerationLocal * ( predictionDelta * predictionDelta * 0.5f ) );
      predictedPoint.screen.position = usePositionScreen + ( useVelocityScreen * predictionDelta ) +
          ( useAccelerationScreen * ( predictionDelta * predictionDelta * 0.5f ) );
#endif



#if 0
      Vector2 secondDerivativeAccelerationLocal( currentAccelerationLocal - mLastAccelerationLocal );
      Vector2 secondDerivativeAccelerationScreen( currentAccelerationScreen - mLastAccelerationScreen );

      // NOTE:
      // 1) threshold? acc change threshold capped (DONE with maxSDA)
      // 2) direction change? need a way of eliminating small direction changes back and forth each frame.
      //     - comp SDA sign vs lastSDA.

      // Confidence factor.
  #if 1
      std::cout << "todor:SDAC:" << secondDerivativeAccelerationLocal << std::endl;
      const float maximumSecondDerivativeAcceleration = 0.03f;//todor needs experimenting
      const Vector2 maximumSecondDerivativeAccelerationVector( maximumSecondDerivativeAcceleration, maximumSecondDerivativeAcceleration );

      if( secondDerivativeAccelerationLocal.x > maximumSecondDerivativeAcceleration )
      {
        secondDerivativeAccelerationLocal.x = maximumSecondDerivativeAcceleration;
      }
      if( secondDerivativeAccelerationLocal.x < -maximumSecondDerivativeAcceleration )
      {
        secondDerivativeAccelerationLocal.x = -maximumSecondDerivativeAcceleration;
      }
      if( secondDerivativeAccelerationLocal.y > maximumSecondDerivativeAcceleration )
      {
        secondDerivativeAccelerationLocal.y = maximumSecondDerivativeAcceleration;
      }
      if( secondDerivativeAccelerationLocal.y < -maximumSecondDerivativeAcceleration )
      {
        secondDerivativeAccelerationLocal.y = -maximumSecondDerivativeAcceleration;
      }
      if( secondDerivativeAccelerationScreen.x > maximumSecondDerivativeAcceleration )
      {
        secondDerivativeAccelerationScreen.x = maximumSecondDerivativeAcceleration;
      }
      if( secondDerivativeAccelerationScreen.x < -maximumSecondDerivativeAcceleration )
      {
        secondDerivativeAccelerationScreen.x = -maximumSecondDerivativeAcceleration;
      }
      if( secondDerivativeAccelerationScreen.y > maximumSecondDerivativeAcceleration )
      {
        secondDerivativeAccelerationScreen.y = maximumSecondDerivativeAcceleration;
      }
      if( secondDerivativeAccelerationScreen.y < -maximumSecondDerivativeAcceleration )
      {
        secondDerivativeAccelerationScreen.y = -maximumSecondDerivativeAcceleration;
      }

      Vector2 confidenceLocal( ( maximumSecondDerivativeAccelerationVector -
          Vector2( fabs( secondDerivativeAccelerationLocal.x ), fabs( secondDerivativeAccelerationLocal.y ) )
              ) / maximumSecondDerivativeAcceleration );
      Vector2 confidenceScreen( ( maximumSecondDerivativeAccelerationVector -
          Vector2( fabs( secondDerivativeAccelerationScreen.x ), fabs( secondDerivativeAccelerationScreen.y ) )
              ) / maximumSecondDerivativeAcceleration );

      //confidenceLocal /= 2.0f;
      //confidenceScreen /= 2.0f;

      std::cout << "todor:.......................CL:" << confidenceLocal << std::endl;
      Vector2 useAccelerationLocal = ( mLastAccelerationLocal * ( Vector2::ONE - confidenceLocal ) ) + ( currentAccelerationLocal * confidenceLocal );
      Vector2 useAccelerationScreen = ( mLastAccelerationScreen * ( Vector2::ONE - confidenceScreen ) ) + ( currentAccelerationScreen * confidenceScreen );
  #endif

      //todor override confidence.
  #if 0
      useAccelerationLocal = currentAccelerationLocal;
      useAccelerationScreen = currentAccelerationScreen;
  #endif

      float predictionDelta( mInputBufferingCompensation + mPredictionDelayBuffer + mPredictionAmount );

      // Calc end position including acceleration curve.

      Vector2 useVelocityLocal = ( mLastNonPredictedResult.local.velocity * ( Vector2::ONE - confidenceLocal ) ) + ( targetPoint.local.velocity * confidenceLocal );
      Vector2 useVelocityScreen = ( mLastNonPredictedResult.screen.velocity * ( Vector2::ONE - confidenceScreen ) ) + ( targetPoint.screen.velocity * confidenceScreen );

      Vector2 usePositionLocal = ( mLastNonPredictedResult.local.position * ( Vector2::ONE - confidenceLocal ) ) + ( targetPoint.local.position * confidenceLocal );
      Vector2 usePositionScreen = ( mLastNonPredictedResult.screen.position * ( Vector2::ONE - confidenceScreen ) ) + ( targetPoint.screen.position * confidenceScreen );

      Vector2 predictionDeltaVectorLocal( confidenceLocal * predictionDelta );
      Vector2 predictionDeltaVectorScreen( confidenceScreen * predictionDelta );


#endif




#if 0
      predictedPoint.local.position = usePositionLocal + ( useVelocityLocal * predictionDeltaVectorLocal ) +
          ( useAccelerationLocal * ( predictionDeltaVectorLocal * predictionDeltaVectorLocal * 0.5f ) );

      predictedPoint.screen.position = usePositionScreen + ( useVelocityScreen * predictionDeltaVectorScreen ) +
          ( useAccelerationScreen * ( predictionDeltaVectorScreen * predictionDeltaVectorScreen * 0.5f ) );
#endif
#if 0
      predictedPoint.local.position = targetPoint.local.position + ( targetPoint.local.velocity * predictionDelta ) +
          ( useAccelerationLocal * ( 0.5f * predictionDelta * predictionDelta ) );

      predictedPoint.screen.position = targetPoint.screen.position + ( targetPoint.screen.velocity * predictionDelta ) +
          ( useAccelerationScreen * ( 0.5f * predictionDelta * predictionDelta ) );
#endif





  #if 0
      // Calc end position from velocity only:
      predictedPoint.local.velocity = targetPoint.local.velocity;
      predictedPoint.local.displacement = predictedPoint.local.velocity * predictionDelta;
      predictedPoint.local.position = targetPoint.local.position + predictedPoint.local.displacement;
  #endif

#if 0
      std::cout << "todor:PRED_AC_F1 ..................................... :sda:" <<
          secondDerivativeAccelerationLocal.x << ", v:" << targetPoint.local.velocity.x << ", a:" <<
          useAccelerationLocal.x << ", t:" << predictionDelta << ", fx:" << predictedPoint.local.position.x << std::endl;
#endif

      //predictedPoint.local.velocity = mLastAccelerationLocal * ( 1000.0f / 60.0f );
      //predictedPoint.local.velocity = targetPoint.local.velocity + mLastAccelerationLocal;
      predictedPoint.local.velocity = targetPoint.local.velocity;
      predictedPoint.local.displacement = predictedPoint.local.velocity * predictionDelta;

      predictedPoint.screen.velocity = targetPoint.screen.velocity;
      predictedPoint.screen.displacement = predictedPoint.screen.velocity * predictionDelta;
      //predictedPoint.screen.position = targetPoint.screen.position + predictedPoint.screen.displacement;

      mLastAccelerationLocal = currentAccelerationLocal;
      mLastAccelerationScreen = currentAccelerationScreen;


  #if 0
      float predictionDelta( mInputBufferingCompensation + mPredictionDelayBuffer + mPredictionAmount );
      //predictedPoint.local.velocity = mLastAccelerationLocal * predictionDelta;

      //predictedPoint.local.velocity = targetPoint.local.velocity + mLastAccelerationLocal;

      predictedPoint.local.velocity = targetPoint.local.velocity;

      predictedPoint.local.displacement = targetPoint.local.displacement * predictedPoint.local.velocity;
      predictedPoint.local.position = targetPoint.local.position + predictedPoint.local.displacement;
  #endif
  #if 0
      //todor worked with vel only.
      float predictionDelta( mInputBufferingCompensation + mPredictionDelayBuffer + mPredictionAmount );
      //predictedPoint.local.velocity = mLastAccelerationLocal * ( 1000.0f / 60.0f );
      //predictedPoint.local.velocity = targetPoint.local.velocity + mLastAccelerationLocal;
      predictedPoint.local.velocity = targetPoint.local.velocity;
      predictedPoint.local.displacement = predictedPoint.local.velocity * predictionDelta;
      predictedPoint.local.position = targetPoint.local.position + predictedPoint.local.displacement;

      predictedPoint.screen.velocity = targetPoint.screen.velocity;
      predictedPoint.screen.displacement = predictedPoint.screen.velocity * predictionDelta;
      predictedPoint.screen.position = targetPoint.screen.position + predictedPoint.screen.displacement;
      //todor not done yet
      //predictedPoint.screen.velocity = targetPoint.screen.velocity + mLastAccelerationScreen;
      //predictedPoint.screen.displacement = targetPoint.screen.displacement * predictedPoint.screen.velocity;
      //predictedPoint.screen.position = targetPoint.screen.position + predictedPoint.screen.displacement;
  #endif

      //todor predictedPoint.time = nextVSyncTime + mPredictionAmount;
      predictedPoint.time = nextVSyncTime;// + mPredictionAmount;
      //DALI_LOG_ERROR( "todor:acc:%f,%f,%d\n", predictedPoint.local.position.y, mLastAccelerationLocal.y, accelerationTapsUsed );
      //std::cout << "todor:FINAL:  pos:" << predictedPoint.local.position.x << ", disp:" << predictedPoint.local.displacement.x << std::endl;


      //mLastGestureResult = targetPoint;

      //.........................................................................................
      // Use predicted point todor: comment to disable prediction.
#if 1
      targetPoint = predictedPoint;
#endif

#endif


      mSmoothingAmount = 0.35f;//todorw 0.25f
#if 1
      SmoothingAlgorithm1( justStarted, targetPoint, 0 );//todor smoothing
//#else
#endif
#if 1
      //custom setup (move & share code with resample code).
      targetPoint.time = nextVSyncTime;
      mPredictionHistory.push_back( targetPoint );
      //int smoothingRange = 35;//todorMAINCHANGE hardcode
      int smoothingRange = 55;//todor hardcode
      Vector2 tmpA, tmpB;
      SmoothingAlgorithm3( mPredictionHistory, nextVSyncTime, nextVSyncTime, smoothingRange, targetPoint, true, tmpA, tmpB );
#endif


      //std::cout << "todor:##### gh4: L:" << targetPoint.local.position << std::endl;

      //todor prediction
      //void PanGesture::PredictiveAlgorithm1(int eventsThisFrame, PanInfo& gestureOut, PanInfoHistory& panHistory, unsigned int lastVSyncTime, unsigned int nextVSyncTime)
  #if 0
      //RemoveOldHistory(panHistory, lastVSyncTime, MAX_GESTURE_AGE, 0);
      size_t panHistorySize = mPanHistory.size();
      if( panHistorySize > 0 )

      PanInfoHistoryConstIter endIter = panHistory.end();
      PanInfoHistoryIter iter = panHistory.begin();

      // Truncate end.
      for( PanInfoHistoryIter it = iter; it != endIter; ++it )
      {
        if( it->time > ( currentTime - mInputBufferingCompensation ) )
        {
          endIter = it;
          break;
        }
      }

      //PanInfo gestureOut = *it;
      PanInfo gestureOut = targetPoint;//todor zero copy

      Vector2 screenVelocity = gestureOut.screen.velocity;
      Vector2 localVelocity = gestureOut.local.velocity;
      Vector2 screenDisplacement = gestureOut.screen.displacement;
      Vector2 localDisplacement = gestureOut.local.displacement;

      bool havePreviousAcceleration = false;
      bool previousVelocity = false;
      float previousAccel = 0.0f;
      unsigned int lastTime(0);

      unsigned int interpolationTime = lastVSyncTime + mCurrentPredictionAmount;

      if( interpolationTime > gestureOut.time ) // Guard against the rare case when gestureOut.time > (lastVSyncTime + mCurrentPredictionAmount)
      {
        interpolationTime -= gestureOut.time;
      }
      else
      {
        interpolationTime = 0u;
      }

      while( iter != endIter )
      {
        PanInfo currentGesture = *iter;
        if( !previousVelocity )
        {
          // not yet set a previous velocity
          screenVelocity = currentGesture.screen.velocity;
          previousVelocity = true;
          lastTime = currentGesture.time;
          ++iter;
          continue;
        }
        float previousValueWeight = (float)(MAX_GESTURE_AGE - (lastVSyncTime - lastTime)) / (float)MAX_GESTURE_AGE;
        float velMag = currentGesture.screen.velocity.Length();
        float velDiff = velMag - screenVelocity.Length();
        float acceleration = 0.0f;

        float time(0.f);
        if (currentGesture.time > lastTime) // Guard against invalid timestamps
        {
          time = static_cast<float>( currentGesture.time - lastTime );
        }
        if( time > Math::MACHINE_EPSILON_1 )
        {
          acceleration = velDiff / time;
        }

        float newVelMag = 0.0f;
        int currentInterpolation = interpolationTime;
        if( !havePreviousAcceleration )
        {
          newVelMag =  velMag;
          havePreviousAcceleration = true;
        }
        else
        {
          newVelMag = velMag + (((acceleration * (1.0f - previousValueWeight)) + (previousAccel * previousValueWeight)) * currentInterpolation);
        }
        float velMod = 1.0f;
        if( velMag > Math::MACHINE_EPSILON_1 )
        {
          velMod = newVelMag / velMag;
        }
        gestureOut.screen.velocity = currentGesture.screen.velocity * velMod;
        gestureOut.local.velocity = currentGesture.local.velocity * velMod;
        screenDisplacement = gestureOut.screen.displacement + (gestureOut.screen.velocity * interpolationTime);
        localDisplacement = gestureOut.local.displacement + (gestureOut.local.velocity * interpolationTime);
        screenVelocity = currentGesture.screen.velocity;
        localVelocity = currentGesture.local.velocity;
        previousAccel = acceleration;
        ++iter;
      }
      // gestureOut's position is currently equal to the last event's position and its displacement is equal to last frame's total displacement
      // add interpolated distance and position to current
      // work out interpolated velocity
      gestureOut.screen.position = (gestureOut.screen.position - gestureOut.screen.displacement) + screenDisplacement;
      gestureOut.local.position = (gestureOut.local.position - gestureOut.local.displacement) + localDisplacement;
      gestureOut.screen.displacement = screenDisplacement;
      gestureOut.local.displacement = localDisplacement;
      gestureOut.time += interpolationTime;
  #endif

      switch( mGestureState )
      {
        case IDLE:
        {
          if( justFinished )
          {
            mGestureState = IDLE;
            //mGestureState = END_GESTURE_QUEUED;//todor
          }
          else if( justStarted )
          {
            mGestureState = IN_GESTURE;
          }
          break;
        }
        case IN_GESTURE:
        {
          if( justFinished )
          {
            //mGestureState = END_GESTURE_QUEUED;//todor
            mGestureState = IDLE;
          }
          break;
        }
        case END_GESTURE_QUEUED:
        {
          if( justStarted && !justFinished )
          {
            mGestureState = IN_GESTURE;
          }
          else if( !performUpdate )
          {
            mGestureState = IDLE;

            //if( mProfiling )
            //{
            //  mProfiling->PrintData();
            //  mProfiling->ClearData();
            //}
          }
          break;
        }
      }

      if( performUpdate )
      {
        TEST_finalResult = targetPoint; // TEST: Use this as the result.
        // only update properties if event received
        // set latest gesture to raw pan info with unchanged time
        //mPanning.Set( mGestureState != IDLE );// mInGesture & !frameInfo.justFinished;
        //mScreenPosition.Set( frameGesture.screen.position );
        //mScreenDisplacement.Set( frameGesture.screen.displacement );
        //mScreenVelocity.Set( frameGesture.screen.velocity );
        //mLocalPosition.Set( frameGesture.local.position );
        //mLocalDisplacement.Set( frameGesture.local.displacement );
        //mLocalVelocity.Set( frameGesture.local.velocity );

        //std::cout << "todor:FINAL:STATE:" << mGestureState << ", POS:" << targetPoint.local.position.x << ", difflast:" <<
        //    ( targetPoint.local.position.x - mLastGesture.local.position.x ) << std::endl;

        //if( mProfiling )
        //{
        //  mProfiling->mAveragedData.push_back( PanGestureProfiling::Position( targetPoint.time, targetPoint.screen.position,
        //      targetPoint.screen.displacement, targetPoint.screen.velocity, targetPoint.state ) );
        //}
      }

      mLastGesture = targetPoint;
      mLastNonPredictedResult = lastNonPredictedResult;
    }

    //performUpdate = true;//todor mod
    //std::cout << "todor:UpdateProperties returning:" << performUpdate << std::endl;
    //return performUpdate;

    //std::cout << "todor:##### ghf: L:" << TEST_finalResult.local.position << std::endl;
    return TEST_finalResult;
  }

  // ####################################################################################################
  // ####################################################################################################
  // ####################################################################################################
  // ####################################################################################################

public:

  ExampleController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ExampleController::Create );
    mTouchPositionHistory.SetMaxSize( 60 ); // 60 touch-frames of history
    mFrameNumber = 0;
    mWritePosition = 0;
    mReadPosition = 0;
    mInGesture = false;
    mTouchStartTime = 0;
    mTEST_predictionMode = DEFAULT_TEST_PREDICTION_CODE;

    // Overrides
    mPredictionMode = PREDICTION_1;
    mPredictionAmount = 14;
    mMinPredictionAmount = 14;
    mMaxPredictionAmount = 110;
    mPredictionAmountAdjustment = 4;
    mSmoothingMode = SMOOTHING_LAST_VALUE;
    mSmoothingAmount = 0.30f;

  }

  ~ExampleController()
  {
    // Nothing to do here;
  }

  ImageActor CreateBitmapActor( int width, int height ) //const Vector4& color, bool border, const Vector4& borderColor, const unsigned int borderSize )
  {
    ImageActor image;
    //BufferImage imageData = BufferImage::New( width, height, Pixel::RGB888 );
    BufferImage imageData = BufferImage::New( width, height, Pixel::RGBA8888 );
    PixelBuffer* pixbuf = imageData.GetBuffer();
    if( !pixbuf )
    {
      return image;
    }
    Vector4 color = Color::BLUE;
    const unsigned int bitmapSize = width * height;
    for( size_t i = 0; i < bitmapSize; ++i )
    {
      pixbuf[i*4+0] = 0xFF * color.r;
      pixbuf[i*4+1] = 0xFF * color.g;
      pixbuf[i*4+2] = 0xFF * color.b;
      pixbuf[i*4+3] = 0xFF * color.a;
    }
    imageData.Update();
    image = ImageActor::New( imageData );
    image.SetParentOrigin( ParentOrigin::CENTER );
    return image;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    const Vector2 stageSize(stage.GetSize());

    // Create a quit button. (left of toolbar)
    Toolkit::PushButton quitButton = Toolkit::PushButton::New();
    quitButton.SetParentOrigin( ParentOrigin::TOP_LEFT );
    quitButton.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    quitButton.ClickedSignal().Connect( this, &ExampleController::OnQuitTouched );
    stage.Add( quitButton );

    // Create a effect toggle button. (right of toolbar)
    Toolkit::PushButton layoutButton = Toolkit::PushButton::New();
    layoutButton.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    layoutButton.SetAnchorPoint( AnchorPoint::TOP_RIGHT );
    layoutButton.ClickedSignal().Connect( this, &ExampleController::OnClearTouched );
    stage.Add( layoutButton );

    mStageWidth = stageSize.width;
    mStageHeight = stageSize.height;
    mDamageArea.x = mStageWidth;
    mDamageArea.y = mStageHeight;
    mDamageArea.width = 0;
    mDamageArea.height = 0;

    // Display the actor on the stage
    //mBitmapImage = BitmapImage::New(stageSize.width, stageSize.height);
    mCanvasImage = CreateBitmapActor( stageSize.width, stageSize.height ); //ImageActor::New( mBitmapImage );
    mCanvasImage.SetParentOrigin( ParentOrigin::CENTER );
    stage.Add( mCanvasImage );

    Clear();
    Update();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &ExampleController::OnTouch );
  }

  /**
   * Draws a rectangle into the bitmap of size width x height, with top-left corner position at x,y pixel coordinates.
   * @param[in] x The x (horizontal) pixel coordinate of where the left side of the rectangle begins.
   * @param[in] y The y (vertical) pixel coordinate of where the top side of the rectangle begins.
   * @param[in] width The width of the rectangle in pixels.
   * @param[in] height The height of the rectangle in pixels.
   * @param[in] color 32-bit unsigned int RGBA representation of color.
   */
  void DrawRectangle( unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int color )
  {
    BufferImage image = BufferImage::DownCast( mCanvasImage.GetImage() );
    //BitmapImage& image = mBitmapImage;
    mDamageArea.x = std::min(mDamageArea.x, x);
    mDamageArea.y = std::min(mDamageArea.y, y);
    mDamageArea.width = std::max(mDamageArea.width, x + width); // width = x2 for now
    mDamageArea.height = std::max(mDamageArea.height, y + height); // height = y2 for now

    unsigned int *p = reinterpret_cast<unsigned int*>(image.GetBuffer());
    p += y * image.GetBufferStride() / sizeof(unsigned int) + x;

    unsigned int gap = (image.GetBufferStride() / sizeof(unsigned int)) - width;
    for(y = 0; y < height; y++)
    {
      unsigned int *pEnd = p + width;

      while(p != pEnd)
      {
        *p++ = color;
      }

      p += gap;
    }
  }

  /**
   * Plots a pixel into the bitmap at x,y pixel coordinates in the canvas,
   * using color.
   * @param[in] x The x (horizontal) pixel coordinate, where 0 is left.
   * @param[in] y The y (vertical) pixel coordinate, where 0 is top.
   * @param[in] color 32-bit unsigned int RGBA representation of color.
   */
  void DrawPixel( unsigned int x, unsigned int y, unsigned int color )
  {
    //color |= 0x00ff00ff;
    //BitmapImage& image = mBitmapImage;
    BufferImage image = BufferImage::DownCast( mCanvasImage.GetImage() );
    // BUG: image.GetWidth()/Height returns 0, when I've clearly specified it's size.
    // while GetBufferStride gives the valid value.
    if(x < mStageWidth && y < mStageHeight)
    {
      mDamageArea.x = std::min(mDamageArea.x, x);
      mDamageArea.y = std::min(mDamageArea.y, y);
      mDamageArea.width = std::max(mDamageArea.width, x + 1); // width = x2 for now
      mDamageArea.height = std::max(mDamageArea.height, y + 1); // height = y2 for now
      unsigned int *p = reinterpret_cast<unsigned int*>(image.GetBuffer());
      p[y * image.GetBufferStride() / sizeof(unsigned int) + x] = color;
    }
  }
  void DrawPoint( unsigned int x, unsigned int y, unsigned int color )
  {
    const float pointGfx[5][5] =
    {
        { 0.0f, 0.2f, 1.0f, 0.2f, 0.0f, },
        { 0.2f, 0.8f, 1.0f, 0.8f, 0.2f, },
        { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, },
        { 0.2f, 0.8f, 1.0f, 0.8f, 0.2f, },
        { 0.0f, 0.2f, 1.0f, 0.2f, 0.0f, },
    };

    for( int iy = 0; iy < 5; ++iy )
    {
      for( int ix = 0; ix < 5; ++ix )
      {
        unsigned int alpha = (unsigned int)( pointGfx[ix][iy] * 255.0f );
        if( alpha > 0 )
        {
          unsigned int useColor = ( 0x000000ff |
              ( ( ( ( ( ( color & 0xff000000 ) >> 24 ) * alpha ) / 255 ) & 0xff ) << 24 ) |
              ( ( ( ( ( ( color & 0x00ff0000 ) >> 16 ) * alpha ) / 255 ) & 0xff ) << 16 ) |
              ( ( ( ( ( ( color & 0x0000ff00 ) >>  8 ) * alpha ) / 255 ) & 0xff ) <<  8 ) );
          //std::cout << "todor: usecol:" << alpha << std::endl;
          DrawPixel( ( x - 2 ) + ix, ( y - 2 ) + iy, useColor );
        }
      }
    }
  }
#if 1
  #define ipart_(X) ((int)(X))
  #define round_(X) ((int)(((double)(X))+0.5))
  #define fpart_(X) (((double)(X))-(double)ipart_(X))
  #define rfpart_(X) (1.0-fpart_(X))
  #define swap_(a, b) do{ __typeof__(a) tmp;  tmp = a; a = b; b = tmp; }while(0)
  inline void _dla_plot( int x, int y, Vector4 color, float br )
  {
    DrawPixel( x, y, ColorVector4ToRGBA( Vector4( color.r, color.g, color.b, br ) ) );
  }
  // Basic Wu line code.
  void DrawLine( int x1, int y1, int x2, int y2, Vector4 color )
  {
    int w = mStageWidth;
    int h = mStageHeight;
    if( x1 < 0 )
    {
      x1 = 0;
    }
    if( x2 < 0 )
    {
      x2 = 0;
    }
    if( y1 < 0 )
    {
      y1 = 0;
    }
    if( y2 < 0 )
    {
      y2 = 0;
    }
    if( x1 >= w )
    {
      x1 = w - 1;
    }
    if( x2 >= w )
    {
      x2 = w - 1;
    }
    if( y1 >= h )
    {
      y1 = h - 1;
    }
    if( y2 >= h )
    {
      y2 = h - 1;
    }
    double dx = (double)x2 - (double)x1;
    double dy = (double)y2 - (double)y1;
    if ( fabs(dx) > fabs(dy) ) {
      if ( x2 < x1 ) {
        swap_(x1, x2);
        swap_(y1, y2);
      }
      double gradient = dy / dx;
      double xend = round_(x1);
      double yend = y1 + gradient*(xend - x1);
      double xgap = rfpart_(x1 + 0.5);
      int xpxl1 = xend;
      int ypxl1 = ipart_(yend);
      _dla_plot(xpxl1, ypxl1, color, rfpart_(yend)*xgap);
      _dla_plot(xpxl1, ypxl1+1, color, fpart_(yend)*xgap);
      double intery = yend + gradient;

      xend = round_(x2);
      yend = y2 + gradient*(xend - x2);
      xgap = fpart_(x2+0.5);
      int xpxl2 = xend;
      int ypxl2 = ipart_(yend);
      _dla_plot(xpxl2, ypxl2, color, rfpart_(yend) * xgap);
      _dla_plot(xpxl2, ypxl2 + 1, color, fpart_(yend) * xgap);

      int x;
      for(x=xpxl1+1; x <= (xpxl2-1); x++) {
        _dla_plot(x, ipart_(intery), color, rfpart_(intery));
        _dla_plot(x, ipart_(intery) + 1, color, fpart_(intery));
        intery += gradient;
      }
    } else {
      if ( y2 < y1 ) {
        swap_(x1, x2);
        swap_(y1, y2);
      }
      double gradient = dx / dy;
      double yend = round_(y1);
      double xend = x1 + gradient*(yend - y1);
      double ygap = rfpart_(y1 + 0.5);
      int ypxl1 = yend;
      int xpxl1 = ipart_(xend);
      _dla_plot(xpxl1, ypxl1, color, rfpart_(xend)*ygap);
      _dla_plot(xpxl1, ypxl1+1, color, fpart_(xend)*ygap);
      double interx = xend + gradient;

      yend = round_(y2);
      xend = x2 + gradient*(yend - y2);
      ygap = fpart_(y2+0.5);
      int ypxl2 = yend;
      int xpxl2 = ipart_(xend);
      _dla_plot(xpxl2, ypxl2, color, rfpart_(xend) * ygap);
      _dla_plot(xpxl2, ypxl2 + 1, color, fpart_(xend) * ygap);

      int y;
      for(y=ypxl1+1; y <= (ypxl2-1); y++) {
        _dla_plot(ipart_(interx), y, color, rfpart_(interx));
        _dla_plot(ipart_(interx) + 1, y, color, fpart_(interx));
        interx += gradient;
      }
    }
  }
  #undef swap_
  #undef ipart_
  #undef fpart_
  #undef round_
  #undef rfpart_
#else
  void DrawLine( int X0, int Y0, int X1, int Y1, int BaseColor )
  {
     short NumLevels = 256;
     unsigned short IntensityBits = 8;

     unsigned short IntensityShift, ErrorAdj, ErrorAcc;
     unsigned int ErrorAccTemp, Weighting, WeightingComplementMask;
     int DeltaX, DeltaY, Temp, XDir;

     /* Make sure the line runs top to bottom */
     if (Y0 > Y1) {
        Temp = Y0; Y0 = Y1; Y1 = Temp;
        Temp = X0; X0 = X1; X1 = Temp;
     }
     /* Draw the initial pixel, which is always exactly intersected by
        the line and so needs no weighting */
     DrawPixel(X0, Y0, BaseColor);

     if ((DeltaX = X1 - X0) >= 0) {
        XDir = 1;
     } else {
        XDir = -1;
        DeltaX = -DeltaX; /* make DeltaX positive */
     }
     /* Special-case horizontal, vertical, and diagonal lines, which
         require no weighting because they go right through the center of
         every pixel */
      if ((DeltaY = Y1 - Y0) == 0) {
         /* Horizontal line */
         while (DeltaX-- != 0) {
            X0 += XDir;
            DrawPixel(X0, Y0, BaseColor);
         }
         return;
      }
      if (DeltaX == 0) {
         /* Vertical line */
         do {
            Y0++;
            DrawPixel(X0, Y0, BaseColor);
         } while (--DeltaY != 0);
         return;
      }
      if (DeltaX == DeltaY) {
         /* Diagonal line */
         do {
            X0 += XDir;
            Y0++;
            DrawPixel(X0, Y0, BaseColor);
         } while (--DeltaY != 0);
         return;
      }
     /* Line is not horizontal, diagonal, or vertical */
     ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
     /* # of bits by which to shift ErrorAcc to get intensity level */
     IntensityShift = 16 - IntensityBits;
     /* Mask used to flip all bits in an intensity weighting, producing the
        result (1 - intensity weighting) */
     WeightingComplementMask = NumLevels - 1;
     /* Is this an X-major or Y-major line? */
     if (DeltaY > DeltaX) {
        /* Y-major line; calculate 16-bit fixed-point fractional part of a
           pixel that X advances each time Y advances 1 pixel, truncating the
           result so that we won't overrun the endpoint along the X axis */
        ErrorAdj = ((unsigned long) DeltaX << 16) / (unsigned long) DeltaY;
        /* Draw all pixels other than the first and last */
        while (--DeltaY) {
           ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
           ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
           if (ErrorAcc <= ErrorAccTemp) {
              /* The error accumulator turned over, so advance the X coord */
              X0 += XDir;
           }
           Y0++; /* Y-major, so always advance Y */
           /* The IntensityBits most significant bits of ErrorAcc give us the
              intensity weighting for this pixel, and the complement of the
              weighting for the paired pixel */
           Weighting = ErrorAcc >> IntensityShift;
           DrawPixel(X0, Y0, BaseColor + Weighting);
           DrawPixel(X0 + XDir, Y0,
                 BaseColor + (Weighting ^ WeightingComplementMask));
        }
        /* Draw the final pixel, which is
           always exactly intersected by the line
           and so needs no weighting */
        DrawPixel(X1, Y1, BaseColor);
        return;
     }
     /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
        pixel that Y advances each time X advances 1 pixel, truncating the
        result to avoid overrunning the endpoint along the X axis */
     ErrorAdj = ((unsigned long) DeltaY << 16) / (unsigned long) DeltaX;
     /* Draw all pixels other than the first and last */
     while (--DeltaX) {
        ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
        ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
        if (ErrorAcc <= ErrorAccTemp) {
           /* The error accumulator turned over, so advance the Y coord */
           Y0++;
        }
        X0 += XDir; /* X-major, so always advance X */
        /* The IntensityBits most significant bits of ErrorAcc give us the
           intensity weighting for this pixel, and the complement of the
           weighting for the paired pixel */
        Weighting = ErrorAcc >> IntensityShift;
        DrawPixel(X0, Y0, BaseColor + Weighting);
        DrawPixel(X0, Y0 + 1,
              BaseColor + (Weighting ^ WeightingComplementMask));
     }
     /* Draw the final pixel, which is always exactly intersected by the line
        and so needs no weighting */
     DrawPixel(X1, Y1, BaseColor);
  }
#endif

  /**
   * Clears the canvas to Black
   */
  void Clear()
  {
    DrawRectangle( 0, 0, mStageWidth, mStageHeight, RGBA_BLACK );
    Update();
  }

  /**
   * Updates the canvas texture with the changes that have occurred
   */
  void Update()
  {
    mDamageArea.width -= mDamageArea.x;
    mDamageArea.height -= mDamageArea.y;
    if(mDamageArea.width != 0 && mDamageArea.height != 0)
    {
      BufferImage image = BufferImage::DownCast( mCanvasImage.GetImage() );
      image.Update( mDamageArea );
    }
    mDamageArea.x = mStageWidth;
    mDamageArea.y = mStageHeight;
    mDamageArea.width = 0;
    mDamageArea.height = 0;
  }

  /**
   * Returns height (y) for a given point (x) on a peak.
   * The peak is shaped like a cosine, it's width can be
   * adjusted by scale.
   * @param[in] x Horizontal position on peak (a value from 0.0 to 1.0,
   * peak repeats at (peak + N, where N is an any integer).
   * @param[in] peak The Horizontal position where the peak lies
   * @param[in] scale The scale factor (width scale) of peak.
   * @return peak height (value from 0 lowest, to 1.0 highest is returned)
   */
  float GetPeak(float x, float peak, float scale = 2.0f)
  {
    x -= peak;
    return cosf(x * Math::PI * scale) * 0.5f + 0.5f;
  }

  /**
   * Returns a rainbow color. Where peak values are:
   * 0 degrees = Red, 120 degrees = Green, 240 degrees = Blue
   * @param[in] hue The hue of the color from 0 (0 degrees) to 1.0 (360 degrees)
   * @return The fully saturated color representing the hue is returned
   */
  Vector4 GetRainbowColor(float hue)
  {
    float r = GetPeak(hue, 0.0f);
    float g = GetPeak(hue, 0.3333f);
    float b = GetPeak(hue, 0.6667f);

    return Vector4(r, g, b, 1.0f);
  }

  /**
   * Signal handler, called whenever user touches screen.
   * Draws on canvas based on touch position.
   *
   * @param[in] actor The actor that was pressed.
   * @param[in] touch Touch event information.
   * @return Whether to consume event or not.
   */
  bool OnTouch( Actor TEST_actor, const TouchEvent& TEST_touch )
  {
    unsigned int ms = GetMs();
    //std::cout << "todordel: OnTouch: " << ms << ", " << ms - mLastMs << std::endl; //todor log time diff
    mLastMs = ms;

    TouchPoint TEST_point = TEST_touch.points[0];
    if( TEST_point.state == TouchPoint::Down )
    {
      Clear();
      mTESTcollectPan.clear();
      mTESTcollectPan.push_back( TEST_touch );
    }
    else if( TEST_point.state == TouchPoint::Up )
    {
      ShowPan();
    }
    else
    {
      mTESTcollectPan.push_back( TEST_touch );
    }

    return true;
  }

  void ShowPan() // Chopin, get it?
  {
    std::cout << "todor:ShowPan" << std::endl;
    if( TEST_SIMULATE_INPUT_STARVATION_AT_ZERO_VELOCITY )
    {
      //todor
    }
    bool TEST_firstPoint = false;
    mTEST_eventsToProvide = 1;
    int itCount = 0;
    for( GestureStore::iterator it = mTESTcollectPan.begin(); it != mTESTcollectPan.end(); ++it )
    {
      std::cout << "todor:ShowPan: iter:" << itCount++ << std::endl;
      TouchEvent TEST_touch = *it;
      //todor fst: const TouchPoint& TEST_point = TEST_touch.points[0];
      TouchPoint TEST_point = TEST_touch.points[0];

      PanInfo TEST_panInfo;
      if( TEST_point.state == TouchPoint::Down )
      {
        //todor del Clear();
        mTouchStartTime = TEST_touch.time;
        mTouchPositionHistory.Clear();
        mTouchPositionLastTime = 0.0f;
  #ifdef TEST_OPTS_USELOCAL
        mLastPoint = TEST_point.local;
  #else
        mLastPoint = TEST_point.screen;
  #endif
        mLastPointPredict = mLastPoint;

        TEST_panInfo.state = Gesture::Started;
      }
      else if( TEST_point.state == TouchPoint::Up )
      {
        TEST_panInfo.state = Gesture::Finished;
      }
      else
      {
        TEST_panInfo.state = Gesture::Continuing;
      }

      std::cout << "todor:ShowPan: gh1" << std::endl;
      //std::cout << "todordel: gh0-1" << std::endl;
      // Use the TouchPositionHistory to keep track of each touch position.
      // Then plot the interpolated points between each point by making small increments
      // of time (0.1ms)
      float TEST_time( TEST_touch.time - mTouchStartTime );
      mTouchPositionHistory.Add( TEST_time, TEST_point.screen );

      float timeDiff( TEST_touch.time - mLastTouch.time );

      //todor fst const TouchPoint& TEST_lastPoint = mLastTouch.points[0];
      TouchPoint TEST_lastPoint = TEST_point;

      // If last point exists, use it.
      if( TEST_panInfo.state != Gesture::Started )
      {
        if( mLastTouch.GetPointCount() < 1 )
        {
          std::cout << "....................................=========================== no points in lasttouch: " << std::endl;
        }
        else
        {
          std::cout << "todor:ShowPan: gh1-1" << std::endl;
          TEST_lastPoint = mLastTouch.points[0];
          std::cout << "todor:ShowPan: gh1-2" << std::endl;
        }
      }

      TEST_panInfo.time = TEST_touch.time;

      TEST_panInfo.local.position = TEST_point.local;
      TEST_panInfo.screen.position = TEST_point.screen;
      TEST_panInfo.local.displacement = TEST_point.local - TEST_lastPoint.local;
      TEST_panInfo.screen.displacement = TEST_point.screen - TEST_lastPoint.screen;

      if( timeDiff > 0.0f )
      {
        TEST_panInfo.local.velocity = TEST_panInfo.local.displacement / timeDiff;
        TEST_panInfo.screen.velocity = TEST_panInfo.screen.displacement / timeDiff;
      }
      else
      {
        TEST_panInfo.local.velocity = Vector2::ZERO;
        TEST_panInfo.screen.velocity = Vector2::ZERO;
      }

      std::cout << "todor:ShowPan: gh2" << std::endl;
      //std::cout << "todordel: gh0-1-2" << std::endl;
      mGestures[ mWritePosition ] = TEST_panInfo;
      ++mWritePosition;
      mWritePosition %= PAN_GESTURE_HISTORY;

      bool TEST_doPrediction = false;


      //std::cout << "todordel: gh0-2" << std::endl;
      // Get number of gestures in circular buffer
      // (doing this the slow way so we make no assumptions that the algo is doing the right thing).
      unsigned int TEST_GesturesWaiting = 0;
      for( unsigned int TEST_readPos = mReadPosition; TEST_readPos != mWritePosition; ++TEST_GesturesWaiting)
      {
        ++TEST_readPos;
        TEST_readPos %= PAN_GESTURE_HISTORY;
      }
      // If we are deliberately buffering, wait for enough events before giving them to the algorithm.
      if( TEST_GesturesWaiting >= mTEST_eventsToProvide )
      {
        TEST_doPrediction = true;

#ifdef TEST_OPTS_90HZ_INPUT
        mTEST_eventsToProvide = 1;
        if( ( mFrameNumber % 2 ) == 1 )
        {
          mTEST_eventsToProvide = 2;
        }
        else
        {
          mTEST_eventsToProvide = 1;
        }
        mFrameNumber++;
        std::cout << "todor: THIS FRAME: " << mTEST_eventsToProvide << std::endl;
#endif
      }

      PanInfo TEST_predictPoint;
      //TEST_drawToPredict = mLastPointPredict;

      //std::cout << "todordel: gh0-3" << std::endl;
  #if 1
      // Bypass prediction code.
      TEST_predictPoint.local.position = TEST_panInfo.local.position;
      TEST_predictPoint.screen.position = TEST_panInfo.screen.position;
  #endif

      std::cout << "todor:ShowPan: gh3" << std::endl;
      if( TEST_doPrediction )
      {
        std::cout << "todor: DOING PREDICTION WITH POINTS: " << TEST_GesturesWaiting << std::endl;
        // ####################################################################################################
        // INPUT:  mGestures      ( Contains PanInfo structs ).
        // OUTPUT: TEST_predictPoint  ( PanInfo )
        // ####################################################################################################

        switch( mTEST_predictionMode )
        {
          case TEST_PREDICTION_CODE_NONE:
          {
            TEST_predictPoint = PredictionCodeNone( mLastTouch.time, TEST_touch.time );
            break;
          }
          case TEST_PREDICTION_CODE_DALI_DEFAULT:
          {
            TEST_predictPoint = PredictionCodeDaliDefault( mLastTouch.time, TEST_touch.time );
            break;
          }
          case TEST_PREDICTION_CODE_TEST_1:
          {
            TEST_predictPoint = PredictionCodeTest1( mLastTouch.time, TEST_touch.time );
            break;
          }
        }

        // Results are drawn next.
      }
      std::cout << "todor:ShowPan: gh4" << std::endl;

  #ifdef TEST_OPTS_USELOCAL
      Vector2 TEST_drawToRaw( TEST_point.local );
      Vector2 TEST_drawToPredict( TEST_predictPoint.local.position );
  #else
      Vector2 TEST_drawToRaw( TEST_point.screen );
      Vector2 TEST_drawToPredict( predictPoint.screen.position );
  #endif

      if( !TEST_firstPoint )
      {
        // Draw raw input.
    #ifdef TEST_DRAW_PIXEL_ONLY
        DrawPixel( TEST_drawToRaw.x, TEST_drawToRaw.y, 0xffffffff );
    #else
        DrawLine( mLastPoint.x, mLastPoint.y, TEST_drawToRaw.x, TEST_drawToRaw.y, Vector4( 0.9f, 0.75f, 0.0f, 1.0f ) );
    #endif
        DrawPoint( mLastPoint.x, mLastPoint.y, 0xffff88ff );
#if 0
        DrawPixel( mLastPoint.x, mLastPoint.y, 0xffff88ff );
        DrawPixel( mLastPoint.x - 1, mLastPoint.y, 0xffff88ff );
        DrawPixel( mLastPoint.x, mLastPoint.y - 1, 0xffff88ff );
        DrawPixel( mLastPoint.x + 1, mLastPoint.y, 0xffff88ff );
        DrawPixel( mLastPoint.x, mLastPoint.y + 1, 0xffff88ff );
#endif
        std::cout << "todor:ShowPan: gh5: PREDACT:" << TEST_doPrediction << ", ptx:" << TEST_drawToPredict.x << std::endl;

        if( TEST_doPrediction )
        {
          // Predicted input.
    #ifdef TEST_DRAW_PIXEL_ONLY
          DrawPixel( TEST_drawToPredict.x, TEST_drawToPredict.y, 0xff00ffff );
    #else
          DrawLine( mLastPointPredict.x, mLastPointPredict.y, TEST_drawToPredict.x, TEST_drawToPredict.y, Vector4( 1.0f, 0.0f, 10.0f, 1.0f ) );
    #endif
          DrawPoint( mLastPointPredict.x, mLastPointPredict.y, 0x88ffffff );
#if 0
          DrawPixel( mLastPointPredict.x, mLastPointPredict.y, 0x88ffffff );
          DrawPixel( mLastPointPredict.x - 1, mLastPointPredict.y, 0x88ffffff );
          DrawPixel( mLastPointPredict.x, mLastPointPredict.y - 1, 0x88ffffff );
          DrawPixel( mLastPointPredict.x + 1, mLastPointPredict.y, 0x88ffffff );
          DrawPixel( mLastPointPredict.x, mLastPointPredict.y + 1, 0x88ffffff );
#endif
          mLastPointPredict = TEST_drawToPredict;
        }
      }
      else
      {
        TEST_firstPoint = false;
      }

      // Automatic distance comparison:
      mStatsPredictionHistory.push_back( TEST_predictPoint );
      PanInfoHistoryIter TEST_statsPredictionIterator = mStatsPredictionHistory.begin();
      PanInfo TEST_statsComparePrediction;
      bool TEST_performCompare = false;

      std::cout << "todor:ShowPan: gh6" << std::endl;
      while( TEST_statsPredictionIterator != mStatsPredictionHistory.end() )
      {
        PanInfo TEST_currentGesture = *TEST_statsPredictionIterator;
        // Check how far in the past the gesture is.
        unsigned int TEST_timeInPast = TEST_touch.time - TEST_currentGesture.time;
        if( TEST_timeInPast > TEST_COMPARE_PAST_PREDICTION_TIME )
        {
          // Too far in past, remove entry.
          TEST_statsPredictionIterator = mStatsPredictionHistory.erase( TEST_statsPredictionIterator );
        }
        else if( TEST_timeInPast < ( TEST_COMPARE_PAST_PREDICTION_TIME - TEST_COMPARE_PAST_PREDICTION_TIME_WINDOW ) )
        {
          // Too far in future, we have no entries to compare, skip comparison.
          break;
        }
        else
        {
          // Entry is comparable, perform the comparison.
          TEST_performCompare = true;
          TEST_statsComparePrediction = TEST_currentGesture;
          break;
        }
      }

      TEST_performCompare = false;//todor force disable
      if( TEST_performCompare )
      {
        float TEST_DifferenceX = 0.0f;
        float TEST_VelocityAtPredictionX = 0.0f;

  #ifdef TEST_OPTS_USELOCAL
        TEST_DifferenceX = TEST_point.local.x - TEST_statsComparePrediction.local.position.x;
        TEST_VelocityAtPredictionX = TEST_statsComparePrediction.local.velocity.x;
  #else
        TEST_DifferenceX = TEST_point.screen.x - TEST_statsComparePrediction.screen.position.x;
        TEST_VelocityAtPredictionX = TEST_statsComparePrediction.screen.velocity.x;
  #endif
        std::cout << "todor: Prediction behind input by " << TEST_DifferenceX <<
            " when at a velocity of ~" << TEST_VelocityAtPredictionX << std::endl;
      }

  #if 0
      //todor try get in future?
      // Render samples from last time to present time to demonstrate history graph smoothing.
      while( mTouchPositionLastTime < time )
      {
        Vector2 position = mTouchPositionHistory.Get(mTouchPositionLastTime);
        Vector4 color = GetRainbowColor(mTouchPositionLastTime * 0.01f);
        DrawPixel( position.x, position.y, ColorVector4ToRGBA( color ) );

        mTouchPositionLastTime += 0.1f;
      }
  #endif

      mLastTouch = TEST_touch;
      mLastPoint = TEST_drawToRaw;
      //todor del mLastPointPredict = TEST_drawToPredict;
    }

    std::cout << "todor:ShowPan: gh7" << std::endl;
    Update();
    std::cout << "todor:ShowPan: gh8" << std::endl;
  }

  /**
   * Signal handler, called when the 'Quit' button has been touched.
   *
   * @param[in] button The button that was pressed.
   * @return Whether to consume event or not.
   */
  bool OnQuitTouched( Dali::Toolkit::Button button )
  {
    mApplication.Quit();
    return true;
  }

  /**
   * Signal handler, called when the 'Clear' button has been touched.
   *
   * @param[in] button The button that was pressed.
   * @return Whether to consume event or not.
   */
  bool OnClearTouched( Dali::Toolkit::Button button )
  {
    Clear();
    mReadPosition = 0;
    mWritePosition = 0;
    mPanHistory.clear();
    mPredictionHistory.clear();
    mStatsPredictionHistory.clear();
    mInGesture = false;

    return true;
  }

private:

  Application&  mApplication;
  Layer mContentLayer;                          ///< The content layer (contains game actors)
  //View mView;                                   ///< The View instance.
  //ToolBar mToolBar;                             ///< The View's Toolbar.

  unsigned long mTouchStartTime;                ///< Keep track of touch time when user touched device
  History<Vector2> mTouchPositionHistory;       ///< Keep track of all the touch positions for the past 1 second roughly.
  float mTouchPositionLastTime;                 ///< Keep track of the last touch-time

  unsigned int mStageWidth;                     ///< Stage dimensions (width)
  unsigned int mStageHeight;                    ///< Stage dimensions (height)
  RectArea mDamageArea;                         ///< Damage area since last update. (it's faster to update a sub-texture than whole)
  //BitmapImage mBitmapImage;                     ///< The bitmap image for the canavas to be drawn on.
  ImageActor mCanvasImage;                      ///< The canvas ImageActor (contains mBitmapImage).


  Vector2 mLastPoint;
  Vector2 mLastPointPredict;
  TouchEvent mLastTouch;
  unsigned int mFrameNumber;
  PanInfoHistory mStatsPredictionHistory;
  int mTEST_predictionMode;

  // Vars from scene-graph-pan-gesture:
  PanInfoHistory mPanHistory;
  PanInfoHistory mPredictionHistory;
  PanInfo mGestures[PAN_GESTURE_HISTORY];     ///< Circular buffer storing recent gestures.
  PanInfo mLastGesture;                       ///< The last gesture. (last update frame).
  //PanInfo mTargetGesture;                     ///< The most recent input gesture, if the current used gesture does not match.
  PanInfo mLastUnmodifiedGesture;             ///< The last gesture before any processing was done on it.
  unsigned int mWritePosition;                ///< The next PanInfo buffer to write to. (starts at 0).
  unsigned int mReadPosition;                 ///< The next PanInfo buffer to read. (starts at 0).
  bool mNotAtTarget;                          ///< Keeps track of if the last gesture used was the most recent received.
  bool mInGesture;                            ///< True if the gesture is currently being handled i.e. between Started <-> Finished/Cancelled.
  PredictionMode mPredictionMode;             ///< The pan gesture prediction mode
  unsigned int  mPredictionAmount;            ///< how far into future to predict in milliseconds
  unsigned int  mCurrentPredictionAmount;     ///< the current prediction amount used by the prediction algorithm
  unsigned int  mMaxPredictionAmount;         ///< the maximum prediction amount used by the prediction algorithm
  unsigned int  mMinPredictionAmount;         ///< the minimum prediction amount used by the prediction algorithm
  unsigned int  mPredictionAmountAdjustment;  ///< the prediction amount to adjust in milliseconds when pan velocity changes
  SmoothingMode mSmoothingMode;               ///< The pan gesture prediction mode
  float         mSmoothingAmount;             ///< How much smoothing to apply [0.0f,1.0f]

  // Test vars - new - used in TEST_PREDICTION_CODE_1:
  typedef enum
  {
    IDLE,
    IN_GESTURE,
    END_GESTURE_QUEUED,
  } GestureState;
  GestureState mGestureState; ///<todor
  PanInfo mLastGestureResult; ///<todor
  Vector2 mTMPlast;
  unsigned int mStarvationCount;
  unsigned int mStarvationMaximum;
  Vector2 mLastVelocityLocal; //todor init
  Vector2 mLastVelocityScreen;
  Vector2 mLastAccelerationLocal; //todor init
  Vector2 mLastAccelerationScreen;
  PanInfo mLastReadTemp;
  PanInfo mReadGestures[PAN_GESTURE_HISTORY];     ///< Linear buffer storing the most recent gestures.
  PanInfo mLastNonPredictedResult;
  Vector2 mLastAccelerationLocalPast;
  Vector2 mLastAccelerationScreenPast;

  // Misc:
  PanInfo mTargetGesture;
  PanInfo mLastFrameReadGesture;

  // Other test vars:
  unsigned int mLastMs;
  GestureStore mTESTcollectPan;
  unsigned int mTEST_eventsToProvide;
};

// Entry point for Linux & SLP applications
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ExampleController test( application );
  application.MainLoop();
  return 0;
}

#endif

#ifdef DEMO_X11DIRECT
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
#include <iostream>
using namespace std;

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

int main( int argc, char **argv )
{
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
}
#endif

#ifdef DEMO_SCROLLVIEW // DALi demo type: Basic ScrollView
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
using namespace Dali;
using namespace Dali::Toolkit;
#include <iostream>
using namespace std;

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
    mScrollViewContainer.SetSize(Vector2( stageSize.width * 3.0f, stageSize.height ) );

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
    ImageActor test1 = ImageActor::New( ResourceImage::New( DALI_IMAGE_DIR "homescreen1.png" ) );
    ImageActor test2 = ImageActor::New( ResourceImage::New( DALI_IMAGE_DIR "homescreen2.png" ) );
    ImageActor test3 = ImageActor::New( ResourceImage::New( DALI_IMAGE_DIR "homescreen3.png" ) );
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

    //todor
    m_ScrollView.Add( mScrollViewContainer );//todor add to scrollview
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

int main( int argc, char **argv )
{
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);
  HelloWorldController test( app );
  app.MainLoop();
}
#endif

#ifdef DEMO_TOUCHROOT // DALi demo type: Touch events on root
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
using namespace Dali;
using namespace Dali::Toolkit;
#include <iostream>
using namespace std;

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

int main( int argc, char **argv )
{
  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);
  HelloWorldController test( app );
  app.MainLoop();
}
#endif // DALi demo type




