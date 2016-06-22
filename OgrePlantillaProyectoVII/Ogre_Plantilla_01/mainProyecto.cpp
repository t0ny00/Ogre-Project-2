#include "Ogre\ExampleApplication.h"

#define coinNumber 30


Ogre::SceneNode* nodeObst1[7];
Ogre::SceneNode* nodeObst2[6];
Ogre::SceneNode* nodeObst3[6];


void rotateObstacle(Ogre::SceneNode *nodeObst, float delta){
	nodeObst->yaw(Ogre::Radian(nodeObst->getOrientation().x + delta));
}

class FrameListenerClase : public Ogre::FrameListener{

private:
	Ogre::SceneNode* _nodoF01;
	OIS::InputManager* _man;
	OIS::Keyboard* _key;
	OIS::Mouse* _mouse;
	Ogre::Camera* _cam;

public:
	FrameListenerClase(Ogre::Camera* cam, RenderWindow* win){

		//Configuracion para captura de teclado y mouse 
		size_t windowHnd = 0;
		std::stringstream windowHndStr;
		win->getCustomAttribute("WINDOW",&windowHnd);
		windowHndStr << windowHnd;

		OIS::ParamList pl;
		pl.insert(std::make_pair(std::string("WINDOW"),windowHndStr.str()));

		//eventos
		_man = OIS::InputManager::createInputSystem(pl);
		_key = static_cast<OIS::Keyboard*>(_man->createInputObject(OIS::OISKeyboard,false));
		_mouse = static_cast<OIS::Mouse*>(_man->createInputObject(OIS::OISMouse,false));
		_cam = cam;
		
		
	}


	~FrameListenerClase(){
		_man->destroyInputObject(_key);
		_man->destroyInputObject(_mouse);
		OIS::InputManager::destroyInputSystem(_man);
	}

	bool frameStarted(const Ogre::FrameEvent &evt){
		float cam_speed = 20;
		_key->capture();
		_mouse->capture();

		float movSpeed=10.0f;
		Ogre::Vector3 tmov(0,0,0);
		Ogre::Vector3 tcam(0,0,0);

		//Camara
		if(_key->isKeyDown(OIS::KC_LSHIFT))
			cam_speed += 30;

		if (_key->isKeyDown(OIS::KC_ESCAPE))
			return false;

		if(_key->isKeyDown(OIS::KC_W))
			tcam += Ogre::Vector3(0,0,-cam_speed);
		
		if(_key->isKeyDown(OIS::KC_S))
			tcam += Ogre::Vector3(0,0,cam_speed);

		if(_key->isKeyDown(OIS::KC_A))
			tcam += Ogre::Vector3(-cam_speed,0,0);
		
		if(_key->isKeyDown(OIS::KC_D))
			tcam += Ogre::Vector3(cam_speed,0,0);

		
		

		

		//camara control
		float rotX = _mouse->getMouseState().X.rel * evt.timeSinceLastFrame*-1;
		float rotY = _mouse->getMouseState().Y.rel * evt.timeSinceLastFrame*-1;
		_cam->yaw(Ogre::Radian(rotX));
		_cam->pitch(Ogre::Radian(rotY));
		_cam->moveRelative(tcam*movSpeed*evt.timeSinceLastFrame);

		
		for (int i = 0; i < (sizeof(nodeObst1) / sizeof(nodeObst1[0])); i++){
			rotateObstacle(nodeObst1[i],4 * evt.timeSinceLastFrame);
		};

		for (int i = 0; i < (sizeof(nodeObst2) / sizeof(nodeObst2[0])); i++){
			rotateObstacle(nodeObst2[i],4 * evt.timeSinceLastFrame);
		};

		for (int i = 0; i < (sizeof(nodeObst2) / sizeof(nodeObst2[0])); i++){
			rotateObstacle(nodeObst3[i],4 * evt.timeSinceLastFrame);
		};

		return true;
	}

};

class Example1 : public ExampleApplication
{

public:

	Ogre::FrameListener* FrameListener01;

	void createCamera() {

		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(0,100,-200);
		mCamera->lookAt(0,0,100);
		mCamera->setNearClipDistance(1);

	}

	void createFrameListener(){
		FrameListener01 = new FrameListenerClase(mCamera,mWindow);
		mRoot->addFrameListener(FrameListener01);

	}

	void createScene()
	{

		mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		mSceneMgr->setSkyDome(true, "StarSky", 5, 8);

		Ogre::Light* LuzPuntual01 = mSceneMgr->createLight("Luz01");
		LuzPuntual01->setType(Ogre::Light::LT_DIRECTIONAL);
		LuzPuntual01->setDiffuseColour(1.0,1.0,1.0);
		LuzPuntual01->setDirection(Ogre::Vector3( 1, -1, -1 ));
		
		Ogre::Light* LuzPuntual02 = mSceneMgr->createLight("Luz02");
		LuzPuntual02->setType(Ogre::Light::LT_DIRECTIONAL);
		LuzPuntual02->setDiffuseColour(1.0,1.0,1.0);
		LuzPuntual02->setDirection(Ogre::Vector3( -1, -1, -1 ));
		LuzPuntual01->setCastShadows(false);
		LuzPuntual02->setCastShadows(false);

		//Rueda
		Ogre::SceneNode* _nodeRueda01 = mSceneMgr->createSceneNode("Rueda01");
		mSceneMgr->getRootSceneNode()->addChild(_nodeRueda01);
			
		Ogre::Entity* _entRueda01 = mSceneMgr->createEntity("entRueda01", "ruedaDetallada.mesh");
		_nodeRueda01->translate(-5.77,3.517,-9.462);
		_entRueda01->setMaterialName("shRueda02");
		_nodeRueda01->attachObject(_entRueda01);

		//Chasis
		Ogre::SceneNode* _nodeChasis01 = mSceneMgr->createSceneNode("Chasis01");
		mSceneMgr->getRootSceneNode()->addChild(_nodeChasis01);
			
		Ogre::Entity* _entChasis01 = mSceneMgr->createEntity("entChasis01", "chasisCarro.mesh");
		_entChasis01->setMaterialName("shCarro01");
		_nodeChasis01->attachObject(_entChasis01);


		//BordePista
		Ogre::SceneNode* _nodeBPista = mSceneMgr->createSceneNode("BordePista");
		mSceneMgr->getRootSceneNode()->addChild(_nodeBPista);
				
		Ogre::Entity* _entBPista = mSceneMgr->createEntity("BordePista01", "bordePista.mesh");
		_nodeBPista->attachObject(_entBPista);


		//PisoObstaculo
		Ogre::SceneNode* _nodePObstaculo = mSceneMgr->createSceneNode("PistaObstaculo");
		mSceneMgr->getRootSceneNode()->addChild(_nodePObstaculo);
				
		Ogre::Entity* _entPObstaculo = mSceneMgr->createEntity("PistaObstaculo", "pisoObstaculo01.mesh");
		_nodePObstaculo->attachObject(_entPObstaculo);

		//PisoNOObstaculo
		Ogre::SceneNode* _nodePNObstaculo = mSceneMgr->createSceneNode("PistaNoObstaculo");
		mSceneMgr->getRootSceneNode()->addChild(_nodePNObstaculo);
				
		Ogre::Entity* _entPNOObstaculo = mSceneMgr->createEntity("PistaNoObstaculo", "pisoNoObstaculo01.mesh");
		_nodePNObstaculo->attachObject(_entPNOObstaculo);


		//PosterInicioFinal
		Ogre::SceneNode* _nodePoster = mSceneMgr->createSceneNode("PosterInicioFinal");
		mSceneMgr->getRootSceneNode()->addChild(_nodePoster);
			
		Ogre::Entity* _entPoster = mSceneMgr->createEntity("PosterInicioFinal", "posterInicioFinal.mesh");
		_nodePoster->attachObject(_entPoster);

				
		//BanderaInicial
		Ogre::SceneNode* _nodeBInicial = mSceneMgr->createSceneNode("BanderaInicial");
		mSceneMgr->getRootSceneNode()->addChild(_nodeBInicial);
				
		Ogre::Entity* _entBanderaI = mSceneMgr->createEntity("BanderaInicial", "banderaInicial.mesh");				
		_entBanderaI->setMaterialName("lambert1");				
		_nodeBInicial->attachObject(_entBanderaI);


		//BanderaFinal
		Ogre::SceneNode* _nodeBFinal = mSceneMgr->createSceneNode("BanderaFinal");
		mSceneMgr->getRootSceneNode()->addChild(_nodeBFinal);
		
		Ogre::Entity* _entBanderaF = mSceneMgr->createEntity("BanderaFinal", "banderaFinal.mesh");
		_entBanderaF->setMaterialName("lambert1");
		_nodeBFinal->attachObject(_entBanderaF);



		//Monedas
		Ogre::Entity* entityCoin[coinNumber];
		Ogre::SceneNode* nodeCoin[coinNumber];
		for (int i = 0; i < (sizeof(entityCoin) / sizeof(entityCoin[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityCoin[i] = mSceneMgr->createEntity("Coin " + number, "sphere.mesh");
			entityCoin[i]->setMaterialName("Gold_3");
			nodeCoin[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("NodeCoin " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			nodeCoin[i]->setPosition(0 , 7, (i+2) * 300);
			nodeCoin[i]->setScale(0.05,0.05,0.05);
			nodeCoin[i]->attachObject(entityCoin[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		nodeCoin[2]->translate(65,0,0);
		nodeCoin[3]->translate(55,0,0);
		nodeCoin[4]->translate(-20,0,0);
		nodeCoin[5]->translate(20,0,0);
		nodeCoin[6]->translate(-70,0,0);
		nodeCoin[7]->translate(25,0,0);
		nodeCoin[9]->translate(-20,0,0);
		nodeCoin[10]->translate(-20,0,0);
		nodeCoin[11]->translate(20,0,0);
		nodeCoin[12]->translate(20,0,0);
		nodeCoin[13]->translate(0,0,0);
		nodeCoin[14]->translate(-20,0,0);
		nodeCoin[15]->translate(40,0,0);
		nodeCoin[16]->translate(60,0,0);
		nodeCoin[17]->translate(25,0,0);
		nodeCoin[18]->translate(-45,0,0);
		nodeCoin[19]->translate(-30,0,0);
		nodeCoin[20]->translate(0,20,0);
		nodeCoin[21]->translate(70,10,25);
		nodeCoin[22]->translate(-30,-10,25);
		nodeCoin[23]->translate(-70,-20,25);
		nodeCoin[24]->translate(-15,15,25);
		nodeCoin[25]->translate(0,-20,25);
		nodeCoin[26]->translate(50,0,25);
		nodeCoin[27]->translate(15,30,25);
		nodeCoin[28]->translate(-55,25,25);
		nodeCoin[29]->translate(0,0,25);

		//Decoracion

		//Decor 1
		Ogre::SceneNode *nodeDecor01 = mSceneMgr->createSceneNode();
		mSceneMgr->getRootSceneNode()->addChild(nodeDecor01);

		Ogre::Entity *subEntityDecor101 = mSceneMgr->createEntity("subEntityDecor101", "usb_dodecaedro.mesh");
		subEntityDecor101->setMaterialName("Grey");
		Ogre::SceneNode *subNodeDecor101 = mSceneMgr->createSceneNode();
		subNodeDecor101->attachObject(subEntityDecor101);
		subNodeDecor101->setPosition(0,-5,0);
		subNodeDecor101->setScale(1,2.5,1);
	  
	  

		Ogre::Entity *subEntityDecor102 = mSceneMgr->createEntity("subEntityDecor102", "usb_torus.mesh");
		subEntityDecor102->setMaterialName("DarkBlue");
		Ogre::SceneNode *subNodeDecor102 = mSceneMgr->createSceneNode();
		subNodeDecor102->attachObject(subEntityDecor102);
		subNodeDecor102->setPosition(0,20,0);
		subNodeDecor102->setScale(2.5,2.5,2.5);
		subNodeDecor102->pitch(Ogre::Degree(90));

		Ogre::Entity *subEntityDecor103 = mSceneMgr->createEntity("subEntityDecor103", "sphere.mesh");
		subEntityDecor103->setMaterialName("LightBlue");
		Ogre::SceneNode *subNodeDecor103 = mSceneMgr->createSceneNode();
		subNodeDecor103->attachObject(subEntityDecor103);
		subNodeDecor103->setPosition(0,20,0);
		subNodeDecor103->setScale(0.06,0.06,0.06);

		Ogre::Entity *subEntityDecor104 = mSceneMgr->createEntity("subEntityDecor104", "usb_cubomod01.mesh");
		subEntityDecor104->setMaterialName("DarkGrey");
		Ogre::SceneNode *subNodeDecor104 = mSceneMgr->createSceneNode();
		subNodeDecor104->attachObject(subEntityDecor104);
		subNodeDecor104->setPosition(0,-10,0);
		subNodeDecor104->setScale(3,3,3);

		nodeDecor01->addChild(subNodeDecor101);
		nodeDecor01->addChild(subNodeDecor102);
		nodeDecor01->addChild(subNodeDecor103);
		nodeDecor01->addChild(subNodeDecor104);
	 
		nodeDecor01->setPosition(60,50,2970);
		nodeDecor01->setScale(2,2,2);
		

		//Decor 2
		Ogre::SceneNode *nodeDecor02 = mSceneMgr->createSceneNode();
		mSceneMgr->getRootSceneNode()->addChild(nodeDecor02);

		Ogre::Entity *subEntityDecor201 = mSceneMgr->createEntity("subEntityDecor201", "usb_dodecaedro.mesh");
		subEntityDecor201->setMaterialName("Grey");
		Ogre::SceneNode *subNodeDecor201 = mSceneMgr->createSceneNode();
		subNodeDecor201->attachObject(subEntityDecor201);
		subNodeDecor201->setPosition(0,-5,0);
		subNodeDecor201->setScale(1,2.5,1);
	  
	  
		
		Ogre::Entity *subEntityDecor202 = mSceneMgr->createEntity("subEntityDecor202", "usb_torus.mesh");
		subEntityDecor202->setMaterialName("DarkBlue");
		Ogre::SceneNode *subNodeDecor202 = mSceneMgr->createSceneNode();
		subNodeDecor202->attachObject(subEntityDecor202);
		subNodeDecor202->setPosition(0,20,0);
		subNodeDecor202->setScale(2.5,2.5,2.5);
		subNodeDecor202->pitch(Ogre::Degree(90));

		Ogre::Entity *subEntityDecor203 = mSceneMgr->createEntity("subEntityDecor203", "sphere.mesh");
		subEntityDecor203->setMaterialName("LightBlue");
		Ogre::SceneNode *subNodeDecor203 = mSceneMgr->createSceneNode();
		subNodeDecor203->attachObject(subEntityDecor203);
		subNodeDecor203->setPosition(0,20,0);
		subNodeDecor203->setScale(0.06,0.06,0.06);

		Ogre::Entity *subEntityDecor204 = mSceneMgr->createEntity("subEntityDecor204", "usb_cubomod01.mesh");
		subEntityDecor204->setMaterialName("DarkGrey");
		Ogre::SceneNode *subNodeDecor204 = mSceneMgr->createSceneNode();
		subNodeDecor204->attachObject(subEntityDecor204);
		subNodeDecor204->setPosition(0,-10,0);
		subNodeDecor204->setScale(3,3,3);

		nodeDecor02->addChild(subNodeDecor201);
		nodeDecor02->addChild(subNodeDecor202);
		nodeDecor02->addChild(subNodeDecor203);
		nodeDecor02->addChild(subNodeDecor204);
	 
		nodeDecor02->setPosition(-60,50,2970);
		nodeDecor02->setScale(2,2,2);
		
		//Decor 3
		Ogre::Entity* entityDecor3[8];
		Ogre::SceneNode* nodeDecor3[8];
		int temp = 1;
		for (int i = 0; i < (sizeof(entityDecor3) / sizeof(entityDecor3[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityDecor3[i] = mSceneMgr->createEntity("Decor30 " + number, "athene.mesh");
			entityDecor3[i]->setMaterialName("lambert1");
			nodeDecor3[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeDecor3 " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			temp *= -1;
			nodeDecor3[i]->rotate(Quaternion (Degree(-temp*90), Vector3::UNIT_Y));
			nodeDecor3[i]->setPosition(temp*190 ,40, i * 250 + 500);
			nodeDecor3[i]->setScale(0.5,0.5,0.5);
			nodeDecor3[i]->attachObject(entityDecor3[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		//Decor 4
		Ogre::SceneNode* nodeDecor04 = mSceneMgr->createSceneNode("Decor04");
		mSceneMgr->getRootSceneNode()->addChild(nodeDecor04);
			
		Ogre::Entity* entDecor04 = mSceneMgr->createEntity("entDecor04", "razor.mesh");
		nodeDecor04->translate(-150,200,9000);
		nodeDecor04->yaw(Ogre::Degree(157));
		nodeDecor04->pitch(Ogre::Degree(67));
		nodeDecor04->scale(0.5,0.5,0.5);
		nodeDecor04->attachObject(entDecor04);

		//Decor 5
		Ogre::SceneNode* nodeDecor05 = mSceneMgr->createSceneNode("Decor05");
		mSceneMgr->getRootSceneNode()->addChild(nodeDecor05);
			
		Ogre::Entity* entDecor05 = mSceneMgr->createEntity("entDecor05", "razor.mesh");
		nodeDecor05->translate(255,-215,8000);
		nodeDecor05->yaw(Ogre::Degree(157));
		nodeDecor05->pitch(Ogre::Degree(240));
		nodeDecor05->scale(0.5,0.5,0.5);
		nodeDecor05->attachObject(entDecor05);


		//Decor 6
		Ogre::Entity* entityDecor6[5];
		Ogre::SceneNode* nodeDecor6[5];
		temp = 1;
		for (int i = 0; i < (sizeof(entityDecor6) / sizeof(entityDecor6[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityDecor6[i] = mSceneMgr->createEntity("Decor60 " + number, "athene.mesh");
			entityDecor6[i]->setMaterialName("lambert1");
			nodeDecor6[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeDecor6 " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			temp *= -1;
			nodeDecor6[i]->rotate(Quaternion (Degree(-temp*90), Vector3::UNIT_Y));
			nodeDecor6[i]->setPosition(temp*190 ,40, i * 100 + 5700);
			nodeDecor6[i]->setScale(0.5,0.5,0.5);
			nodeDecor6[i]->attachObject(entityDecor6[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		//Obstacles 1

		Ogre::Entity* entityObst1[7];
		
		temp = 1;
		for (int i = 0; i < (sizeof(entityObst1) / sizeof(entityObst1[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityObst1[i] = mSceneMgr->createEntity("Obst1 " + number, "spine.mesh");
			entityObst1[i]->setMaterialName("lambert1");
			nodeObst1[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeObst1 " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			temp *= -1;
			//nodeObst1[i]->rotate(Quaternion (Degree(-temp*90), Vector3::UNIT_Y));
			nodeObst1[i]->setPosition(0 , 0 , i * 50 + 500);
			nodeObst1[i]->setScale(0.5,0.5,0.5);
			nodeObst1[i]->attachObject(entityObst1[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		nodeObst1[1]->translate(-50,0,0);
		nodeObst1[2]->translate(45,0,0);
		//nodeObst1[3]->translate(0,0,0);
		nodeObst1[4]->translate(-35,0,0);
		nodeObst1[5]->translate(35,0,0);
		nodeObst1[6]->translate(20,0,50);

		//Obstacles 2 (Moving)

		Ogre::Entity* entityObst2[6];
		
		temp = 1;
		for (int i = 0; i < (sizeof(entityObst2) / sizeof(entityObst2[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityObst2[i] = mSceneMgr->createEntity("Obst2 " + number, "spine.mesh");
			entityObst2[i]->setMaterialName("lambert1");
			nodeObst2[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeObst2 " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			temp *= -1;
			//nodeObst2[i]->rotate(Quaternion (Degree(-temp*90), Vector3::UNIT_Y));
			nodeObst2[i]->setPosition(0 , 0 , i * 125 + 1400);
			nodeObst2[i]->setScale(0.5,0.5,0.5);
			nodeObst2[i]->attachObject(entityObst2[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		//Obstacles 3

		Ogre::Entity* entityObst3[6];
		
		temp = 1;
		for (int i = 0; i < (sizeof(entityObst3) / sizeof(entityObst3[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityObst3[i] = mSceneMgr->createEntity("Obst3 " + number, "spine.mesh");
			entityObst3[i]->setMaterialName("lambert1");
			nodeObst3[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeObst3 " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			temp *= -1;
			//nodeObst3[i]->rotate(Quaternion (Degree(-temp*90), Vector3::UNIT_Y));
			nodeObst3[i]->setPosition(0 , 0 , i * 150 + 5600);
			nodeObst3[i]->setScale(0.5,0.5,0.5);
			nodeObst3[i]->attachObject(entityObst3[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		nodeObst3[1]->translate(-50,0,0);
		nodeObst3[2]->translate(45,0,0);
		//nodeObst3[3]->translate(0,0,0);
		nodeObst3[4]->translate(-35,0,0);
		nodeObst3[5]->translate(35,0,0);
		







	}

};


int main (void)
{

	Example1 app;
	app.go();
	return 0; 
}