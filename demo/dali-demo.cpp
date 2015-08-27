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

//#define DEMO_X11DIRECT
//#define DEMO_SCROLLVIEW
//#define DEMO_TOUCHROOT
#define DEMO_TESTALGO


#ifdef DEMO_TESTALGO
#include <stdio.h>
#include <set>
#include <limits>
#include <iostream>
#include <vector>

// INTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
//#include "../shared/view.h"

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;

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
// --------------------------- //
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
public:

  ExampleController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ExampleController::Create );
    mTouchPositionHistory.SetMaxSize( 60 ); // 60 touch-frames of history
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
    color |= 0x00ff00ff;
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
  void DrawLine( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, Vector4 color )
  {
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
  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    const TouchPoint& point = touch.points[0];

    if( point.state == TouchPoint::Down )
    {
      Clear();
      mPanHistory.clear();
      mTouchStartTime = touch.time;
      mTouchPositionHistory.Clear();
      mTouchPositionLastTime = 0.0f;
      mLastPointPosition = point.screen;
    }

    // Use the TouchPositionHistory to keep track of each touch position.
    // Then plot the interpolated points between each point by making small increments
    // of time (0.1ms)
    float time( touch.time - mTouchStartTime );
    mTouchPositionHistory.Add( time, point.screen );

    float timeDiff( touch.time - mLastTouch.time );
    const TouchPoint& lastPoint = touch.points[0];
    PanInfo panInfo;
    panInfo.time = touch.time;
    panInfo.state = point.state;
    panInfo.local.position = point.local;
    panInfo.local.displacement = point.local - mLastTouch.local;
    panInfo.local.velocity = panInfo.local.displacement / timeDiff;
    mPanHistory.push_back( panInfo );


    // Draw raw input.
    DrawLine( mLastPointPosition.x, mLastPointPosition.y, point.screen.x, point.screen.y, Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );


    // Predicted input.
    Vector2 pointPredict = mTouchPositionHistory.Get( time + 50.0f );
    DrawLine( mLastPointPredict.x, mLastPointPredict.y, pointPredict.x, pointPredict.y, Vector4( 1.0f, 0.0f, 10.0f, 1.0f ) );


#if 0
    //todor try get in future?
    // Render samples from last time to present time to demonstrate history graph smoothing.
    while(mTouchPositionLastTime < time)
    {
      Vector2 position = mTouchPositionHistory.Get(mTouchPositionLastTime);
      Vector4 color = GetRainbowColor(mTouchPositionLastTime * 0.01f);
      DrawPixel( position.x, position.y, ColorVector4ToRGBA( color ) );

      mTouchPositionLastTime += 0.1f;
    }
#endif


    Update();

    mLastTouch = touch;
    mLastPointPosition = point.screen;
    mLastPointPredict = pointPredict;

    return true;
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
    return true;
  }

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
  PanInfoHistory mPanHistory;















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
  Vector2 mLastPointPosition;
  Vector2 mLastPointPredict;

  TouchEvent mLastTouch;
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




