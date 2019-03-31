#include "MacthFightSucAwdLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "AnimationEffect.h"

std::string awds[] = {"s002-2","m002-2", "r006-1000", "r012-20","q001-5", "q002-5", "q003-5","i002-1", "r001-100", "r002-100", "r003-100","r004-100", "r005-100", "r007-10","r008-10", "r009-10" };
int rnd[] = { 5,10,20,40,45,50,55,60,65,70,75,80,85,90,95,100 };

MacthFightSucAwdLayer::MacthFightSucAwdLayer()
{
	iscancolse = false;
}


MacthFightSucAwdLayer::~MacthFightSucAwdLayer()
{

}

bool MacthFightSucAwdLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("matchfightSuccAwdLayer.csb"));
	this->addChild(m_csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	//����
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("title");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("matchfightawd_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	for (int i = 0; i < 3; i++)
	{
		std::string  name = StringUtils::format("box%d", i);
		box[i] = (cocos2d::ui::ImageView*)m_csbnode->getChildByName(name);
		box[i]->setTag(i);
		box[i]->addTouchEventListener(CC_CALLBACK_2(MacthFightSucAwdLayer::onImgClick, this));
	}

	desc = (cocos2d::ui::Text*)m_csbnode->getChildByName("desc");
	desc->setString(ResourceLang::map_lang["matchfightawddesc"]);

	animnode = m_csbnode->getChildByName("animnode");
	animnode->setVisible(false);

	resbox = (cocos2d::ui::ImageView*)animnode->getChildByName("resbox");
	res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		if (iscancolse)
			AnimationEffect::closeAniEffect(this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void MacthFightSucAwdLayer::onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clickbtn = (Node*)pSender;
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		int tag = clickbtn->getTag();

		box[tag]->loadTexture("ui/matchfightawdbox1.png", cocos2d::ui::Widget::TextureResType::PLIST);
		this->scheduleOnce(schedule_selector(MacthFightSucAwdLayer::delayShowResAnim), 0.2f);
		this->setTag(tag);
		for (int i = 0; i < 3; i++)
			box[i]->setEnabled(false);
	}
}

void MacthFightSucAwdLayer::delayShowResAnim(float dt)
{
	int r = GlobalInstance::getInstance()->createRandomNum(100);

	int index = 0;
	for (int i = 0; i < sizeof(rnd) / sizeof(rnd[0]); i++)
	{
		if (r < rnd[i])
		{
			index = i;
			break;
		}
	}

	std::vector<std::string> vec_ret;

	std::string qustr = "ui/resbox.png";
	int t = 0;
	int qu = 0;
	std::string resid;
	int count = 0;
	CommonFuncs::split(awds[index], vec_ret, "-");
	{
		resid = vec_ret[0];
		count = atoi(vec_ret[1].c_str());
		if (vec_ret.size() >= 3)
			qu = atoi(vec_ret[2].c_str());
	};

	MyRes::Add(resid, count, MYSTORAGE, qu, GlobalInstance::getInstance()->generateStoneCount(qu));

	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		qu = GlobalInstance::map_GF[resid].qu;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_RENS && t <= T_BOX)
	{
		qu = atoi(resid.substr(1).c_str()) - 1;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_HEROCARD && t <= T_ARMCARD)
	{
		qu = 3;
		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}

	resbox->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);
	std::string residstr = StringUtils::format("ui/%s.png", resid.c_str());
	res->loadTexture(residstr, cocos2d::ui::Widget::TextureResType::PLIST);
	animnode->setPosition(box[this->getTag()]->getPosition());
	animnode->setVisible(true);

	auto action = CSLoader::createTimeline(ResourcePath::makePath("matchfightSuccAwdLayer.csb"));
	animnode->runAction(action);
	action->gotoFrameAndPlay(0, false);
	box[this->getTag()]->runAction(Sequence::create(DelayTime::create(0.5f), FadeTo::create(0.02f, 200), FadeTo::create(0.02f, 100), CallFunc::create(CC_CALLBACK_0(MacthFightSucAwdLayer::finishAnim, this)), NULL));
	
	CommonFuncs::playResBoxEffect(resbox, t, qu, 0);
}


void MacthFightSucAwdLayer::finishAnim()
{
	iscancolse = true;
	desc->setString(ResourceLang::map_lang["matchfightawddesc1"]);
}