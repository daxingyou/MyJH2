﻿#include "Equip.h"
#include "Const.h"
#include "GlobalInstance.h"
Equip::Equip()
{
	m_qu.setValue(0);
	m_lv.setValue(0);
}


Equip::~Equip()
{
}

//攻击
float Equip::getAtk()
{
	int equ = getQU().getValue();
	float ebns = POTENTIAL_BNS[equ];
	float atk = 0.0f;
	atk = ebns * GlobalInstance::map_Equip[getId()].atk;
	if (getType() == T_ARMOR)
	{
		//atk += getLv().getValue() *(getLv().getValue()*0.85f + atk * 0.13f + 3);
		atk += getLv().getValue() * (getLv().getValue() * 1.00f + getLv().getValue() * atk * 0.010f + atk * 0.015 + 5);
	}
	for (unsigned int i = 0; i < vec_stones.size(); i++)
	{
		std::string stoneid = vec_stones[i];
		if (stoneid.length() > 1)
		{
			int intid = atoi(stoneid.substr(1).c_str()) - 1;
			int intv = intid / 3;
			if (intv == 0)
				atk += STONE_BNS[intv][intid%3];
		}
	}
	return atk;
}

float Equip::getDf()
{
	int equ = getQU().getValue();
	float ebns = POTENTIAL_BNS[equ];
	float df = 0.0f;
	df = ebns *  GlobalInstance::map_Equip[getId()].df;
	if (getType() == T_EQUIP)
	{
		df += getLv().getValue() *(getLv().getValue()*0.55f + getLv().getValue() * df * 0.012f + df * 0.015 + 3);
	}
	else if (getType() == T_HANDARMOR || getType() == T_FASHION)
	{
		df += getLv().getValue() * (getLv().getValue() * 0.2f + getLv().getValue() * df * 0.005f + df * 0.01 + 2);
	}
	for (unsigned int i = 0; i < vec_stones.size(); i++)
	{
		std::string stoneid = vec_stones[i];
		if (stoneid.length() > 1)
		{
			int intid = atoi(stoneid.substr(1).c_str()) - 1;
			int intv = intid / 3;

			if (intv == 1)
				df += STONE_BNS[intv][intid % 3];
		}
	}
	return df;
}

//血量
float Equip::getHp()
{
	int equ = getQU().getValue();
	float ebns = POTENTIAL_BNS[equ];
	float hp = 0.0f;
	hp = ebns * GlobalInstance::map_Equip[getId()].maxhp;
	if (getType() == T_FASHION)
	{
		//hp += getLv().getValue() *(getLv().getValue()*1.15f + hp * 0.11f + 11);
		hp += getLv().getValue() * (getLv().getValue() * 1.00f + getLv().getValue() * hp * 0.008f + hp * 0.012 + 10);
	}
	for (unsigned int i = 0; i < vec_stones.size(); i++)
	{
		std::string stoneid = vec_stones[i];
		if (stoneid.length() > 1)
		{
			int intid = atoi(stoneid.substr(1).c_str()) - 1;
			int intv = intid / 3;
			if (intv == 2)
				hp += STONE_BNS[intv][intid % 3];
		}
	}
	return hp;
}

//防御
float Equip::getSuitDf()
{
	int equ = getQU().getValue();
	float ebns = POTENTIAL_BNS[equ];
	float df = 0.0f;
	df = ebns *  GlobalInstance::map_Equip[getId()].df;

	if (GlobalInstance::map_EquipSuit.find(getId()) != GlobalInstance::map_EquipSuit.end() && GlobalInstance::map_EquipSuit[getId()].vec_suit.size() >= 3)
	{
		std::string eid = GlobalInstance::map_EquipSuit[getId()].vec_suit[2];
		df = df * GlobalInstance::map_EquipSuit[getId()].vec_bns[1] / 100;
	}
	return df;
}
//血量
float Equip::getSuitHp()
{
	int equ = getQU().getValue();
	float ebns = POTENTIAL_BNS[equ];
	float hp = 0.0f;
	hp = ebns * GlobalInstance::map_Equip[getId()].maxhp;
	if (GlobalInstance::map_EquipSuit.find(getId()) != GlobalInstance::map_EquipSuit.end() && GlobalInstance::map_EquipSuit[getId()].vec_suit.size() >= 2)
	{
		std::string eid = GlobalInstance::map_EquipSuit[getId()].vec_suit[1];
		hp = hp * GlobalInstance::map_EquipSuit[getId()].vec_bns[0] / 100;
	}
	return hp;
}
//攻击速度
float Equip::getAtkSpeed()
{
	int equ = getQU().getValue();
	float ebns = POTENTIAL_BNS[equ];
	float atkspeed = 0.0f;

	atkspeed = ebns * GlobalInstance::map_Equip[getId()].speed;

	return atkspeed;
}
//暴击
float Equip::getCrit()
{
	int equ = getQU().getValue();
	float ebns = POTENTIAL_BNS[equ];
	float crit = 0.0f;
	crit = ebns * GlobalInstance::map_Equip[getId()].crit;
	for (unsigned int i = 0; i < vec_stones.size(); i++)
	{
		std::string stoneid = vec_stones[i];
		if (stoneid.length() > 1)
		{
			int intid = atoi(stoneid.substr(1).c_str()) - 1;
			int intv = intid / 3;
			if (intv == 4)
				crit += STONE_BNS[intv][intid % 3];
		}
	}
	return crit;
}
//闪避
float Equip::getDodge()
{
	int equ = getQU().getValue();
	float ebns = POTENTIAL_BNS[equ];
	float dodge = 0.0f;

	dodge = ebns * GlobalInstance::map_Equip[getId()].avoid;
	if (getType() == T_HANDARMOR)
	{
		for (unsigned int i = 0; i < getLv().getValue(); i++)
		{
			if (i < 5)
				dodge += dodge * 0.01f * (i + 1);
			else
				break;
		}
		if (getLv().getValue() > 5)
			dodge += dodge * 0.05f * (getLv().getValue() - 5);
		if (getLv().getValue() >= 15)
			dodge += dodge * 0.02f * (getLv().getValue() - 14);

	}
	for (unsigned int i = 0; i < vec_stones.size(); i++)
	{
		std::string stoneid = vec_stones[i];
		if (stoneid.length() > 1)
		{
			int intid = atoi(stoneid.substr(1).c_str()) - 1;
			int intv = intid / 3;
			if (intv == 3)
				dodge += STONE_BNS[intv][intid % 3];
		}
	}
	return dodge;
}