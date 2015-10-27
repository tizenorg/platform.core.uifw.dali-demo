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

#include <dali/dali.h>
#include <iostream>
#include <stdio.h>
#include <dali-toolkit/dali-toolkit.h>
#include <cstdarg>
#include <sstream>

using namespace Dali;
using namespace Dali::Toolkit;
using namespace std;

const char* SIMPLE_VERTEX_SHADER = DALI_COMPOSE_SHADER(
	void main()
	{
		vec4 pos = uProjection * uModelView * vec4(aPosition, 1.0);
		gl_Position = vec4(pos.x + 50., pos.y - 50., pos.zw);
		vTexCoord = aTexCoord;
	}
);

const char* SIMPLE_VERTEX_SHADER_UNIFORM = DALI_COMPOSE_SHADER(
  uniform float uTest;
	void main()
	{
		vec4 pos = uProjection * uModelView * vec4(aPosition, 1.0);
		gl_Position = vec4(pos.x + uTest, pos.y - uTest, pos.zw);
		vTexCoord = aTexCoord;
	}
);

const char* ALPHA_MASK_VERTEX_SHADER_SOURCE =
"uniform vec2 uImageSize;                                                       \n"
"uniform vec2 uMaskSize;                                                        \n"
"varying vec2 vMaskTexCoord;                                                    \n"
"                                                                               \n"
"void main()                                                                    \n"
"{                                                                              \n"
"  gl_Position = uMvpMatrix * vec4(aPosition, 1.0);                             \n"
"                                                                               \n"
"  // Ignore mask UVs for image                                                 \n"
"                                                                               \n"
"  highp vec2 halfImageSize = uImageSize * 0.5;                                 \n"
"  vTexCoord = (aPosition.xy + halfImageSize) / uImageSize;                     \n"
"                                                                               \n"
"  // UVs were calculated for image size, so convert for mask size              \n"
"                                                                               \n"
"  highp vec2 halfMaskSize  = uMaskSize * 0.5;                                  \n"
"  highp vec2 halfSizeDelta = halfImageSize - halfMaskSize;                     \n"
"                                                                               \n"
"  highp vec2 maskPosition = aPosition.xy;                                      \n"
"  maskPosition.x -= halfSizeDelta.x * sign(aPosition.x);                       \n"
"  maskPosition.y -= halfSizeDelta.y * sign(aPosition.y);                       \n"
"                                                                               \n"
"  vMaskTexCoord = (maskPosition + halfMaskSize) / uMaskSize;                   \n"
"}                                                                              \n";

/**
 * This fragment shader operates for premultiplied source image.
 * Application should use proper blend mode for unpremultiplying to use it
 */

const char* ALPHA_MASK_FRAGMENT_SHADER_SOURCE =
"varying mediump vec2 vMaskTexCoord;                                                    \n"
"                                                                               \n"
"void main()                                                                    \n"
"{                                                                              \n"
"  highp vec4 mask = texture2D(sEffect, vMaskTexCoord);                         \n"
"  gl_FragColor = texture2D(sTexture, vTexCoord) * vec4(uColor.rgb*uColor.a, uColor.a) * vec4(mask.a,mask.a,mask.a,mask.a); \n"
"}                                                                              \n";

const char* IMAGE_REGION_VERTEX_SHADER =
    "uniform mediump vec2 uTopLeft;\n"
    "uniform mediump vec2 uBottomRight;\n"
    "void main()\n"
    "{\n"
    "  mediump vec4 position = vec4(aPosition,1.0);\n"
    "  gl_Position = uMvpMatrix * position;\n"
    // The line below is doing the same as the following commented lines:
    //"  vec2 imageSize = sTextureRect.zw - sTextureRect.xy;\n"
    //"  vec2 topLeft = sTextureRect.xy + uTopLeft * imageSize;\n"
    //"  vec2 bottomRight = sTextureRect.xy + uBottomRight * imageSize;\n"
    //"  vec2 texCoord = (aTexCoord - sTextureRect.xy) / imageSize;\n"
    //"  vTexCoord = topLeft + texCoord * ( bottomRight - topLeft );\n"
    "  vTexCoord = sTextureRect.xy + uTopLeft * ( sTextureRect.zw - sTextureRect.xy ) + ( aTexCoord - sTextureRect.xy ) * ( uBottomRight - uTopLeft );\n"
    "}\n";

struct UnpremultiplyNinePatchMaskEffectSizeConstraint
{
  void operator()( Vector2& current, const PropertyInputContainer& property )
  {
    const Vector3& actorSize = property[0]->GetVector3();
    current = Vector2( actorSize.x, actorSize.y );
  }
};

const char* UNPREMULTIPLY_FRAGMENT_SHADER_SOURCE =
"void main()                                                                    \n"
"{                                                                              \n"
"  gl_FragColor = texture2D(sTexture, vTexCoord) * vec4(uColor.rgb*uColor.a, uColor.a); \n"
"}                                                                              \n";



// This example shows how to create and display Hello World! using a simple TextActor
//
class HelloWorldController : public ConnectionTracker
{
public:

  HelloWorldController( Application& application )
  : mApplication( application )
  {
    std::cout << "HelloWorldController::HelloWorldController" << std::endl;

    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HelloWorldController::Create );
  }

  ~HelloWorldController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    std::cout << "HelloWorldController::Create" << std::endl;

    Stage::GetCurrent().SetBackgroundColor(Vector4(1,1,1,1));

//#define IMAGE_ACTOR
//#define IMAGE_VIEW

#ifdef IMAGE_ACTOR
    //ImageActor imgAct = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-1.jpg"));
    ImageActor imgAct = ImageActor::New(ResourceImage::New(DALI_IMAGE_DIR "blocks-ball.png"));
    imgAct.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    imgAct.SetSize(200,200);
    Stage::GetCurrent().Add(imgAct);
#endif

#ifdef IMAGE_VIEW
    //ImageView imgView = ImageView::New(ResourceImage::New(DALI_IMAGE_DIR "gallery-large-1.jpg"));
    ImageView imgView = ImageView::New(ResourceImage::New(DALI_IMAGE_DIR "blocks-ball.png"));
    imgView.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    imgView.SetSize(200,200);
    imgView.SetPosition(0,300);
    Stage::GetCurrent().Add(imgView);
#endif

    // 1.simple vertex shader
    if(0)
    {
#ifdef IMAGE_ACTOR
      // ImageActor
      Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(SIMPLE_VERTEX_SHADER, "");
      imgAct.SetShaderEffect(shaderEffect);
#endif

#ifdef IMAGE_VIEW
      // ImageView
      imgView.SetShader(SIMPLE_VERTEX_SHADER, "");
#endif
    }

    // 2.simple vertex shader with uniform
    if(0)
    {
#ifdef IMAGE_ACTOR
      // ImageActor
      Dali::ShaderEffect shaderEffect =  Dali::ShaderEffect::New(SIMPLE_VERTEX_SHADER_UNIFORM, "");
      shaderEffect.SetUniform("uTest", 200.f);
      imgAct.SetShaderEffect(shaderEffect);
#endif

#ifdef IMAGE_VIEW
      // ImageView
      imgView.SetShader(SIMPLE_VERTEX_SHADER_UNIFORM, "");
      Property::Index uTestIndex = imgView.RegisterProperty("uTest", 100.f);
      imgView.MapUniform(uTestIndex, "uTest");
#endif
    }

    // 3.nine patch masking
    if(0)
    {
#ifdef IMAGE_ACTOR
      //////////////////////////////
      // ImageActor
      string maskImage( DALI_IMAGE_DIR "image_cover.9.png" );
      //string maskImage( DALI_IMAGE_DIR "button-disabled.9.png" );
      //string maskImage( DALI_IMAGE_DIR "gallery-large-1.jpg" );

      ImageDimensions size = ResourceImage::GetImageSize( maskImage );
      Vector2 maskSize( size.GetWidth(), size.GetHeight() );

      const float leftRight = (maskSize.width  - 1.0f) * 0.5f;
      const float topBottom = (maskSize.height - 1.0f) * 0.5f;
      Vector4 maskBorder( leftRight, topBottom, leftRight, topBottom );
      //Vector4 maskBorder( 50., 50., 0., 0. );

      {

      ShaderEffect maskEffect = ShaderEffect::New( ALPHA_MASK_VERTEX_SHADER_SOURCE,
          ALPHA_MASK_FRAGMENT_SHADER_SOURCE,
          GeometryType( GEOMETRY_TYPE_IMAGE ),
          ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );

      //imgAct.SetBlendMode(BlendingMode::ON);
      //ShaderEffect maskEffect = ShaderEffect::New( ALPHA_MASK_VERTEX_SHADER_SOURCE,
          //ALPHA_MASK_FRAGMENT_SHADER_SOURCE,
          //GeometryType( GEOMETRY_TYPE_IMAGE ),
          //ShaderEffect::GeometryHints( ShaderEffect::HINT_NONE ) );

      maskEffect.SetEffectImage( ResourceImage::New( maskImage ) );

      maskEffect.SetUniform( "uImageSize", Vector2(0,0) /*Constrained to actor size*/ );

      Constraint constraint = Constraint::New<Vector2>( maskEffect, maskEffect.GetPropertyIndex("uImageSize"),
          UnpremultiplyNinePatchMaskEffectSizeConstraint() );
      constraint.AddSource( Source(imgAct, Actor::Property::SIZE) );
      constraint.Apply();

      maskEffect.SetUniform( "uMaskSize", maskSize );

      //// Actor must provide nine-patch style geometry for this effect to work
      imgAct.SetStyle( ImageActor::STYLE_NINE_PATCH );
      imgAct.SetNinePatchBorder( maskBorder );
      imgAct.SetShaderEffect( maskEffect );
      }
#endif


#ifdef IMAGE_VIEW
      //////////////////////////////
      // ImageView
      imgView.SetShader(ALPHA_MASK_VERTEX_SHADER_SOURCE, ALPHA_MASK_FRAGMENT_SHADER_SOURCE);
      Property::Index uImageSizeIndex = imgView.RegisterProperty( "uImageSize", Vector2(0,0) /*Constrained to actor size*/ );
      Property::Index uMaskSizeIndex = imgView.RegisterProperty( "uMaskSize", maskSize );
      imgView.MapUniform(uImageSizeIndex, "uImageSize");
      imgView.MapUniform(uMaskSizeIndex, "uMaskSize");

      Constraint constraint = Constraint::New<Vector2>( imgView, imgView.GetPropertyIndex("uImageSize"),
          UnpremultiplyNinePatchMaskEffectSizeConstraint() );
      constraint.AddSource( LocalSource(Actor::Property::SIZE) );
      constraint.Apply();
#endif
    }

    // 4.unpremultiply effect
    if(0)
    {
#ifdef IMAGE_ACTOR
      // ImageActor
      ShaderEffect maskEffect = ShaderEffect::New( "",
          UNPREMULTIPLY_FRAGMENT_SHADER_SOURCE,
          GeometryType( GEOMETRY_TYPE_IMAGE ),
          ShaderEffect::GeometryHints( ShaderEffect::HINT_BLENDING ) );
      imgAct.SetShaderEffect( maskEffect );
#endif

#ifdef IMAGE_VIEW
      //// ImageView
      //imgView.SetShader("", UNPREMULTIPLY_FRAGMENT_SHADER_SOURCE);
#endif
    }

    // 5.ImageRegionEffect
    if(1)
    {
#ifdef IMAGE_ACTOR

      //{
      //// ImageActor with ImageRegionEffect
      //ImageRegionEffect effect = ImageRegionEffect::New();
      //effect.SetTopLeft(Vector2(0.0f, 0.0f));
      //effect.SetBottomRight(Vector2(1.0f, 1.0f));
      //imgAct.SetShaderEffect( effect );
      //}

      {
      // ImageActor with ShaderEffect
      ShaderEffect effect = ShaderEffect::New(IMAGE_REGION_VERTEX_SHADER, "");
      effect.SetUniform("uTopLeft", Vector2(0.0f, 0.0f));
      effect.SetUniform("uBottomRight", Vector2(1.0f, 1.0f));
      imgAct.SetShaderEffect( effect );
      }

#endif

#ifdef IMAGE_VIEW
      //// ImageView
      //imgView.SetShader("", UNPREMULTIPLY_FRAGMENT_SHADER_SOURCE);
#endif
    }
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    //mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  HelloWorldController test( application );

  application.MainLoop();
}

// Entry point for Linux & SLP applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
