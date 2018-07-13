﻿#include "WinRewardLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"

WinRewardLayer::WinRewardLayer()
{

}


WinRewardLayer::~WinRewardLayer()
{

}

WinRewardLayer* WinRewardLayer::create(std::vector<FOURProperty> reward_res)
{
	WinRewardLayer *pRet = new(std::nothrow)WinRewardLayer();
	if (pRet && pRet->init(reward_res))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool WinRewardLayer::init(std::vector<FOURProperty> reward_res)
{

	m_rewards = reward_res;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("winRewardLayer.csb"));
	this->addChild(csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("winrewardtitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1002);
	closebtn->addTouchEventListener(CC_CALLBACK_2(WinRewardLayer::onBtnClick, this));

	//按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->setTag(1000);
	actionbtn->addTouchEventListener(CC_CALLBACK_2(WinRewardLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("allgetbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	//按钮
	cocos2d::ui::Widget* continuebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn_0");
	continuebtn->setTag(1001);
	continuebtn->addTouchEventListener(CC_CALLBACK_2(WinRewardLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* continuebtntxt = (cocos2d::ui::ImageView*)continuebtn->getChildByName("text");
	continuebtntxt->loadTexture(ResourcePath::makeTextImgPath("continuegamebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* droptxt = (cocos2d::ui::ImageView*)csbnode->getChildByName("winsepbox")->getChildByName("text");
	droptxt->loadTexture(ResourcePath::makeTextImgPath("drop_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* mypackagetxt = (cocos2d::ui::ImageView*)csbnode->getChildByName("winsepbox_0")->getChildByName("text");
	mypackagetxt->loadTexture(ResourcePath::makeTextImgPath("package_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* carrytext = (cocos2d::ui::Text*)csbnode->getChildByName("carrytext");
	carrytext->setString(ResourceLang::map_lang["carrytext"]);

	carrycountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("carrycount");

	cocos2d::ui::ScrollView* dropscrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	dropscrollview->setScrollBarEnabled(false);
	dropscrollview->setBounceEnabled(true);
	vec_scrollview.push_back(dropscrollview);

	cocos2d::ui::ScrollView* mypackagescrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview0");
	mypackagescrollview->setScrollBarEnabled(false);
	mypackagescrollview->setBounceEnabled(true);
	vec_scrollview.push_back(mypackagescrollview);

	loadScrollviewData();
	updateScrollviewContent();

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void WinRewardLayer::updateScrollviewContent()
{
	for (unsigned int i = 0; i < 2; i++)
	{
		cocos2d::ui::ScrollView* sv = vec_scrollview[i];
		sv->removeAllChildrenWithCleanup(true);

		std::vector<ResBase*> vec_res;
		if (i == 0)
			vec_res = vec_dropdownres;
		else
			vec_res = vec_mypackagres;
		int ressize = vec_res.size();
		int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);

		int itemheight = 160;

		int innerheight = itemheight * row;

		int contentheight = sv->getContentSize().height;
		if (innerheight < contentheight)
			innerheight = contentheight;
		sv->setInnerContainerSize(Size(sv->getContentSize().width, innerheight));


		for (unsigned int m = 0; m < vec_res.size(); m++)
		{
			std::string qustr = "ui/resbox.png";
			int qu = 0;
			if (vec_res[m]->getType() >= T_ARMOR && vec_res[m]->getType() <= T_NG)
			{
				qu = ((Equipable*)vec_res[m])->getQU().getValue();
				qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}

			Sprite * qubox = Sprite::createWithSpriteFrameName(qustr);

			MenuItemSprite* boxItem = MenuItemSprite::create(
				qubox,
				qubox,
				qubox,
				CC_CALLBACK_1(WinRewardLayer::onclick, this));
			boxItem->setUserData((void*)vec_res[m]);

			boxItem->setPosition(Vec2(qubox->getContentSize().width / 2 + 20 + m % 4 * 160, innerheight - itemheight / 2 - m / 4 * itemheight));

			MyMenu* menu = MyMenu::create();
			menu->addChild(boxItem);
			menu->setTouchlimit(sv);
			menu->setPosition(Vec2(0, 0));

			sv->addChild(menu);

			std::string resid = vec_res[m]->getId();

			std::string str = StringUtils::format("ui/%s.png", resid.c_str());
			if (qu == 3)
			{
				str = StringUtils::format("ui/%s_2.png", resid.c_str());
			}
			else if (qu == 4)
			{
				str = StringUtils::format("ui/%s_3.png", resid.c_str());
			}

			Sprite * res = Sprite::createWithSpriteFrameName(str);
			res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
			boxItem->addChild(res);

			Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
			namelbl->setColor(Color3B(34, 74, 79));
			namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -20));
			boxItem->addChild(namelbl);

			std::string countstr = StringUtils::format("%d", vec_res[m]->getCount().getValue());
			Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
			countlbl->setAnchorPoint(Vec2(1, 0));
			countlbl->setColor(Color3B::WHITE);
			countlbl->setPosition(Vec2(boxItem->getContentSize().width - 10, 10));
			boxItem->addChild(countlbl);
		}
	}
}

void WinRewardLayer::loadScrollviewData()
{
	for (unsigned int i = 0; i < m_rewards.size(); i++)
	{
		ResBase* res = new ResBase();
		res->setId(m_rewards[i].sid);
		DynamicValueInt dv;
		dv.setValue(m_rewards[i].intPara1);
		res->setCount(dv);
		dv.setValue(m_rewards[i].intPara2);
		res->setQU(dv);
		vec_dropdownres.push_back(res);
	}

	for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
	{
		ResBase* res = MyRes::vec_MyResources[i];
		if (res->getWhere() == MYPACKAGE)
			vec_mypackagres.push_back(res);
	}

}

void WinRewardLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* btnnode = (Node*)pSender;
		int tag = btnnode->getTag();
		switch (tag)
		{
		case 1000://allget
		{
			break;
		}
		case 1001://continue
		{

		}
			break;
		case 1002://close
			this->getParent()->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void WinRewardLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
}