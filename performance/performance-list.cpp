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
const Vector4 BACKGROUND_COLOR( 1.0f, 1.0f, 1.0f, 1.f );
const char* IMAGE_FOLDER( PERFORMANCE_LIST_IMAGE_DIR );
const unsigned int NUMBER_OF_COLUMNS = 1;
const unsigned int MAX_CHARACTERS_PER_LINE = 40;
const unsigned int MAX_ROWS_OF_TEXT_PER_ENTRY = 5;
unsigned int NUMBER_OF_ITEMS = 100;
unsigned int NUMBER_OF_IMAGES = 100;
unsigned int NUMBER_OF_TEXT_VIEWS = 100;
unsigned int NUMBER_OF_UNIQUE_TEXTS = 4;
const Vector3 DEFAULT_ICON_SIZE(180.0f,120.0f, 1.0);
const Vector3 TEXT_SIZE_RELATIVE_TO_COLUMN( 0.85f, 1.0f, 1.0f );
const float DEFAULT_TEXT_HEIGHT = 180.0f;
const float LIST_ROW_SPACING = 10.0f;
const float TEXT_ITEM_HEIGHT = 180.0f;
const float DEFAULT_SCROLL_SPEED_FACTOR = 0.01f;

const float DEFAULT_TOP_MARGIN     =  95.0f;
const float DEFAULT_BOTTOM_MARGIN  =  20.0f;
const float DEFAULT_SIDE_MARGIN    =  20.0f;
const float DEFAULT_COLUMN_SPACING =  20.0f;
const float DEFAULT_ROW_SPACING    =  20.0f;
const float DEFAULT_MAXIMUM_SWIPE_SPEED = 100.0f;
const float DEFAULT_ITEM_FLICK_ANIMATION_DURATION = 0.05f;


// 4 orientations are supported
static const unsigned int ORIENTATION_COUNT = 4;


enum AllImagesLayouts
{
  GRID_LAYOUT,

  NUMBER_OF_LAYOUTS
};

const char* TEXT_TEXT[] = {
      "Tuesday it is in the final stage of negotiations with Google Inc. to manufacture Internet-connected TVs powered by the U.S. company's software.",
      "Yoon Book-keun, president of Samsung's digital media business, said talks between the two companies on Google TV are at the last stage, confirming an earlier report by Reuters.",
      "Samsung has not yet decided if it will unveil the Samsung-backed Google TV during a January consumer electronics show in Las Vegas, Yoon told reporters on the sidelines of a conference.",
      "The adoption of Google's TV operating system by Samsung, the world's largest TV supplier, will likely boost the U.S. search engine operator's TV project, which has not made a big mark in the market.",
      "Market watchers expect TVs to be the next battle stage for global mobile software companies, such as Google, Apple Inc. and Microsoft Corp., as consumers seek to watch, share and access content from multiple screens.",
      "The Google TV project to seamlessly mesh the Internet with the living room staple faced a backlash when one of its partners announced departure earlier this month.",
      "Logitech International SA said it will stop producing the set-top boxes for Google TVs after disappointing sales of early models, leaving Sony Corp. as the only Google TV partner.",
      "Google, which introduced an update to its TV software last month, has been seeking to woo Korean TV vendors, the world's two-biggest players in the market, to expand sales.",
      "LG Electronics Inc., the world's second-largest maker of flat-screen TVs, plans to unveil its Google TV at the January trade show in Las Vegas, Bloomberg reported earlier citing two unnamed sources.",
      "Samsung and LG manufacture smart TVs running on their own in-house operating system, which offer video-on-demand, Internet surfing and search services and other functions through applications",
      "A significant expansion in access to treatment helped slash the number of AIDS-related deaths in 2010, bringing the number of people living with HIV to a record 34 million, the United Nations said Monday.",
      "About half of those eligible for treatment are now receiving it, with the most dramatic improvement in access seen in sub-Saharan Africa, which recorded a 20-percent jump in people undergoing treatment between 2009 and 2010.",
      "As a result of better access to healthcare, the number of AIDS-related deaths was also falling, said UNAIDS, the UN agency spearheading the international campaign against the disease.",
      "In 2010, 1.8 million deaths were linked to AIDS, down from a peak of 2.2 million last seen in 2006.",
      "ot only is treatment helping to prevent new AIDS-related deaths but it is also contributing to a drop in new HIV infections.",
      "Patients undergoing care were less likely to infect others, as prevention programmes coupled with treatments were proving effective.",
      "In Namibia for instance, where treatment access reached an all-time high of 90 percent and condom use rose to 75 percent among men, the combined impact contributed to a 60 percent drop in new infections by 2010, noted UNAIDS.",
      "The UN agency added that the full preventive impact of treatment was likely to be seen in the next five years, as more countries reach high levels of treatment coverage.",
      "Medical charity Medecins Sans Frontieres urged governments to keep up their funding.",
      "Swedish scientists have succeeded in creating light from vacuum ??observing an effect first predicted over 40 years ago.",
      "In an experiment, they have managed to capture some of the photons that are constantly appearing and disappearing in the vacuum.",
      "The results have been published in the journal Nature.",
      "Many lawmakers, professors and lawyers had demanded that the regulator order Lone Star to sell KEB shares to anonymous investors on the stock market and ban the fund from selecting a particular acquirer.",
      "Further, if the fund is found to have been a non-financial investor, Lone Star? Is acquisition of KEB in 2003 could be invalidated under the nation? Is banking laws.",
      "The FSC was ordered by courts in January and September 2009 to publicize the documents which it has been examining to determine Lone Star? Is eligibility for KEB control since 2007.",
      "But it rejected the order and appealed with the Supreme Court, which is reportedly scheduled to deliver a verdict on Nov. 24",
      "They also said the FSC neglected growing calls from the ruling and opposition lawmakers and civic groups for stern action on Lone Star.",
      "Lebanon has improved since its loss to Korea, notching seven points against the UAE and Kuwait, while Cho? Is side has been sluggish recently.",
      "Korea has long been a powerhouse in Asian football, and this year the Taegeuk Warriors are on the path to qualify for their eighth consecutive appearance in the World Cup.",
      "Just one more victory will seal a berth in Asia? Is final qualifying round for the 2014 World Cup. It can be clinched on Tuesday when Korea, which sits on top in Group B, faces runner-up Lebanon in Beirut.",
      "In its previous encounter in September, manager Cho Kwang-rae? Is side crushed the Middle Eastern team 6-0 at home. Cho admitted, however, that Tuesday? Is away game would be different.",
      "Although it managed to beat the UAE last week thanks to goals from Park Chu-young and Lee Keun-ho, the performance was rather disappointing.",
      "Korea had no shot on target during the first 45 minutes and could not break the UAE? Is defensive wall until Lee found the back of the net in the 87th minute and Park added one in extra time.",
      "Since his appointment in 2010, Cho has been relying on a new tactic ??playing a short-passing game with fast-paced running.",
      "This did not work against the UAE, largely due to the absence of key midfielders Lee Chung-young, who broke his leg in July and Ki Sung-yueng, who was sidelined with fatigue issues.",
      "A squad change is inevitable as the two players will not be available again as well as striker Park Chu-young, who has scored six goals in the past four games, will miss Tuesday? Is game because of suspension.",
      "Moreover, young striker Ji Dong-won is struggling to find his rhythm, leaving only a few options for Cho.",
      "Lee Keun-ho is expected to fill the void left by Park. Cho already noted that he would employ Lee as the lone striker with Son Heung-min supporting Lee instead of Ji.",
      "The 20-year-old Son is not yet ripe for the national team, but the manager said he is now in great form and ready for a big role.",
      "Playing an away game is always tricky, but playing in the Middle East is even more difficult as Korea will be forced to battle the blistering-hot weather and a large home crowd.",
      "On top of that, Cho says, Korea has to overcome the poor pitch condition at Sports City Stadium in Beirut.",
      "TASHKENT, Uzbekistan (AP) _ Timur Kapadze scored in the second half to secure Uzbekistan's 1-0 victory over North Korea in a World Cup qualifier.",
      "The result ended North Korea's hopes of qualifying for a second straight World Cup, making it the first team to have played in South Africa in 2010 to be eliminated from the 2014 tournament in Brazil.",
      "Kapadze headed in Alexander Geynrikh's corner in the 49th minute as the home side dominated the game.",
      "The victory put Uzbekistan on par with Japan on top of Group C with 10 points from four games. North Korea has three points, followed by Tajikistan with no points.",
      "Despite its domination, Uzbekistan failed to find a breakthrough in the first half.",
      "Geynrikh, who scored the winner when the two teams played in Pyongyang last month, hit the crossbar in the 18th minute. A minute later, Maksim Shatskikh's low shot from just inside the box hit the left post.",
      "Geynrikh again came close with a minute remaining in the first half.",
      "After the restart, North Korea goalkeeper Ri Myong Guk tipped the ball over the crossbar to keep out an own goal before Kapadze found the net.",
      "The visitors pressed for an equalizer, forcing Ignatiy Nesterov to make several saves to preserve the victory.",
      "Scientists create light from vacuum",
      "Swedish scientists have succeeded in creating light from vacuum",
      "observing an effect first predicted over 40 years ago.",
      "In an experiment, they have managed to capture some of the photons that are constantly appearing and disappearing in the vacuum.",
      "The results have been published in the journal Nature.",
      "Virtual photons bounce off a mirror that vibrates at a speed that is almost as high as the speed of light.",
      "The round mirror in the picture is a symbol, and under that is the quantum electronic component (referred to as a SQUID),",
      "which acts as a mirror. This makes real photons appear (in pairs) in vacuum.",
      "The experiment is based on one of the most counterintuitive, yet, one of the most important principles in quantum mechanics",
      "that vacuum is by no means empty nothingness.",
      "In fact, the vacuum is full of various particles that are continuously fluctuating in and out of existence.",
      "They appear, exist for a brief moment and then disappear again.",
      "Since their existence is so fleeting, they are usually referred to as virtual particles.",
      "Christopher Wilson and his co-workers at Chalmers University of Technology have succeeded in getting photons to leave their virtual state and become real photons, i.e. measurable light.",
      "The physicist Moore predicted way back in 1970 that this should happen if the virtual photons are allowed to bounce off a mirror that is moving at a speed that is almost as high as the speed of light.",
      "The phenomenon, known as the dynamical Casimir effect, has now been observed for the first time in a brilliant experiment conducted by the Chalmers scientists.",
      "Since its not possible to get a mirror to move fast enough",
      "we have developed another method for achieving the same effect",
      "explains Per Delsing, Professor of Experimental Physics at Chalmers.",
      "Instead of varying the physical distance to a mirror",
      "we've varied the electrical distance to an electrical short circuit that acts as a mirror for microwaves.",
      "The mirror consists of a quantum electronic component referred to as a SQUID",
      "Superconducting quantum interference device",
      "which is extremely sensitive to magnetic fields.",
      "By changing the direction of the magnetic field several billions of times a second the scientists were able to make the mirror vibrate at a speed of up to 25 percent of the speed of light.",
      "The result was that photons appeared in pairs from the vacuum",
      "which we were able to measure in the form of microwave radiation",
      "says Per Delsing.",
      "We were also able to establish that the radiation had precisely the same properties that quantum theory says",
      "it should have when photons appear in pairs in this way.",
      "What happens during the experiment is that the mirror transfers some of its kinetic energy to virtual photons",
      "which helps them to materialise.",
      "According to quantum mechanics,",
      "there are many different types of virtual particles in vacuum",
      "as mentioned earlier. Goran Johansson, Associate Professor of Theoretical Physics",
      "explains that the reason why photons appear in the experiment is that they lack mass.",
      "Relatively little energy is therefore required in order to excite them out of their virtual state.",
      "In principle, one could also create other particles from vacuum",
      "such as electrons or protons",
      "but that would require a lot more energy.",
      "The scientists find the photons that appear in pairs",
      "in the experiment interesting to study in closer detail.",
      "They can perhaps be of use in the research field of quantum information",
      "which includes the development of quantum computers.",
      "However, the main value of the experiment is that",
      "it increases our understanding of basic physical concepts",
      "such as vacuum fluctuations",
      "the constant appearance and disappearance of virtual particles in vacuum.",
      "It is believed that vacuum fluctuations may have a connection with dark energy which drives the accelerated expansion of the universe.",
      "The discovery of this acceleration was recognized this year with the awarding of the Nobel Prize in Physics."
};

  static Vector3 GetItemSizeDefaultFunction(unsigned int numberOfColumns, float layoutWidth, float sideMargin, float columnSpacing)  /// ItemSizeFunction
  {
    float width = (layoutWidth - sideMargin * 2.0f );
    float height = TEXT_ITEM_HEIGHT;
    float depth = 1.0;
    return Vector3(width, height , depth);
  }

} // end namespace

// MyListLayout code

class MyListLayout;

typedef IntrusivePtr<MyListLayout> MyListLayoutPtr;

/**
 * An ItemView layout which arranges items in a list.
 */
class MyListLayout : public ItemLayout
{
public:

  typedef boost::function<Vector3 (unsigned int numberOfColumns, float layoutWidth, float sideMargin, float columnSpacing)> ItemSizeFunction;

  static MyListLayoutPtr New();

  virtual ~MyListLayout();

  void SetNumberOfColumns(unsigned int columns);

  unsigned int GetNumberOfColumns() const;

  void SetRowSpacing(float spacing);

  float GetRowSpacing() const;

  void SetColumnSpacing(float spacing);

  float GetColumnSpacing() const;

  void SetTopMargin(float margin);

  float GetTopMargin() const;

  void SetBottomMargin(float margin);

  float GetBottomMargin() const;

  void SetSideMargin(float margin);

  float GetSideMargin() const;

  void SetItemSizeFunction(ItemSizeFunction function);

  ItemSizeFunction GetItemSizeFunction() const;

  void SetScrollSpeedFactor(float scrollSpeed);

  void SetMaximumSwipeSpeed(float speed);

  void SetItemFlickAnimationDuration(float durationSeconds);

  virtual float GetScrollSpeedFactor() const;

  virtual float GetMaximumSwipeSpeed() const;

  virtual float GetItemFlickAnimationDuration() const;

private:

  // Pure virtual functions from ItemLayout

  virtual float GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const;

  virtual float GetClosestAnchorPosition(float layoutPosition) const;

  virtual float GetItemScrollToPosition(unsigned int itemId) const;

  virtual ItemRange GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const;

  virtual unsigned int GetReserveItemCount(Vector3 layoutSize) const;

  virtual bool GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const;

  virtual void GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const;

  virtual bool GetPositionConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const;

  virtual bool GetRotationConstraint(unsigned int itemId, ItemLayout::QuaternionFunction& constraint) const;

  virtual bool GetScaleConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const;

  virtual bool GetColorConstraint(unsigned int itemId, ItemLayout::Vector4Function& constraint) const;

  virtual bool GetVisibilityConstraint(unsigned int itemId, ItemLayout::BoolFunction& constraint) const;

  virtual Degree GetScrollDirection() const;

protected:

  /**
   * Protected constructor; see also GridLayout::New()
   */
  MyListLayout();

private:

  struct Impl;
  Impl* mImpl;
};

struct GridPositionConstraint0
{
  GridPositionConstraint0(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float topMargin, const float sideMargin, MyListLayout::ItemSizeFunction itemSizeFunction)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mTopMargin(topMargin),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.width, mSideMargin, mColumnSpacing);

    return Vector3(mSideMargin + (mColumnIndex * (itemSize.x + mColumnSpacing)) + itemSize.x * 0.5f - layoutSize.x * 0.5f,
                   ((itemSize.y + mRowSpacing) * (layoutPosition - mColumnIndex)) / mNumberOfColumns - layoutSize.height * 0.5f + itemSize.y * 0.5f + mTopMargin,
                   0.0f);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mSideMargin;
  MyListLayout::ItemSizeFunction mItemSizeFunction;
};

struct GridPositionConstraint90
{
  GridPositionConstraint90(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float topMargin, const float sideMargin, MyListLayout::ItemSizeFunction itemSizeFunction)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mTopMargin(topMargin),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.height, mSideMargin, mColumnSpacing);

    return Vector3(((itemSize.y + mRowSpacing) * (layoutPosition - mColumnIndex)) / mNumberOfColumns - layoutSize.width * 0.5f + itemSize.y * 0.5f + mTopMargin,
                   -(mSideMargin + (mColumnIndex * (itemSize.x + mColumnSpacing)) + itemSize.x * 0.5f - layoutSize.y*0.5f),
                   0.0f);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mSideMargin;
  MyListLayout::ItemSizeFunction mItemSizeFunction;
};

struct GridPositionConstraint180
{
  GridPositionConstraint180(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float topMargin, const float sideMargin, MyListLayout::ItemSizeFunction itemSizeFunction)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mTopMargin(topMargin),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.width, mSideMargin, mColumnSpacing);

    return Vector3(-(mSideMargin + (mColumnIndex * (itemSize.x + mColumnSpacing)) + itemSize.y * 0.5f - layoutSize.x * 0.5f),
                   -(((itemSize.y + mRowSpacing) * (layoutPosition - mColumnIndex)) / mNumberOfColumns - layoutSize.height * 0.5f + itemSize.y * 0.5f + mTopMargin),
                   0.0f);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mSideMargin;
  MyListLayout::ItemSizeFunction mItemSizeFunction;
};

struct GridPositionConstraint270
{
  GridPositionConstraint270(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float topMargin, const float sideMargin, MyListLayout::ItemSizeFunction itemSizeFunction)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mTopMargin(topMargin),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  Vector3 operator()(const Vector3& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.height, mSideMargin, mColumnSpacing);

    return Vector3(-(((itemSize.y + mRowSpacing) * (layoutPosition - mColumnIndex)) / mNumberOfColumns - layoutSize.width * 0.5f + itemSize.y * 0.5f + mTopMargin),
                   mSideMargin + (mColumnIndex * (itemSize.x + mColumnSpacing)) + itemSize.x * 0.5f - layoutSize.y * 0.5f,
                   0.0f);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mSideMargin;
  MyListLayout::ItemSizeFunction mItemSizeFunction;
};

struct GridVisibilityConstraintPortrait
{
  GridVisibilityConstraintPortrait(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float sideMargin, MyListLayout::ItemSizeFunction itemSizeFunction)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.width, mSideMargin, mColumnSpacing);

    float row = (layoutPosition - static_cast<float>(mColumnIndex)) / mNumberOfColumns;
    int rowsPerPage = ceil(layoutSize.height / (itemSize.y + mRowSpacing));

    return (row > -2.0f) && (row < rowsPerPage);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mSideMargin;
  MyListLayout::ItemSizeFunction mItemSizeFunction;
};

struct GridVisibilityConstraintLandscape
{
  GridVisibilityConstraintLandscape(const unsigned int columnIndex, const unsigned int numberOfColumns, const float rowSpacing, const float columnSpacing, const float sideMargin, MyListLayout::ItemSizeFunction itemSizeFunction)
  : mColumnIndex(columnIndex),
    mNumberOfColumns(numberOfColumns),
    mRowSpacing(rowSpacing),
    mColumnSpacing(columnSpacing),
    mSideMargin(sideMargin),
    mItemSizeFunction(itemSizeFunction)
  {
  }

  bool operator()(const bool& current, const float& layoutPosition, const float& scrollSpeed, const Vector3& layoutSize)
  {
    Vector3 itemSize = mItemSizeFunction(mNumberOfColumns, layoutSize.height, mSideMargin, mColumnSpacing);

    float row = (layoutPosition - static_cast<float>(mColumnIndex)) / mNumberOfColumns;
    int rowsPerPage = ceil(layoutSize.width / (itemSize.y + mRowSpacing));

    return (row > -2.0f) && (row < rowsPerPage);
  }

public:

  unsigned int mColumnIndex;
  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mSideMargin;
  MyListLayout::ItemSizeFunction mItemSizeFunction;
};

struct MyListLayout::Impl
{
  Impl()
  : mNumberOfColumns(NUMBER_OF_COLUMNS),
    mRowSpacing(DEFAULT_ROW_SPACING),
    mColumnSpacing(DEFAULT_COLUMN_SPACING),
    mTopMargin(DEFAULT_TOP_MARGIN),
    mBottomMargin(DEFAULT_BOTTOM_MARGIN),
    mSideMargin(DEFAULT_SIDE_MARGIN),
    mScrollSpeedFactor(DEFAULT_SCROLL_SPEED_FACTOR),
    mMaximumSwipeSpeed(DEFAULT_MAXIMUM_SWIPE_SPEED),
    mItemFlickAnimationDuration(DEFAULT_ITEM_FLICK_ANIMATION_DURATION),
    mItemSizeFunction(GetItemSizeDefaultFunction)
  {
  }

  unsigned int mNumberOfColumns;
  float mRowSpacing;
  float mColumnSpacing;
  float mTopMargin;
  float mBottomMargin;
  float mSideMargin;

  float mScrollSpeedFactor;
  float mMaximumSwipeSpeed;
  float mItemFlickAnimationDuration;

  ItemSizeFunction mItemSizeFunction;
};

MyListLayoutPtr MyListLayout::New()
{
  return MyListLayoutPtr(new MyListLayout());
}

MyListLayout::~MyListLayout()
{
  delete mImpl;
}

void MyListLayout::SetNumberOfColumns(unsigned int columns)
{
  mImpl->mNumberOfColumns = columns;
}

unsigned int MyListLayout::GetNumberOfColumns() const
{
  return mImpl->mNumberOfColumns;
}

void MyListLayout::SetRowSpacing(float spacing)
{
  mImpl->mRowSpacing = spacing;
}

float MyListLayout::GetRowSpacing() const
{
  return mImpl->mRowSpacing;
}

void MyListLayout::SetColumnSpacing(float spacing)
{
  mImpl->mColumnSpacing = spacing;
}

float MyListLayout::GetColumnSpacing() const
{
  return mImpl->mColumnSpacing;
}

void MyListLayout::SetTopMargin(float margin)
{
  mImpl->mTopMargin = margin;
}

float MyListLayout::GetTopMargin() const
{
  return mImpl->mTopMargin;
}

void MyListLayout::SetBottomMargin(float margin)
{
  mImpl->mBottomMargin = margin;
}

float MyListLayout::GetBottomMargin() const
{
  return mImpl->mBottomMargin;
}

void MyListLayout::SetSideMargin(float margin)
{
  mImpl->mSideMargin = margin;
}

float MyListLayout::GetSideMargin() const
{
  return mImpl->mSideMargin;
}

void MyListLayout::SetItemSizeFunction(ItemSizeFunction function)
{
  mImpl->mItemSizeFunction = function;
}

MyListLayout::ItemSizeFunction MyListLayout::GetItemSizeFunction() const
{
  return mImpl->mItemSizeFunction;
}

void MyListLayout::SetScrollSpeedFactor(float scrollSpeed)
{
  mImpl->mScrollSpeedFactor = scrollSpeed;
}

void MyListLayout::SetMaximumSwipeSpeed(float speed)
{
  mImpl->mMaximumSwipeSpeed = speed;
}

void MyListLayout::SetItemFlickAnimationDuration(float durationSeconds)
{
  mImpl->mItemFlickAnimationDuration = durationSeconds;
}

float MyListLayout::GetScrollSpeedFactor() const
{
  return mImpl->mScrollSpeedFactor;
}

float MyListLayout::GetMaximumSwipeSpeed() const
{
  return mImpl->mMaximumSwipeSpeed;
}

float MyListLayout::GetItemFlickAnimationDuration() const
{
  return mImpl->mItemFlickAnimationDuration;
}

float MyListLayout::GetMinimumLayoutPosition(unsigned int numberOfItems, Vector3 layoutSize) const
{
  float layoutWidth = IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width;
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize = mImpl->mItemSizeFunction( mImpl->mNumberOfColumns, layoutWidth, mImpl->mSideMargin, mImpl->mColumnSpacing);

  unsigned int itemsLastRow = numberOfItems % mImpl->mNumberOfColumns;
  if (itemsLastRow == 0)
  {
    itemsLastRow = mImpl->mNumberOfColumns;
  }

  float rowsLastPage = (layoutHeight - mImpl->mBottomMargin - mImpl->mTopMargin) / (itemSize.y + mImpl->mRowSpacing);
  float itemsLastPage = (rowsLastPage - 1.0f) * static_cast<float>(mImpl->mNumberOfColumns) + static_cast<float>(itemsLastRow);

  return itemsLastPage - static_cast<float>(numberOfItems);
}

float MyListLayout::GetClosestAnchorPosition(float layoutPosition) const
{
  float rowIndex = static_cast<float>(round(layoutPosition / mImpl->mNumberOfColumns));
  return rowIndex * static_cast<float>(mImpl->mNumberOfColumns);
}

float MyListLayout::GetItemScrollToPosition(unsigned int itemId) const
{
  float rowIndex = static_cast<float>(itemId / mImpl->mNumberOfColumns);
  return -rowIndex * static_cast<float>(mImpl->mNumberOfColumns);
}

ItemRange MyListLayout::GetItemsWithinArea(float firstItemPosition, Vector3 layoutSize) const
{
  float layoutWidth = IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width;
  float layoutHeight = IsHorizontal(mOrientation) ? layoutSize.width : layoutSize.height;

  Vector3 itemSize = mImpl->mItemSizeFunction( mImpl->mNumberOfColumns, layoutWidth, mImpl->mSideMargin, mImpl->mColumnSpacing);

  int itemsPerPage = mImpl->mNumberOfColumns * ceil(layoutHeight / (itemSize.y + mImpl->mRowSpacing));
  int firstVisibleItem = -(static_cast<int>(firstItemPosition / mImpl->mNumberOfColumns)) * mImpl->mNumberOfColumns;

  int firstItemIndex = std::max(0, firstVisibleItem - static_cast<int>(mImpl->mNumberOfColumns));
  int lastItemIndex  = std::max(0, firstVisibleItem + itemsPerPage);

  return ItemRange(firstItemIndex, lastItemIndex);
}

unsigned int MyListLayout::GetReserveItemCount(Vector3 layoutSize) const
{
  int itemsPerPage = NUMBER_OF_TEXT_VIEWS; // ensure all the text views are reserved.

  return itemsPerPage;
}

bool MyListLayout::GetItemSize(unsigned int itemId, Vector3 layoutSize, Vector3& itemSize) const
{
  // Note: itemId is not checked, since every item has the same size

  itemSize = mImpl->mItemSizeFunction( mImpl->mNumberOfColumns, (IsHorizontal(mOrientation) ? layoutSize.height : layoutSize.width), mImpl->mSideMargin, mImpl->mColumnSpacing);
  return true;
}

void MyListLayout::GetResizeAnimation(Animation& animation, Actor actor, Vector3 size, float durationSeconds) const
{
  if(animation)
  {
    Vector3 currentSize( actor.GetCurrentSize() );
    Vector3 shrink( currentSize );

    shrink.width = std::min(size.width, currentSize.width);
    shrink.height = std::min(size.height, currentSize.height);

    // Do a nonlinear size animation to shrink the actor first when the actor size changes,
    // so that we can avoid the actors overlapping during orientation change.
    animation.Resize( actor, shrink, AlphaFunctions::EaseOut, 0.0f, durationSeconds * 0.5f );
    animation.Resize( actor, size, AlphaFunctions::EaseIn, 0.0f, durationSeconds );
    animation.SetDestroyAction( Animation::Bake );
  }
}

bool MyListLayout::GetPositionConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  unsigned int columnIndex = itemId % mImpl->mNumberOfColumns;

  if (mOrientation == ControlOrientation::Up)
  {
    constraint = GridPositionConstraint0(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mTopMargin, mImpl->mSideMargin, mImpl->mItemSizeFunction);
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    constraint = GridPositionConstraint90(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mTopMargin, mImpl->mSideMargin, mImpl->mItemSizeFunction);
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    constraint = GridPositionConstraint180(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mTopMargin, mImpl->mSideMargin, mImpl->mItemSizeFunction);
  }
  else // mOrientation == ControlOrientation::Right
  {
    constraint = GridPositionConstraint270(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mTopMargin, mImpl->mSideMargin, mImpl->mItemSizeFunction);
  }

  return true;
}

bool MyListLayout::GetRotationConstraint(unsigned int itemId, ItemLayout::QuaternionFunction& constraint) const
{
  return false; // No rotation
}

bool MyListLayout::GetScaleConstraint(unsigned int itemId, ItemLayout::Vector3Function& constraint) const
{
  return false; // No scaling
}

bool MyListLayout::GetColorConstraint(unsigned int itemId, ItemLayout::Vector4Function& constraint) const
{
  return false; // No color
}

bool MyListLayout::GetVisibilityConstraint(unsigned int itemId, ItemLayout::BoolFunction& constraint) const
{
  unsigned int columnIndex = itemId % mImpl->mNumberOfColumns;

  if (IsVertical(mOrientation))
  {
    constraint = GridVisibilityConstraintPortrait(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mSideMargin, mImpl->mItemSizeFunction);
  }
  else // horizontal
  {
    constraint = GridVisibilityConstraintLandscape(columnIndex, mImpl->mNumberOfColumns, mImpl->mRowSpacing, mImpl->mColumnSpacing, mImpl->mSideMargin, mImpl->mItemSizeFunction);
  }

  return true;
}

Degree MyListLayout::GetScrollDirection() const
{
  Degree scrollDirection(0.0f);

  if (mOrientation == ControlOrientation::Up)
  {
    scrollDirection = 0.0f;
  }
  else if (mOrientation == ControlOrientation::Left)
  {
    scrollDirection = 90.0f;
  }
  else if (mOrientation == ControlOrientation::Down)
  {
    scrollDirection = 180.0f;
  }
  else // mOrientation == ControlOrientation::Right
  {
    scrollDirection = 270.0f;
  }

  return scrollDirection;
}

MyListLayout::MyListLayout()
: mImpl(NULL)
{
  mImpl = new Impl();
}

//// Performance test code

class ListViewPerformanceTest : public ConnectionTracker, public ItemFactory
{
public:

  ListViewPerformanceTest( Application &application )
  : mApplication( application ),
    mView(),
    mToolBar(),
    mActorRoot(),
    mStageSize(0.0f, 0.0f, 0.0f)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &ListViewPerformanceTest::Create );
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
    Dali::ImageActor backgroundImageActor = Toolkit::CreateSolidColorActor( BACKGROUND_COLOR, true, Color::WHITE );
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

    CreateTextObjects( NUMBER_OF_TEXT_VIEWS );

    CreateView();
  }

  virtual unsigned int GetNumberOfItems()
  {
    return NUMBER_OF_ITEMS;
  }

  virtual Actor NewItem(unsigned int itemId)
  {
    Actor item = Actor::New();

    item.Add( mTextList[ itemId ]);

    ImageActor imageActor = ImageActor::New(  mImageList[itemId % mNumberOfIcons] );
    imageActor.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    imageActor.SetAnchorPoint( AnchorPoint::CENTER_LEFT );
    imageActor.SetSize( DEFAULT_ICON_SIZE );
    item.Add(imageActor);

    return item;
  }


private:

  void LoadImages();

  void CreateView();

  void CreateTextObjects( unsigned int requireNumberOfObjects );

private:
  Application&  mApplication;
  Toolkit::View mView;
  Toolkit::ToolBar mToolBar;
  Actor mActorRoot;
  Vector3 mStageSize;
  unsigned int mNumberOfIcons;
  std::vector<Image> mImageList;
  ItemView mItemView;
  MyListLayoutPtr mGridLayout;
  std::vector<TextView> mTextList;
};

void ListViewPerformanceTest::LoadImages()
{
  // Get all the file paths and file names in the image resource folder
  DIR *dir;
  std::stringstream fileName;
  std::vector<std::string> imageFileList;

  if ((dir = opendir (IMAGE_FOLDER)) != NULL)
  {
    for (unsigned int index = 0; index < NUMBER_OF_IMAGES; index++ )
    {
      fileName << IMAGE_FOLDER <<"/image"<< index <<".png";
      imageFileList.push_back(fileName.str());
      fileName.str( "" );
    }
    closedir (dir);
  }
  else
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

void ListViewPerformanceTest::CreateView()
{
  // Create the item view actor
  mItemView = ItemView::New( *this );
  mItemView.SetParentOrigin(ParentOrigin::CENTER);
  mItemView.SetAnchorPoint(AnchorPoint::CENTER);
  mActorRoot.Add(mItemView);

  // Create the layout
  mGridLayout = MyListLayout::New();
  mGridLayout->SetNumberOfColumns(NUMBER_OF_COLUMNS);

  // Add the layout to item view
  mItemView.AddLayout(*mGridLayout);
  mGridLayout->SetRowSpacing( LIST_ROW_SPACING );
  mGridLayout->SetScrollSpeedFactor( DEFAULT_SCROLL_SPEED_FACTOR );
  mGridLayout->SetItemSizeFunction( GetItemSizeDefaultFunction ); // Set size of each item in the grid using the provide function.

  Stage stage = Dali::Stage::GetCurrent();
  Vector3 size(stage.GetSize());

  mItemView.ActivateLayout( GRID_LAYOUT, size, 0.0f);
}

void ListViewPerformanceTest::CreateTextObjects( unsigned int requireNumberOfObjects)
{
  for (unsigned int index = 0; index < requireNumberOfObjects; index++ )
  {
    TextView text = TextView::New( TEXT_TEXT[index%NUMBER_OF_UNIQUE_TEXTS] );
    text.SetSize( Vector2(mStageSize.width - DEFAULT_ICON_SIZE.width, DEFAULT_TEXT_HEIGHT ) );
    text.SetPosition(DEFAULT_ICON_SIZE.x, 0.0f, 0.0f);
    text.SetMultilinePolicy(TextView::SplitByWord);
    text.SetWidthExceedPolicy(TextView::ShrinkToFit);
    text.SetParentOrigin( ParentOrigin::CENTER_LEFT );
    text.SetAnchorPoint( AnchorPoint::CENTER_LEFT );

    TextStyle style;
    style.SetTextColor( Vector4(0.f,0.f,0.f,1.f) );
    style.SetFontPointSize(PointSize(6.0));
    text.SetStyleToCurrentText( style, static_cast<TextStyle::Mask>(TextStyle::COLOR | TextStyle::SIZE) );

    mTextList.push_back(text);
  }
}

void RunTest( Application& application )
{
  ListViewPerformanceTest test( application );

  application.MainLoop();
}

int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
