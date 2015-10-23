/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <stdio.h>
#include <math.h>
#include <dali-toolkit/dali-toolkit.h>

using namespace std;
using namespace Dali;
using namespace Dali::Toolkit;


#define DPI1           6.0f
#define DPI2           5.0f
#define DPI3           3.0f
#define IMAGE_PATH DALI_IMAGE_DIR

#define SCREEN_WIDTH	1920
#define SCREEN_HEIGHT	1080

#define TITLE_WIDTH		1920
#define TITLE_HEIGHT	80

#define HEAD_WIDTH		1920
#define HEAD_HEIGHT		40

#define WIDGET_WIDTH	128
#define WIDGET_HEIGHT	160

#define FOLDER_WIDTH	128
#define FOLDER_HEIGHT	106
#define FOLDER_POS_Y	10

#define TEXT_POS_Y		120

int row ,col;
int no_actor = 200;

float factor = 7.8f;
int dir = 1;

float max_c(float a, float b)
{
	return a > b ? a : b;
}

float min_c(float a, float b)
{
	return a < b ? a : b;
}



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

  void on_timeline_new_frame( Animation& source )
  {
    float scroll_offset = SCREEN_WIDTH - col * WIDGET_WIDTH;

    Vector3 point = border.GetCurrentPosition();
    Vector3 point_widget = container_actor.GetCurrentPosition();

    if (point.x + WIDGET_WIDTH > SCREEN_WIDTH  || point.x < 0)
    		dir *= -1;

    point.x += dir*factor;
    dalyAni.AnimateTo(Property(border, Actor::Property::POSITION_X), point.x);

    if (point.x > dir * (SCREEN_WIDTH) / 2.0)
  	{
  		point_widget.x += -dir * 20.0f;
  	}

    point_widget.x = max_c(point_widget.x, scroll_offset);
  	point_widget.x = min_c(0.0f, point_widget.x);

    dalyAni.AnimateTo(Property(container_actor, Actor::Property::POSITION_X), point_widget.x);
    dalyAni.Play();
  }

  void setPosition()
  {
    Toolkit::Control titleBar, headerBar;
    TextLabel titleText, headerText;

  	//Header1
    titleBar = Control::New();
   	titleBar.SetParentOrigin(ParentOrigin::TOP_LEFT);
  	titleBar.SetAnchorPoint(AnchorPoint::TOP_LEFT);
	  titleBar.SetPosition(0, 0);
		titleBar.SetSize(TITLE_WIDTH, TITLE_HEIGHT);
    titleBar.SetBackgroundColor(Vector4(136/255.0f, 136/255.0f, 136/255.0f, 1.0f));
    stage.Add(titleBar);

  	//Header2
    headerBar = Control::New();
   	headerBar.SetParentOrigin(ParentOrigin::TOP_LEFT);
  	headerBar.SetAnchorPoint(AnchorPoint::TOP_LEFT);
	  headerBar.SetPosition(0, TITLE_HEIGHT);
		headerBar.SetSize(HEAD_WIDTH, HEAD_HEIGHT);
    headerBar.SetBackgroundColor(Vector4(64/255.0f, 64/255.0f, 64/255.0f, 1.0f));
    stage.Add(headerBar);

   	//Header Text
    titleText = TextLabel::New();
    titleText.SetParentOrigin(ParentOrigin::TOP_LEFT );
    titleText.SetAnchorPoint(AnchorPoint::TOP_LEFT );
    titleText.SetPosition(0, 0);
    titleText.SetSize(TITLE_WIDTH, TITLE_HEIGHT);
    titleText.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "BEGIN");
    titleText.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "TOP");
    titleText.SetProperty(TextLabel::Property::TEXT, "My Content");
    titleText.SetProperty(TextLabel::Property::FONT_FAMILY, "Sans");    
    titleText.SetProperty(TextLabel::Property::POINT_SIZE, 35.0f*DPI1);
    titleText.SetProperty(TextLabel::Property::TEXT_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    stage.Add(titleText);

    //Header Text
    headerText = TextLabel::New();
    headerText.SetParentOrigin( ParentOrigin::TOP_LEFT );
    headerText.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    headerText.SetPosition(0, TITLE_HEIGHT);
    headerText.SetSize(HEAD_WIDTH, HEAD_HEIGHT);
    headerText.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
    headerText.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
    headerText.SetProperty(TextLabel::Property::TEXT, "DataTraveler 2.0" );
    headerText.SetProperty(TextLabel::Property::FONT_FAMILY, "Sans");    
    headerText.SetProperty(TextLabel::Property::POINT_SIZE, 20.0f*DPI2);    
    headerText.SetProperty(TextLabel::Property::TEXT_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    stage.Add(headerText);

    //Load Image
    folderImage = IMAGE_PATH "folder_icon.png";
    bgColors[0] = Vector4(121.f/255.f, 61.f/255.f,181.f/255.f,1.f);
    bgColors[1] = Vector4(180.f/255.f, 77.f/255.f,166.f/255.f,1.f);
    bgColors[2] = Vector4( 37.f/255.f, 56.f/255.f, 86.f/255.f,1.f);
    bgColors[3] = Vector4( 39.f/255.f,149.f/255.f,136.f/255.f,1.f);
    bgColors[4] = Vector4( 71.f/255.f,120.f/255.f,197.f/255.f,1.f);

  	col = 2*SCREEN_WIDTH / WIDGET_WIDTH;
	  row = no_actor / col;
  	row = row < 0 ? 1 :row;

    printf("col: %d row: %d\n",col,row);

    container_actor = Control::New();
    container_actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
    container_actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    container_actor.SetPosition(0, TITLE_HEIGHT+HEAD_HEIGHT);
    container_actor.SetSize(col*WIDGET_WIDTH, row*WIDGET_HEIGHT);
    //container_actor.SetBackgroundColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    stage.Add( container_actor );



  	for(int i=0; i<no_actor; ++i)
  	{
  		int row_i = i / col;
  		int col_j = i % col;

  		Control     widget; 
      ImageView  actor_folder;
  		TextLabel   text;

      widget = Control::New();
      widget.SetParentOrigin( ParentOrigin::TOP_LEFT );
      widget.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      widget.SetPosition((float)(col_j * WIDGET_WIDTH), (float)(row_i * WIDGET_HEIGHT));
      widget.SetSize(WIDGET_WIDTH, WIDGET_HEIGHT);

      widget.SetBackgroundColor(bgColors[row_i%5]);

      container_actor.Add( widget );

#ifdef USE_LAYER
      Layer layer0, layer1, layer2;
      layer0 = Layer::New();
      layer0.SetParentOrigin( ParentOrigin::TOP_LEFT );
      layer0.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      layer0.SetPosition(0, 0);
      layer0.SetSize(WIDGET_WIDTH, WIDGET_HEIGHT);
      widget.Add(layer0);

      actor_bg_image = ImageActor::New(image[rand()%4+1]);
      actor_bg_image.SetParentOrigin(ParentOrigin::TOP_LEFT);
    	actor_bg_image.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  	  actor_bg_image.SetPosition(0, 0);
  		actor_bg_image.SetSize(WIDGET_WIDTH, WIDGET_HEIGHT);
      layer0.Add(actor_bg_image);


      layer1 = Layer::New();
      layer1.SetParentOrigin( ParentOrigin::TOP_LEFT );
      layer1.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  	  layer1.SetPosition(0, FOLDER_POS_Y);
  		layer1.SetSize(FOLDER_WIDTH, FOLDER_HEIGHT);
      widget.Add(layer1);

      actor_folder = ImageActor::New(image[0]);
      actor_folder.SetParentOrigin(ParentOrigin::TOP_LEFT);
    	actor_folder.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  	  actor_folder.SetPosition(0, 0);
  		actor_folder.SetSize(FOLDER_WIDTH, FOLDER_HEIGHT);
      layer1.Add(actor_folder);

      layer2 = Layer::New();
      layer2.SetParentOrigin( ParentOrigin::TOP_LEFT );
      layer2.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  	  layer2.SetPosition(0, TEXT_POS_Y);
  		layer2.SetSize(WIDGET_WIDTH, WIDGET_HEIGHT-FOLDER_HEIGHT);
      widget.Add(layer2);

      text = TextLabel::New();
      text.SetParentOrigin( ParentOrigin::TOP_LEFT );
      text.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      text.SetPosition(0, 0);
      text.SetSize(WIDGET_WIDTH, WIDGET_HEIGHT-FOLDER_HEIGHT);
      text.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
      text.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "TOP");
      text.SetProperty(TextLabel::Property::TEXT, "Sample Text" );
      text.SetProperty(TextLabel::Property::FONT_FAMILY, "Sans");    
      text.SetProperty(TextLabel::Property::POINT_SIZE, 16.0f*DPI3);    
      text.SetProperty(TextLabel::Property::TEXT_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
      layer2.Add(text);


      layer1.RaiseAbove(layer0);
      layer2.RaiseAbove(layer1);
#else

      actor_folder = ImageView::New(folderImage);
      actor_folder.SetParentOrigin(ParentOrigin::TOP_LEFT);
    	actor_folder.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  	  actor_folder.SetPosition(0, FOLDER_POS_Y, 0.1f);
  		actor_folder.SetSize(FOLDER_WIDTH, FOLDER_HEIGHT);
      widget.Add(actor_folder);

      text = TextLabel::New();
      text.SetParentOrigin( ParentOrigin::TOP_LEFT );
      text.SetAnchorPoint( AnchorPoint::TOP_LEFT );
      text.SetPosition(0, TEXT_POS_Y, 0.1f);
      text.SetSize(WIDGET_WIDTH, WIDGET_HEIGHT-FOLDER_HEIGHT);
      text.SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
      text.SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "TOP");
      text.SetProperty(TextLabel::Property::TEXT, "Sample Text" );
      text.SetProperty(TextLabel::Property::FONT_FAMILY, "Sans");    
      text.SetProperty(TextLabel::Property::POINT_SIZE, 16.0f*DPI3);    
      text.SetProperty(TextLabel::Property::TEXT_COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
      widget.Add(text);
#endif


     }


  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    stage = Stage::GetCurrent();

    setPosition();

    Layer border_Layer;

    border_Layer = Layer::New();
    border_Layer.SetParentOrigin( ParentOrigin::TOP_LEFT );
    border_Layer.SetAnchorPoint( AnchorPoint::TOP_LEFT );
	  border_Layer.SetPosition(0, TITLE_HEIGHT+HEAD_HEIGHT);
		border_Layer.SetSize(WIDGET_WIDTH, SCREEN_WIDTH);
    stage.Add(border_Layer);

    Property::Map borderProperty;
    borderProperty.Insert("renderer-type", "border-renderer");
    borderProperty.Insert("border-color", Color::RED );
    borderProperty.Insert("border-size", 9.0f);

    border = Control::New();
    border.SetProperty(Control::Property::BACKGROUND, borderProperty);
    border.SetParentOrigin(ParentOrigin::TOP_LEFT);
    border.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  	border.SetPosition(0, 0);
  	border.SetSize(WIDGET_WIDTH, WIDGET_HEIGHT);

    border_Layer.Add(border);
    border_Layer.RaiseToTop();

    dalyAni = Animation::New(1/1000.0f);
  	dalyAni.FinishedSignal().Connect(this, &HelloWorldController::on_timeline_new_frame);
    dalyAni.Play();

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
  Stage         stage;
  Control       container_actor;
  std::string   folderImage;
  Vector4       bgColors[5];
  Control       border;
  Animation     dalyAni;

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
	if(argc>1)
	{
		no_actor = atoi(argv[1]);
  }

  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
