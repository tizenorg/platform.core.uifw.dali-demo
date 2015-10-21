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
#include <dali/public-api/common/dali-common.h>

using namespace Dali;
using namespace Dali::Toolkit;


namespace
{

const std::string RESOURCE_IMAGE_PATH( DALI_IMAGE_DIR "icon-insert-selected.png" );

unsigned int gRowsPerPage(25);
unsigned int gColumnsPerPage( 25 );

}
// Test application to measure performance of ImageView with TextLabel
// By default, the application consist of 10 pages of 25x25 ImageView, this can be modified using the following command line arguments:
// -r NumberOfRows  (Modifies the number of rows per page)
// -c NumberOfColumns (Modifies the number of columns per page)

//
class Benchmark : public ConnectionTracker
{
public:

  Benchmark( Application& application )
: mApplication( application ),
  mRowsPerPage( gRowsPerPage ),
  mColumnsPerPage( gColumnsPerPage )
{
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &Benchmark::Create );
}

  ~Benchmark()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &Benchmark::OnTouch );

    CreateImageViews();
  }

  void CreateImageViews()
  {
    Stage stage = Stage::GetCurrent();
    unsigned int actorCount(mRowsPerPage*mColumnsPerPage);
    mImageView.resize(actorCount);
    mTextLabel.resize(actorCount);

    Vector2 stageSize = stage.GetSize();
    mSize = Vector3( stageSize.x / mColumnsPerPage, stageSize.y / mRowsPerPage, 0.0f );

    for( size_t i(0); i<mColumnsPerPage; ++i )
    {
      float xpos = mSize.x * i;

      for( size_t j(0); j<mRowsPerPage; ++j)
      {
        float ypos = mSize.y * j;

        size_t index = j * mColumnsPerPage + i;

        mImageView[index] = ImageView::New(RESOURCE_IMAGE_PATH);
        mImageView[index].SetSize(mSize);
        mImageView[index].SetPosition(xpos+mSize.x*0.5f, ypos+mSize.y*0.5f, 0.0f);
        mImageView[index].SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

        float color_r = static_cast<float>(i % 5) / 5.0f;
        float color_g = static_cast<float>(j % 10) / 10.0f;
        float color_b = static_cast<float>( ( i + j ) % 5 ) / 5.0f;
        mImageView[index].SetBackgroundColor(Vector4(color_r, color_g, color_b, 1.0f));

        stage.Add(mImageView[index]);
/*
        mTextLabel[index] = TextLabel::New( "Text" );
        mTextLabel[index].SetProperty(TextLabel::Property::POINT_SIZE, (mSize.x + mSize.y) * 0.1f);
        mTextLabel[index].SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
        mTextLabel[index].SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
        mTextLabel[index].SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
*/
//        mImageView[index].Add(mTextLabel[index]);
      }
    }
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
//    mApplication.Quit();

    Animation animation = Animation::New( 10.0f );
    animation.AnimateBy( Property( mImageView[0], Actor::Property::COLOR ), Vector4(1.0f, 1.0f, 1.0f, 1.0f) );
    animation.SetLooping( true );
    animation.Play();

    return true;
  }

private:
  Application&  mApplication;

  std::vector<TextLabel>  mTextLabel;
  std::vector<ImageView>  mImageView;

  Vector3             mSize;
  unsigned int        mRowsPerPage;
  unsigned int        mColumnsPerPage;
};

void RunTest( Application& application )
{
  Benchmark test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  for( int i(1) ; i < argc; ++i )
  {
    std::string arg( argv[i] );
    if( arg.compare(0, 2, "-r" ) == 0)
    {
      gRowsPerPage = atoi( arg.substr( 2, arg.size()).c_str());
    }
    else if( arg.compare(0, 2, "-c" ) == 0)
    {
      gColumnsPerPage = atoi( arg.substr( 2, arg.size()).c_str());
    }
  }

  RunTest( application );

  return 0;
}
