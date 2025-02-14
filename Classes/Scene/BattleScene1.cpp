#include"BattleScene1.h"
#include"music/music.h"
#include"Map/BattleMap.h"
#include"Player.h"
#include"PlayerMove.h"
#include"PlayerAttribute.h"
#include"Pistol.h"
#include"PistolAmmo.h"
#include"Monster.h"
#include"RemoteMonster.h"
#include"MonsterDashAmmo.h"
#include"TreasureBoxes.h"
#include"miniMapTab.h"
#include"SafeScene.h"
#include"HealingVial.h"
#include"WeaponManager.h"
#include"Coin.h"
#include"PitchForkAmmo.h"
#include"StartScene.h"
#define PI 3.1415926f
static long long SwordEnd;
static long long PistolEnd;
//记录上一颗子弹消失的时刻，后续在生成子弹时，需起码与此间隔0.3s
bool BattleScene1::init()
{
	
	srand((unsigned)time(NULL));//根据时间取随机种子
	auto visibleSize = Director::getInstance()->getVisibleSize();
	
	if (initWithPhysics()) {
		getPhysicsWorld()->setGravity(Vec2::ZERO);
		getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
		getPhysicsWorld()->setSubsteps(20);
	}
	getmusicManager()->changeMusic("bgm/Room.mp3");
	Mapinit();
	//设置按钮
	auto settings = MenuItemImage::create("ui/settings.png", "ui/settings.png", [&](Ref* sender) {
		getmusicManager()->effectPlay("effect/button.mp3");
		getmusicManager()->menu(this);
	});
	//位置位于左上角
	settings->setPosition(visibleSize.width - settings->getContentSize().width / 2, visibleSize.height - settings->getContentSize().height / 2);
	//菜单
	auto menu = Menu::create(settings, nullptr);
	menu->setPosition(Vec2::ZERO);
	addChild(menu, 3);

	//玩家创建
	auto physicsBody = PhysicsBody::createBox(Size(40.0f, 40.0f),
		PhysicsMaterial(0.0f, 0.0f, 0.0f));
	physicsBody->setDynamic(false);
	bindPlayer(Player::create());
	getPlayer()->setTag(AllTag::Player_TAG);
//	getPlayer()->PistolInit();//手枪
//	getPlayer()->SwordInit();//剑
	/*
	bindWeaponManager(WeaponManager::create());
	getWeaponManager()->bindPlayer(getPlayer());
	getWeaponManager()->getWeapon(getPlayer()->getWeapon1(), getPlayer()->getWeapon2());//创建WeaponManager并与角色绑定
	*/
	getPlayer()->addComponent(physicsBody);
	getPlayer()->getPhysicsBody()->setCategoryBitmask(0x0010);
	getPlayer()->getPhysicsBody()->setCollisionBitmask(0x0010);
	getPlayer()->getPhysicsBody()->setContactTestBitmask(0x0010);
	getPlayer()->setPosition(64 * 2 + 32, 64 * 2 + 32);
	getPlayer()->getplayermove()->bindMap(parentMap->getBattleMap());//PlayerMove跟这个地图绑定
	getPlayer()->getPlayerAttribute()->setPosition(getPlayer()->getPlayerAttribute()->getSprite()->getContentSize().width / 2,
		visibleSize.height - getPlayer()->getPlayerAttribute()->getSprite()->getContentSize().height / 2);//属性UI位置设置
	this->addChild(getPlayer()->getPlayerAttribute(), 4);
	this->addChild(getPlayer(), 2);
	//宝箱创建,血瓶创建
	if (parentMap->getBattleMap()->getTag() == NormalRoom_TAG)
	{
		parentMap->BoxInit();
		parentMap->getBox().back()->bindScene(this);
		parentMap->BoxCreate();
		parentMap->getBox().back()->BoxBirth(1);

		parentMap->ItemInit();//宝箱里的物品创建
		parentMap->getItems().back()->bindScene(this);
		parentMap->ItemCreate();

		parentMap->schedule(CC_SCHEDULE_SELECTOR(BattleMap::ItemInBoxUpdate), 0.1f);
	}


	parentMap->DropsInit();//打怪的掉落物
	for (int i = 0; i < parentMap->getDrops().size(); i++)
	{
		parentMap->getDrops().at(i)->bindScene(this);
	}
	parentMap->DropsCreate();
	parentMap->schedule(CC_SCHEDULE_SELECTOR(BattleMap::DropsUpdate), 0.1f);

	//小地图创建
	MiniMap = miniMapTab::create();
	MiniMap->bindBattleScene(this);
	this->addChild(MiniMap, 10);
	MiniMap->setScale(1);
	MiniMap->setPosition(Vec2(visibleSize.width-MiniMap->getSprite()->getContentSize().width/2
	, MiniMap->getSprite()->getContentSize().height / 2));

	

	////eventlistener,键盘监听，用于移动人物
	auto myKeyListener = EventListenerKeyboard::create();
	myKeyListener->onKeyPressed = [=](EventKeyboard::KeyCode keycode, cocos2d::Event* event)//键盘按下时响应
	{
		getPlayer()->getplayermove()->TrueKeyCode(keycode);//PlayerMove里keyMap的对应键码置true
		if (keycode==EventKeyboard::KeyCode::KEY_E)//按E进门
		{



			inGate();
			for (int i = 0; i < parentMap->getBox().size(); i++)
			{
				if(parentMap->getBattleMap()->getTag()==NormalRoom_TAG)
					parentMap->getBox().at(i)->Interact(" ");
				else if(parentMap->getBattleMap()->getTag()==ShopRoom_TAG)
					parentMap->getBox().at(i)->Interact("$10");
			}


			
		}
		if (keycode == EventKeyboard::KeyCode::KEY_Q)//按Q切换武器
		{
			
			if (getPlayer()->getWeapon2()!=nullptr)
			{
				getPlayer()->swapWeapon();
			}

			

		}
		if (keycode == EventKeyboard::KeyCode::KEY_SPACE)//按空格开技能
		{
			getPlayer()->HeroSkill(0);
		}
	};
	
	myKeyListener->onKeyReleased = [=](EventKeyboard::KeyCode keycode, cocos2d::Event* event)//键盘松开时响应
	{
		getPlayer()->getplayermove()->FalseKeyCode(keycode);
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(myKeyListener, this);

	//鼠标监听
	auto myMouseListener = EventListenerMouse::create();
	myMouseListener->onMouseMove = [=](cocos2d::Event* event)//移动
	{
		EventMouse* e = (EventMouse*)event;
		getPlayer()->changeMouseLocation(e->getLocation());


	};
	myMouseListener->onMouseDown = [=](cocos2d::Event* event)//按下
	{
		EventMouse* e = (EventMouse*)event;
		getPlayer()->trueMouseMap(e->getMouseButton());

	};
	myMouseListener->onMouseUp = [=](cocos2d::Event* event)//松开
	{
		EventMouse* e = (EventMouse*)event;
		getPlayer()->flaseMouseMap(e->getMouseButton());
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(myMouseListener, this);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(BattleScene1::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

	this->scheduleUpdate();
//	this->schedule(CC_SCHEDULE_SELECTOR(BattleScene1::test), 1.0f);
	this->schedule(CC_SCHEDULE_SELECTOR(BattleScene1::DeleteAmmo), 0.001f);
	this->schedule(CC_SCHEDULE_SELECTOR(BattleScene1::Ammoupdate), 0.01f);//每0.01s检测玩家子弹是否需要生成
	this->schedule(CC_SCHEDULE_SELECTOR(BattleScene1::AmmoUpdate_Monster), 0.3f);//怪物子弹
	this->schedule(CC_SCHEDULE_SELECTOR(BattleScene1::LandDamageUpdate), 0.5f);//地形伤害
	this->schedule(CC_SCHEDULE_SELECTOR(BattleScene1::PlayerDeatheUpdate), 1.0f);//
	MapGateInit();
	return 1;
}
void BattleScene1::MapGateInit()//地图门的初始化
{
	for (int i = 1; i <= 5; i++)
	{
		m_mapgate.pushBack(MapGate::create());
		m_mapgate.back()->bindPlayer(getPlayer());
		this->addChild(m_mapgate.back(), 5);
		m_mapgate.back()->IsAble(false);//初始默认该门不可用
	}

	//上
	m_mapgate.at(0)->setPosition(Director::getInstance()->getVisibleSize().width / 2,
		Director::getInstance()->getVisibleSize().height - m_mapgate.back()->getSprite()->getContentSize().height * 0.5 - 64);
	//下
	m_mapgate.at(1)->setPosition(Director::getInstance()->getVisibleSize().width / 2, m_mapgate.back()->getSprite()->getContentSize().height * 0.5 + 64);

	//左
	m_mapgate.at(2)->setPosition(m_mapgate.back()->getSprite()->getContentSize().width/2+64, Director::getInstance()->getVisibleSize().height / 2);
	//右
	m_mapgate.at(3)->setPosition(Director::getInstance()->getVisibleSize().width- m_mapgate.back()->getSprite()->getContentSize().width / 2-64,
		Director::getInstance()->getVisibleSize().height / 2);
	m_mapgate.at(4)->setPosition(Director::getInstance()->getVisibleSize()/2);//Boos房
}
void BattleScene1::inGate()//进门的判断和传送的实现
{
	for (int i = 0; i < 5; i++)
	{
		if (m_mapgate.at(i)->getAble())//获取该门是否可用
		{
			if (m_mapgate.at(i)->isAround(getPlayer()->getPositionX(), getPlayer()->getPositionY()) )//判断玩家在周围
			{
//				CCLOG("%d", parentMap->getNumber());
				switch (i)
				{
				case 0://改变地图并改变玩家位置，即传送
					changeMap(parentMap->getNumber() + 4);
					getPlayer()->setPosition(Director::getInstance()->getVisibleSize().width / 2, m_mapgate.back()->getSprite()->getContentSize().height * 0.5 + 64);
					break;
				case 1:
					changeMap(parentMap->getNumber() - 4);
					getPlayer()->setPosition(Director::getInstance()->getVisibleSize().width / 2,
						Director::getInstance()->getVisibleSize().height - m_mapgate.back()->getSprite()->getContentSize().height * 0.5 - 64);
					break;
				case 2:
					changeMap(parentMap->getNumber() - 1);
					getPlayer()->setPosition(Director::getInstance()->getVisibleSize().width - m_mapgate.back()->getSprite()->getContentSize().width / 2 - 64,
						Director::getInstance()->getVisibleSize().height / 2);
					break;
				case 3:
					changeMap(parentMap->getNumber() + 1);
					getPlayer()->setPosition(m_mapgate.back()->getSprite()->getContentSize().width / 2 + 64,
						Director::getInstance()->getVisibleSize().height / 2);
					break;
				case 4:
					Director::getInstance()->replaceScene(BattleScene1::create());
					break;
				default:
					break;
				}
				break;


			}
			
		}
	}
}
void BattleScene1::update(float dt)
{
	int able = 1;
	for (int i = 0; i < parentMap->getMonster().size(); i++)
	{
		if (parentMap->getMonster().at(i)->getIsDead()==0)//但凡这张图里有一只怪没死，就置为不可用 able=0
			able = 0;
	}
	int x = parentMap->getNumber()%4+1;
	int y = parentMap->getNumber() / 4+1;
	//根据地图的编号位置选择传送门的开启，例如最左侧地图不会开左门
	for (int i = 0; i < 5; i++)
	{
		m_mapgate.at(i)->IsAble(0);
	}
	if (x<=3 && m_battleMap.at(parentMap->getNumber()+1)->getName()!="no")
	{
		m_mapgate.at(3)->IsAble(able);
	}
	if (x>=2 && m_battleMap.at(parentMap->getNumber() - 1)->getName() != "no")
	{
		m_mapgate.at(2)->IsAble(able);
	}
	if (y>=2 && m_battleMap.at(parentMap->getNumber() - 4)->getName() != "no")
	{
		m_mapgate.at(1)->IsAble(able);
	}
	if (y<=3 && m_battleMap.at(parentMap->getNumber() + 4)->getName() != "no")
	{
		m_mapgate.at(0)->IsAble(able);
	}
	if (parentMap->getNumber() == getBossRoom())
		m_mapgate.at(4)->IsAble(able);
	for (int i = 0; i < parentMap->getBox().size(); i++)
		parentMap->getBox().at(i)->setIsCanSee(able);
}
//怪物Pistol子弹
void BattleScene1::AmmoUpdate_Monster(float dt)
{
	//获取玩家坐标
	float Px = getPlayer()->getPositionX() - getPlayer()->getContentSize().width / 2;
	float Py = getPlayer()->getPositionY() - getPlayer()->getContentSize().height / 2;
	for (int i = 0; i < parentMap->getMonster().size(); i++)
	{
		bool CanShoot = 0;
		float Mx = parentMap->getMonster().at(i)->getPositionX();//获取怪物坐标
		float My = parentMap->getMonster().at(i)->getPositionY();
		Vec2 m = Vec2(Vec2(Px - Mx, Py - My) / sqrt((Px - Mx) * (Px - Mx) + (Py - My) * (Py - My)));
//		CCLOG("%d", parentMap->getMonster().at(i)->getTag());
		if (parentMap->getMonster().at(i)->getIsDead() == 0 && //没死
			parentMap->getMonster().at(i)->CanSee()&&			//看见玩家
			((parentMap->getMonster().at(i)->getTag()==AllTag::O_small_monster_TAG)||//是一种远程怪
			(parentMap->getMonster().at(i)->getTag() == AllTag::Y_craw_monster_TAG)))
		{

			m_monsterAmmoList.push_back(parentMap->getMonster().at(i)->MonsterAttack());//生成子弹
			m_monsterAmmoList.back()->start = clock();									//标记出生时间
			m_monsterAmmoList.back()->setTag(AllTag::MonsterAmmo_PistolTAG);
			this->addChild(m_monsterAmmoList.back(), 3);
			auto physicBody = PhysicsBody::createBox(Size(28.0f, 28.0f), PhysicsMaterial(0, 0, 0));//添加刚体
			m_monsterAmmoList.back()->addComponent(physicBody);
			m_monsterAmmoList.back()->getPhysicsBody()->setDynamic(false);
			m_monsterAmmoList.back()->setPosition(parentMap->getMonster().at(i)->getPosition());
			m_monsterAmmoList.back()->getPhysicsBody()->setVelocity(m * 500);

			m_monsterAmmoList.back()->getPhysicsBody()->setCategoryBitmask(0x0010);//掩码，怪物子弹和玩家相同
			m_monsterAmmoList.back()->getPhysicsBody()->setCollisionBitmask(0x0010);
			m_monsterAmmoList.back()->getPhysicsBody()->setContactTestBitmask(0x0010);
		}
	}
}
//玩家子弹的生成
void BattleScene1::Ammoupdate(float dt)
{

	float x = getPlayer()->getMouseLocation().x;
	float y = Director::getInstance()->getVisibleSize().height - getPlayer()->getMouseLocation().y;
	float Px = getPlayer()->getPositionX() - getPlayer()->getContentSize().width / 2;
	float Py = getPlayer()->getPositionY() - getPlayer()->getContentSize().height / 2;
	float Wx = (Px + getPlayer()->getWeapon1()->getPositionX() + getPlayer()->getWeapon1()->getContentSize().width / 2);
	float Wy = (Py + getPlayer()->getWeapon1()->getPositionY());
	Vec2 v = Vec2(Vec2(x - Wx, y - Wy) / sqrt((x - Wx) * (x - Wx) + (y - Wy) * (y - Wy)));//从人物指向鼠标位置的单位向量

	//玩家Pistol子弹
	if (getPlayer()->getMouseMap()[EventMouse::MouseButton::BUTTON_LEFT]		//鼠标左键按下
		&&getPlayer()->getPlayerAttribute()->getHp()>0							//Player没死
		&&(getPlayer()->getWeapon1()->getTag()==AllTag::PlayerWeapon_Pistol_TAG
			|| getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapon_Shotgun_TAG))//Weapon是枪
	{
		bool CanShoot = 0;


		if (AmmoList.size() == 0)
		{
			if (clock() - PistolEnd > 
				getPlayer()->getWeapon1 ()->getWeaponSpeed()
				+getPlayer()->getPlayerAttribute()->getShootSpeed()
				- getPlayer()->getPlayerAttribute()->getShootSpeed_Buff())
			{
				CanShoot = 1;
			}

		}
		else if (clock() - AmmoList.back()->start > 
			getPlayer()->getWeapon1()->getWeaponSpeed()
			+getPlayer()->getPlayerAttribute()->getShootSpeed()
			- getPlayer()->getPlayerAttribute()->getShootSpeed_Buff())
			CanShoot = 1;

		if (AmmoList.size() == 0)
		{

		}
		if (CanShoot)
		{
			int Num = 1;
			if (getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapon_Shotgun_TAG)//霰弹则三连发
				Num = 5;
			for (int i = 1; i <= Num; i++)
			{
				AmmoList.push_back(getPlayer()->getWeapon1()->Attack());


				AmmoList.back()->setTag(AllTag::PlayerAmmo_Pistol_TAG);
				AmmoList.back()->start = clock();//每个子弹内部的start负责记录出生时刻
				addChild(AmmoList.back(), 3);
				auto physicBody = PhysicsBody::createBox(Size(20.0f, 20.0f), PhysicsMaterial(0, 0, 0));
				AmmoList.back()->addComponent(physicBody);
				AmmoList.back()->getPhysicsBody()->setDynamic(false);
				AmmoList.back()->setPosition(getPlayer()->getPosition() + getPlayer()->getWeapon1()->getPosition());
				if (i == 1)
					AmmoList.back()->getPhysicsBody()->setVelocity(v * 500);
				else if (i == 2)
				{					
					float ang_v = v.getAngle() + 4 * (PI / 180);
					Vec2 v2 = Vec2(cos(ang_v), sin(ang_v));

					AmmoList.back()->getPhysicsBody()->setVelocity(v2 * 500);
				}
				else if (i == 3)
				{
					float ang_v = v.getAngle() -4  * (PI / 180);
					Vec2 v2 = Vec2(cos(ang_v), sin(ang_v));

					AmmoList.back()->getPhysicsBody()->setVelocity(v2 * 500);
				}
				else if (i == 4)
				{
					float ang_v = v.getAngle() - 8 * (PI / 180);
					Vec2 v2 = Vec2(cos(ang_v), sin(ang_v));

					AmmoList.back()->getPhysicsBody()->setVelocity(v2 * 500);
				}
				else if (i == 5)
				{
					float ang_v = v.getAngle() - 8 * (PI / 180);
					Vec2 v2 = Vec2(cos(ang_v), sin(ang_v));

					AmmoList.back()->getPhysicsBody()->setVelocity(v2 * 500);
				}
				AmmoList.back()->getPhysicsBody()->setCategoryBitmask(0x0001);//0001
				AmmoList.back()->getPhysicsBody()->setCollisionBitmask(0x0001);//0001
				AmmoList.back()->getPhysicsBody()->setContactTestBitmask(0x0001);
			}
		
		}
	
	}
	//玩家Sword子弹 &PitchFor子弹
	if (getPlayer()->getMouseMap()[EventMouse::MouseButton::BUTTON_LEFT] &&
		getPlayer()->getPlayerAttribute()->getHp() > 0 && 
		(getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapon_Sword_TAG
			|| getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapn_PitchFork_TAG))
	{
		bool CanShoot = 0;


		if (AmmoList.size() == 0)
		{
			if (clock() - SwordEnd > 
				getPlayer()->getWeapon1()->getWeaponSpeed()
				+getPlayer()->getPlayerAttribute()->getShootSpeed()
				- getPlayer()->getPlayerAttribute()->getShootSpeed_Buff())
			{
				CanShoot = 1;
			}

		}
		else if (clock() - AmmoList.back()->start > 
			getPlayer()->getWeapon1()->getWeaponSpeed()
			+getPlayer()->getPlayerAttribute()->getShootSpeed()
			- getPlayer()->getPlayerAttribute()->getShootSpeed_Buff())
			CanShoot = 1;
		if (CanShoot)
		{

			AmmoList.push_back(getPlayer()->getWeapon1()->Attack());
			AmmoList.back()->start = clock();
			AmmoList.back()->setTag(AllTag::PlayerAmmo_Sword_TAG);
			addChild(AmmoList.back(), 3);
			auto physicBody = PhysicsBody::createBox(Size(60.0f, 60.0f), PhysicsMaterial(0, 0, 0));
			AmmoList.back()->addComponent(physicBody);
			AmmoList.back()->getPhysicsBody()->setDynamic(false);
			AmmoList.back()->setPosition(getPlayer()->getPosition() + getPlayer()->getWeapon1()->getPosition());
			AmmoList.back()->getPhysicsBody()->setVelocity(v * 1500);
			AmmoList.back()->getPhysicsBody()->setCategoryBitmask(0x0001);//0001
			AmmoList.back()->getPhysicsBody()->setCollisionBitmask(0x0001);//0001
			AmmoList.back()->getPhysicsBody()->setContactTestBitmask(0x0001);
			if (getPlayer()->getIsFlip()&&getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapon_Sword_TAG)
				AmmoList.back()->getSprite()->setFlippedX(1);
			if (getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapn_PitchFork_TAG)
			{
				AmmoList.back()->getSprite()->setRotation(getPlayer()->getWeapon1()->getSprite()->getRotation());
			}
		}

	}
	
	
}

bool BattleScene1::isWall(float x, float y)//判断传入的这个坐标对应在地图上是不是墙体
{
	int mapX = (int)(x / 64);
	int mapY = (int)(11 - int(y / 64));
	int tileGid = parentMap->getBattleMap()->getLayer("wall")->getTileGIDAt(Vec2(mapX, mapY));
	if (tileGid)
		return true;	//是墙

	else
		return false;	//不是墙
}
bool BattleScene1::isDamagingLand(float x, float y)
{
	if (parentMap->getBattleMap()->getTag() == NormalRoom_TAG)
	{
		int mapX = (int)(x / 64);
		int mapY = (int)(11 - int(y / 64));
		int tileGid = parentMap->getBattleMap()->getLayer("DamagingLand")->getTileGIDAt(Vec2(mapX, mapY));
		if (tileGid)
			return true;

		else
			return false;
	}
	else
		return false;

}
//清除子弹
void BattleScene1::DeleteAmmo(float dt)
{
	if (AmmoList.size() != 0)
	{
		int i = 0;
		auto ix = AmmoList.begin();
		int size = AmmoList.size();
		//Sword&&PitchFork的子弹回收
		if (getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapon_Sword_TAG
			|| getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapn_PitchFork_TAG)
		{

			
			for (; i < size; ix++, i++)
			{
				
				if (AmmoList[i]->getTag() == AllTag::PlayerAmmo_Sword_TAG)
				{
					AmmoList[i]->setPosition(getPlayer()->getPosition() + getPlayer()->getWeapon1()->getPosition());
					double NowTime = clock();

					if (NowTime - AmmoList[i]->start >= getPlayer()->getPlayerAttribute()->getShootSpeed()
						- getPlayer()->getPlayerAttribute()->getShootSpeed_Buff())
					{
						removeChild(AmmoList[i]);
						AmmoList.erase(ix);
						SwordEnd = clock();
						break;
					}
				}

			}
		}
		i = 0;
		ix = AmmoList.begin();
		size = AmmoList.size();
//子弹撞墙回收
			for (; i < size; ix++, i++)
			{

				if (isWall((*ix)->getPositionX(), (*ix)->getPositionY()))
				{
					removeChild(AmmoList[i]);
					AmmoList.erase(ix);
					PistolEnd = clock();
					break;
				}
				


			}
		

	}
	//怪物手枪子弹的撞墙回收 
	if (m_monsterAmmoList.size() != 0)
	{
		int i = 0;
		auto ix = m_monsterAmmoList.begin();
		int size = m_monsterAmmoList.size();
		for (; i < size; ix++, i++)
		{
			if (isWall((*ix)->getPositionX(), (*ix)->getPositionY()))
			{
				removeChild(m_monsterAmmoList[i]);
				m_monsterAmmoList.erase(ix);
				break;
			}

		}
	}
	if (getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapon_Sword_TAG
		|| getPlayer()->getWeapon1()->getTag() == AllTag::PlayerWeapn_PitchFork_TAG)
	{
		if (AmmoList.size())
			getPlayer()->getWeapon1()->getSprite()->setOpacity(0);
		else
			getPlayer()->getWeapon1()->getSprite()->setOpacity(255);
	}

}
//碰撞检测
bool BattleScene1::onContactBegin(PhysicsContact& contact)
{
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();
	if (nodeA && nodeB)
	{
		Ammo* ammo;
		MonsterPistolAmmo* ammo_M;
		MonsterDashAmmo* dash_M;
		Monster* monster;
		Player* player;

		int TagA = nodeA->getTag();
		int TagB = nodeB->getTag();

		if (TagA == AllTag::PlayerAmmo_Pistol_TAG || TagA == AllTag::PlayerAmmo_Sword_TAG)
			ammo = dynamic_cast<Ammo*>(nodeA);
		else if (TagA>AllTag::CloseMonster_TAG&&TagA<AllTag::RemoteMonster_TAG)
			monster = dynamic_cast<Monster*>(nodeA);
		else if (TagA == 0)
			player = dynamic_cast<Player*>(nodeA);
		else if (TagA == AllTag::MonsterAmmo_PistolTAG)
			ammo_M = dynamic_cast<MonsterPistolAmmo*>(nodeA);
		else if (TagA == AllTag::MonsterAmmo_CloseTAG)
			dash_M = dynamic_cast<MonsterDashAmmo*>(nodeA);



		if (TagB == AllTag::PlayerAmmo_Pistol_TAG || TagB == AllTag::PlayerAmmo_Sword_TAG)
			ammo = dynamic_cast<Ammo*>(nodeB);
		else if (TagB > AllTag::CloseMonster_TAG && TagB < AllTag::RemoteMonster_TAG)
			monster = dynamic_cast<Monster*>(nodeB);
		else if (TagB == 0)
			player = dynamic_cast<Player*>(nodeB);
		else if (TagB == AllTag::MonsterAmmo_PistolTAG)
			ammo_M = dynamic_cast<MonsterPistolAmmo*>(nodeB);
		else if (TagB == AllTag::MonsterAmmo_CloseTAG)
			dash_M = dynamic_cast<MonsterDashAmmo*>(nodeB);
		//怪物撞玩家手枪子弹
		if ((TagA > AllTag::CloseMonster_TAG && TagA < AllTag::RemoteMonster_TAG) &&(TagB==AllTag::PlayerAmmo_Pistol_TAG)||
			(TagB > AllTag::CloseMonster_TAG && TagB < AllTag::RemoteMonster_TAG) && (TagA == AllTag::PlayerAmmo_Pistol_TAG))
		{
			int i = 0;
			auto ix = AmmoList.begin();
			int size = AmmoList.size();
			for (; i < size; ix++, i++)
			{
				if (*ix == ammo)
				{
					removeChild(AmmoList[i]);
					AmmoList.erase(ix);
					PistolEnd = clock();
					break;
				}
			}
			monster->takingDamage(getPlayer()->getPlayerAttribute()->getDamage()
			+ getPlayer()->getPlayerAttribute()->getDamage_Buff());
		}
		//怪物撞玩家刀剑子弹
		if ((TagA > AllTag::CloseMonster_TAG && TagA < AllTag::RemoteMonster_TAG) && (TagB == AllTag::PlayerAmmo_Sword_TAG) ||
			(TagB > AllTag::CloseMonster_TAG && TagB < AllTag::RemoteMonster_TAG) && (TagA == AllTag::PlayerAmmo_Sword_TAG))
		{
			int i = 0;
			auto ix = AmmoList.begin();
			int size = AmmoList.size();
			for (; i < size; ix++, i++)
			{
				if (*ix == ammo&&clock()-(*ix)->start>400)
				{
					removeChild(AmmoList[i]);
					AmmoList.erase(ix);
					SwordEnd = clock();	
					break;
				}
			}
			monster->takingDamage(getPlayer()->getPlayerAttribute()->getDamage()
				+ getPlayer()->getPlayerAttribute()->getDamage_Buff());
		}
		//玩家撞怪物Pistol子弹
		if ((TagA==AllTag::Player_TAG&&TagB==AllTag::MonsterAmmo_PistolTAG)||
			(TagB == AllTag::Player_TAG && TagA == AllTag::MonsterAmmo_PistolTAG))
		{
			int i = 0;
			auto ix = m_monsterAmmoList.begin();
			int size = m_monsterAmmoList.size();
			for (; i < size; ix++, i++)
			{
				if (*ix == ammo_M)
				{
					removeChild(m_monsterAmmoList[i]);
					m_monsterAmmoList.erase(ix);
					break;
				}
			}
			getPlayer()->getPlayerAttribute()->takeDamage(1);
		}
		//玩家撞怪物Close子弹
		if ((TagA == AllTag::Player_TAG && TagB == AllTag::MonsterAmmo_CloseTAG) ||
			(TagB == AllTag::Player_TAG && TagA == AllTag::MonsterAmmo_CloseTAG))
		{
			getPlayer()->getPlayerAttribute()->takeDamage(4);
		}

		return 1;
	}
	return 0;
}
void BattleScene1::LandDamageUpdate(float dt)
{
	
	if (isDamagingLand(getPlayer()->getPositionX(), getPlayer()->getPositionY()))
	{
		getPlayer()->getPlayerAttribute()->takeDamage(1);
	}
}
void BattleScene1::PlayerDeatheUpdate(float dt)
{
	int Hp=getPlayer()->getPlayerAttribute()->getHp();
	if (Hp <= 0)
	{	

		Director::getInstance()->replaceScene(StartScene::create());
	}
}
