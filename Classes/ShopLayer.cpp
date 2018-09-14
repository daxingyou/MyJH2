#include "ShopLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MainScene.h"
#include "AnimationEffect.h"
#include "ShopNode.h"
#include "DynamicValue.h"
#include "MyRes.h"
#include "GiftContentLayer.h"

USING_NS_CC;

int ShopLayer::payindex = -1;
bool ShopLayer::isPaying = false;

ShopLayer::ShopLayer()
{

}

ShopLayer::~ShopLayer()
{

}


ShopLayer* ShopLayer::create()
{
	ShopLayer *pRet = new(std::nothrow)ShopLayer();
	if (pRet && pRet->init())
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

// on "init" you need to initialize your instance
bool ShopLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("shopLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//��ť
	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(ShopLayer::onBtnClick, this));

	cocos2d::ui::Text* text = (cocos2d::ui::Text*)csbnode->getChildByName("text");
	text->setString(ResourceLang::map_lang["officalQQ"]);

	coinlbl = (cocos2d::ui::Text*)csbnode->getChildByName("coinbox")->getChildByName("countlbl");

	scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollView");

	int size = GlobalInstance::vec_shopdata.size();
	int itemheight = 157;
	int innerheight = itemheight * size;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(640, innerheight));

	for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
	{
		ShopNode* node = ShopNode::create(&GlobalInstance::vec_shopdata[i]);
		node->setTag(i);
		scrollView->addChild(node);
		//node->setPosition(Vec2(scrollView->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
		node->setPosition(Vec2(scrollView->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
		node->runAction(EaseSineIn::create(MoveBy::create(0.15f + i*0.07f, Vec2(-scrollView->getContentSize().width / 2 - 600, 0))));
	}

	updateCoinLable(0);
	this->schedule(schedule_selector(ShopLayer::updateCoinLable), 0.1f);
	//�����²���
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	/*listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};*/
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void ShopLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}

void ShopLayer::updateCoinLable(float dt)
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	coinlbl->setString(str);
}

void ShopLayer::onExit()
{
	Layer::onExit();
}

void ShopLayer::beginPay(int index)
{
	if (isPaying)
		return;

	isPaying = true;
	payindex = index;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	setMessage(PAY_SUCC);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	JniMethodInfo methodInfo;
	char p_str1[32] = { 0 };
	sprintf(p_str1, "%s", "paycode.1");
	char p_str2[32] = { 0 };
	sprintf(p_str2, "%s", "paycode desc");
	if (JniHelper::getStaticMethodInfo(methodInfo, "com/csfb/myjh/PayAction", "pay", "(Ljava/lang/String;Ljava/lang/String;I)V"))
	{
		jstring str1 = methodInfo.env->NewStringUTF(p_str1);
		jstring str2 = methodInfo.env->NewStringUTF(p_str2);
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, str1, str2, 0);
	}
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	//payCode
	//buy((char*)payCode[payindex].c_str());
#endif
}


void ShopLayer::setMessage(PYARET ret)
{
	if (ret == PAY_SUCC && payindex >= 0)
	{
		int type = GlobalInstance::vec_shopdata[payindex].type;
		if (type == COIN)//Ԫ��
		{
			for (unsigned int i = 0; i < GlobalInstance::vec_shopdata[payindex].res.size(); i++)
			{
				std::vector<std::string> vec_res = GlobalInstance::vec_shopdata[payindex].res[i];
				std::string resid = vec_res[0];
				int count = atoi(vec_res[1].c_str());
				DynamicValueInt dal;
				dal.setValue(count);
				GlobalInstance::getInstance()->addMyCoinCount(dal);
			}
		}
		else if (type == GIFT)//���
		{
			for (unsigned int i = 0; i < GlobalInstance::vec_shopdata[payindex].res.size(); i++)
			{
				std::vector<std::string> vec_res = GlobalInstance::vec_shopdata[payindex].res[i];
				std::string resid = vec_res[0];
				int count = atoi(vec_res[1].c_str());
				int qu = 0;
				if (vec_res.size() > 2)
				{
					qu = atoi(vec_res[2].c_str());
				}
				int stonescount = GlobalInstance::getInstance()->generateStoneCount(qu);
				MyRes::Add(resid, count, MYSTORAGE, qu, stonescount);
			}
		}
		else if (type == VIP)//�¿�
		{
			showVipReward(&GlobalInstance::vec_shopdata[payindex], payindex);
		}
	}
	std::string str = StringUtils::format("buy_%d", (int)ret);
	MovingLabel::show(ResourceLang::map_lang[str]);
	payindex = -1;
	isPaying = false;
}

void ShopLayer::showVipReward(ShopData* data, int tag)
{
	GiftContentLayer* layer = GiftContentLayer::create(data, tag, 1);
	if (g_mainScene != NULL)
	{
		g_mainScene->addChild(layer, 10, "viprewardlayer");
	}
}