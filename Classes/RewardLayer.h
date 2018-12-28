#ifndef __REWARD_LAYER_H__
#define __REWARD_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalInstance.h"
#include "ResBase.h"
#include "HttpDataSwap.h"

USING_NS_CC;

class RewardLayer : public cocos2d::Layer, public HTTPDataDelegateProtocol
{
public:
	RewardLayer();
	~RewardLayer();

	static RewardLayer* create(std::vector<MSGAWDSDATA> vec_rewards, int forwhere = MYSTORAGE, int type = 0);//type:0--正常领取；1--竞技场领取；2--宝箱领取，基础资源放到背包，其他方到仓库
	bool init(std::vector<MSGAWDSDATA> vec_rewards, int forwhere = MYSTORAGE, int type = 0);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void removeSelf();

private:

};

#endif
