#include "HillResNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "TaskMainNode.h"
#include "TaskMainDescLayer.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"

TaskMainNode::TaskMainNode()
{

}


TaskMainNode::~TaskMainNode()
{

}

TaskMainNode* TaskMainNode::create(TaskMainData* data, TaskLayer* layer)
{
	TaskMainNode *pRet = new(std::nothrow)TaskMainNode();
	if (pRet && pRet->init(data,layer))
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

bool TaskMainNode::init(TaskMainData* data, TaskLayer* layer)
{
	m_layer = layer;
	m_Data = data;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("taskMainNode.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* resitem = (cocos2d::ui::ImageView*)csbnode->getChildByName("resitem");
	resitem->addTouchEventListener(CC_CALLBACK_2(TaskMainNode::onImgClick, this));
	resitem->setSwallowTouches(false);

	//����
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(data->name);

	//��ʾ���
	redpoint = (cocos2d::ui::ImageView*)csbnode->getChildByName("redpoint");

	//
	finish = (cocos2d::ui::Widget*)csbnode->getChildByName("finish");
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)finish->getChildByName("Image");
	text->loadTexture(ResourcePath::makeTextImgPath("taskfinish", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	updateData(0);

	this->schedule(schedule_selector(TaskMainNode::updateData), 1.0f);
	return true;
}

void TaskMainNode::updateData(float dt)
{
	if (m_Data->isfinish == QUEST_FINISH)
	{
		finish->setVisible(true);
	}
	else if (m_Data->isfinish == QUEST_GET)
	{
		finish->setVisible(true);
		redpoint->setVisible(false);
	}
	else
	{
		finish->setVisible(false);
		redpoint->setVisible(false);
		if (m_Data->id == GlobalInstance::myCurMainData.id)
		{
			redpoint->setVisible(true);
		}
	}

}

void TaskMainNode::onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		clickflag = true;
		beginTouchPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));
	}
	else if (type == ui::Widget::TouchEventType::MOVED)
	{
		Vec2 movedPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));

		if (fabs(movedPoint.x - beginTouchPoint.x) >= CLICKOFFSETP || fabs(movedPoint.y - beginTouchPoint.y) >= CLICKOFFSETP)
			clickflag = false;
	}
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!clickflag)
			return;

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		Layer* layer = TaskMainDescLayer::create(m_Data);
		if (m_layer!=NULL)
		{
			m_layer->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
	}
}