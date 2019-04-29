#pragma once
#pragma once
#ifndef _Hospital_LAYER_H_
#define _Hospital_LAYER_H_
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "Hero.h"
#include "MyHeroNode.h"

USING_NS_CC;

class HospitalLayer :public Layer
{
public:
	HospitalLayer();
	~HospitalLayer();

	virtual bool init();

	CREATE_FUNC(HospitalLayer);
	void updateContent();

	void delayShowNewerGuide(float dt);
	void showNewerGuide(int step);

private:
	Node* m_csbnode;
	cocos2d::ui::ScrollView* scrollview;
	std::vector<Hero*> vec_deadheros;
	cocos2d::ui::Text* cointext;
	cocos2d::ui::Text* silvertext;
	cocos2d::ui::Text* hintdesc;
	cocos2d::ui::Text* revivecountlbl;
	cocos2d::ui::Text* revivecounttext;
	std::vector<MyHeroNode*> vec_deadNodes;
	int dcount;
	cocos2d::ui::Widget* closebtn;
	cocos2d::ui::Text* repairtimelbl;
	cocos2d::ui::Widget* repairbtn;
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void todoNewGuide();
	void updateUI(float dt);
	void updateRepairUi();
public:

};
#endif