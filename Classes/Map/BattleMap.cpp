#include"BattleMap.h"
#include"CloseMonster.h"
#include"RemoteMonster.h"
#include"Monster.h"
#include"TreasureBoxes.h"
#include"HealingVial.h"
#include"PowerVest.h"
#include"NormalBattleScene.h"
#include"Player.h"
#include"PlayerMove.h"
#include"AllTag.h"
#include"Coin.h"
#include"Entity.h"
#include"ShieldBattery.h"
bool BattleMap::init()
{



	return 1;
}
void BattleMap::MapInit(int mode)
{
	int i = rand() % 3;
	if (mode == 0)//战斗房间
	{

		char s[40];
		sprintf(s, "maps/BattleScene%d.tmx", i);
		m_map = TMXTiledMap::create(s);
		m_map->setTag(NormalRoom_TAG);
		this->addChild(m_map);
	}
	if (mode == 1)//商店房间 
	{
		m_map = TMXTiledMap::create("maps/ShopScene.tmx");
		m_map->setTag(ShopRoom_TAG);
		this->addChild(m_map);
	}

}
void BattleMap::bindScene(NormalBattleScene* scene)
{
	m_scene = scene;
}

TMXTiledMap* BattleMap::getBattleMap()
{
	return m_map;
}

Vector<Monster*> BattleMap::getMonster()
{
	return Vector<Monster*>(m_monster);
}
std::vector <TreasureBoxes*> BattleMap::getBox()
{
	return m_box;
}
std::vector<Item*> BattleMap::getItems()
{
	return m_Items;
}
void BattleMap::BoxInit()
{
	m_box.push_back(TreasureBoxes::create());

}
void BattleMap::ItemInit()
{
	//随机创建血瓶,力量药水,护盾电池 
	int i = rand() % 3;
//	i = 2;
	if(i==0)
		m_Items.push_back(HealingVial::create());
	else if(i==1)
		m_Items.push_back(PowerVest::create());
	else if(i==2)
		m_Items.push_back(ShieldBattery::create());

}
void BattleMap::BoxCreate()
{
	m_box.back()->setTag(TreasureBox_TAG);
	m_box.back()->bindMap(this);
	m_box.back()->bindPlayer(m_scene->getPlayer());
	this->addChild(m_box.back(), 1);
}
void BattleMap::ItemCreate()
{
	this->addChild(m_Items.back(), 10);
	m_Items.back()->setPosition(Vec2(this->getBox().back()->getPosition()));
	m_Items.back()->getSprite()->setOpacity(0);
	m_Items.back()->bindPlayer(m_scene->getPlayer());

	m_Items.back()->bindMap(this);
	m_Items.back()->notice("");
}
void BattleMap::ItemInBoxUpdate(float dt)
{
	//宝箱中的Item
	for (int i = 0; i < m_box.size(); i++)
	{
		//宝箱能看见，宝箱被使用，宝箱被打开
		if (m_box.at(i)->getIsCanSee() == 1 && m_box.at(i)->getIsUsed() == 1 && m_box.at(i)->getIsOpen() == 1)
		{
			m_Items.at(i)->getSprite()->setOpacity(255);
			//按下R
			if (m_scene->getPlayer()->getplayermove()->getkeyMap()[EventKeyboard::KeyCode::KEY_R])
			{
				if(m_map->getTag()!=ShopRoom_TAG)
					m_Items.at(i)->Interact(0);//mode：0，商店里要花钱的
				else if(m_map->getTag()==ShopRoom_TAG)
					m_Items.at(i)->Interact(1);//mode：1，战斗地图的白嫖奖励
			}
			//玩家在周围，抬头提示
			if (m_Items.at(i)->isAround(50))
			{
				m_Items.at(i)->NoticeOnorOff(1);
			}
			else
				m_Items.at(i)->NoticeOnorOff(0);

		}
		else
		{
			m_Items.at(i)->NoticeOnorOff(0);
		}
		
		if (m_box.at(i)->getIsOpen() == 0)
			m_Items.at(i)->getSprite()->setOpacity(0);
		else
			m_Items.at(i)->getSprite()->setOpacity(255);
	}

}
void BattleMap::createMonster(int MonsterNum)
{

	char s1[30] = "O_small_monster";
	char s2[30] = "pig_monster";
	char s3[30] = "snow_monster";
	char s4[30] = "Y_craw_monster";

	for (int i = 0; i < MonsterNum; i++)
	{
		char Monster_name[30];
		int Monster_type = rand()%4+1;
		
		if (Monster_type == 1)
		{
			strcpy(Monster_name,s1);
			m_monster.pushBack(RemoteMonster::create());
			m_monster.back()->setTag(AllTag::O_small_monster_TAG);
		}
		else if(Monster_type == 2)
		{
			strcpy(Monster_name, s2);
			m_monster.pushBack(CloseMonster::create());
			m_monster.back()->setTag(AllTag::pig_monster_TAG);
		}
		else if (Monster_type == 3)
		{
			strcpy(Monster_name, s3);
			m_monster.pushBack(CloseMonster::create());
			m_monster.back()->setTag(AllTag::snow_monster_TAG);
		}
		else if (Monster_type == 4)
		{
			strcpy(Monster_name, s4);
			m_monster.pushBack(RemoteMonster::create());
			m_monster.back()->setTag(AllTag::Y_craw_monster_TAG);
		}


		m_monster.back()->InitWithName(Monster_name);
		m_monster.back()->FramCacheInit(Monster_name);
		m_monster.back()->bindScene(m_scene);
		m_monster.back()->Birth(i+1);
//		m_monster.back()->setTag(1);
		this->addChild(m_monster.back());

	}


}
void BattleMap::DropsInit()
{
	for (int i = 0; i < m_monster.size(); i++)
	{
		m_Drops.push_back(Coin::create());
	}

}
void BattleMap::DropsCreate()
{
	for (int i = 0; i < m_Drops.size(); i++)
	{
		this->addChild(m_Drops.at(i), 11);
		m_Drops.at(i)->setTag(Coin_TAG);

		m_Drops.at(i)->getSprite()->setOpacity(0);
		m_Drops.at(i)->bindPlayer(m_scene->getPlayer());
		m_Drops.at(i)->bindMap(this);
	}

}
void BattleMap::DropsUpdate(float dt)
{
	for (int i = 0; i < m_monster.size(); i++)
	{
		if (m_monster.at(i)->getIsDead()&&m_Drops.at(i)->getIsCanSee()==0&& m_Drops.at(i)->getIsUsed()==0)
		{
			//掉落物
			m_Drops.at(i)->getSprite()->setOpacity(255);
			m_Drops.at(i)->setPosition(m_monster.at(i)->getPosition());
		}
	}
	for (int i = 0; i < m_Drops.size(); i++)
	{
		if (m_Drops.at(i)->isAround(30)&& m_Drops.at(i)->getIsCanSee()==0&& m_Drops.at(i)->getIsUsed()==0)
		{
			m_Drops.at(i)->Interact(0);
			m_Drops.at(i)->getSprite()->setOpacity(0);
		}
	}
}
std::vector<Item*> BattleMap::getDrops()
{
	return m_Drops;
}
void BattleMap::MonsterUpdate(float dt)
{


}
int BattleMap::getNumber()
{
	return Number;
}
void BattleMap::setNumber(int i)
{
	Number = i;
}


