#include "Ogre\ExampleApplication.h"
#include "OgreTextAreaOverlayElement.h" 
#include "OgreFontManager.h"

#define coinNumber 30

float radiusCar = 12.5;
float radiusCoin = 0.5;
float radiusObstacles = 0.7;
float radiusWall = 5;
bool isColliding = false;
TextAreaOverlayElement* scoreText;
Overlay* overlay;
int score = 0;
int coinsCollected[coinNumber];

// car 12.5
// coin 0.5
//obs 0.7

Ogre::SceneNode* _nodeChasis01;
Ogre::SceneNode* nodeObst1[7];
Ogre::SceneNode* nodeObst2[8];
Ogre::SceneNode* nodeObst3[6];
Ogre::SceneNode* nodeRock01[8];
Ogre::SceneNode* nodeRock02[8];
Ogre::SceneNode* nodeRock03[8];
Ogre::SceneNode* nodeRock04[8];
Ogre::SceneNode* nodeCoin[coinNumber];

int movingObstDir[8];
int movingObstSpeed[8];
Vector3 wallLeftCoord[119];
Vector3 wallRightCoord[119];
Vector3 wallRightSecondCoord[119];
Vector3 wallLeftSecondCoord[119];

Ogre::SceneNode* nodeParticle;
Ogre::ParticleSystem* partSystem;

//Ogre::AnimationState* animationState;
//Ogre::Animation* animationObstSpin[19];

void rotateObstacle(Ogre::SceneNode *nodeObst, float delta){
	nodeObst->yaw(Ogre::Radian(nodeObst->getOrientation().x + delta));
}

void rotateRock(Ogre::SceneNode *nodeObst, float delta){
	Quaternion temp = nodeObst->getOrientation();
	nodeObst->rotate(Quaternion (Degree(delta), Vector3::UNIT_Y));
	nodeObst->rotate(Quaternion (Degree(delta), Vector3::UNIT_X));
}

void moveObstacle(Ogre::SceneNode *nodeObst, float speed, int &dir){
	if (dir == 0) nodeObst->translate(speed ,0,0);
	else nodeObst->translate(-speed ,0,0);
	if (nodeObst->getPosition().x > 200 && dir == 0) dir = 1;
	else if (nodeObst->getPosition().x < -200 && dir == 1) dir = 0;


}

bool collision(Ogre::SceneNode *nodeCar, Ogre::SceneNode *nodeObj, float radius1, float radius2 ){
	Ogre::Vector3 relPos = nodeCar->getPosition() - nodeObj->getPosition();;
	float dist = relPos.x * relPos.x + relPos.y * relPos.y + relPos.z * relPos.z;
	float minDist = radius1 + radius2;
	return dist <= minDist * minDist;
}

bool collisionWall(Ogre::SceneNode *nodeCar, Vector3 sphere, float radius1, float radius2 ){
	Ogre::Vector3 relPos = nodeCar->getPosition() - sphere;
	float dist = relPos.x * relPos.x + relPos.y * relPos.y + relPos.z * relPos.z;
	float minDist = radius1 + radius2;
	return dist <= minDist * minDist;
}



class FrameListenerClase : public Ogre::FrameListener{

private:
	Ogre::SceneNode* _nodoF01;
	OIS::InputManager* _man;
	OIS::Keyboard* _key;
	OIS::Mouse* _mouse;
	Ogre::Camera* _cam;
	float time;

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
		
		time = 0;
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
			cam_speed += 300;

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

		/*if(_key->isKeyDown(OIS::KC_V))
			r1 += 0.5;

		if(_key->isKeyDown(OIS::KC_B))
			r1 -= 0.5;

		if(_key->isKeyDown(OIS::KC_N))
			r2 += 0.5;

		if(_key->isKeyDown(OIS::KC_M))
			r2 -= 0.5;*/

		if(_key->isKeyDown(OIS::KC_C))
			system("cls");

		//Car test
		bool hit = false;
		Vector3 mov(0,0,0);

		if(_key->isKeyDown(OIS::KC_I)){
			for (int i = 0; i < (sizeof(wallLeftCoord) / sizeof(wallLeftCoord[0])); i++) // Loop through the entities
			{
				if(collisionWall(_nodeChasis01,wallLeftCoord[i],radiusCar,radiusWall)) hit = true;
				if(collisionWall(_nodeChasis01,wallRightCoord[i],radiusCar,radiusWall)) hit = true;
			}
			if (!hit)mov += Ogre::Vector3(0,0,100);
			hit = false;
		}
		if(_key->isKeyDown(OIS::KC_J)){
			for (int i = 0; i < (sizeof(wallLeftCoord) / sizeof(wallLeftCoord[0])); i++) // Loop through the entities
			{
				if(collisionWall(_nodeChasis01,wallLeftCoord[i],radiusCar,radiusWall)) hit = true;
				if(collisionWall(_nodeChasis01,wallLeftSecondCoord[i],radiusCar,radiusWall)) hit = true;
			}
			if (!hit) mov += Ogre::Vector3(100,0,0);
			hit = false;
		}
			

		if(_key->isKeyDown(OIS::KC_K)){
			for (int i = 0; i < (sizeof(wallRightCoord) / sizeof(wallRightCoord[0])); i++) // Loop through the entities
			{
				if(collisionWall(_nodeChasis01,wallLeftSecondCoord[i],radiusCar,radiusWall)) hit = true;
				if(collisionWall(_nodeChasis01,wallRightSecondCoord[i],radiusCar,radiusWall)) hit = true;

			}
			if (!hit) mov += Ogre::Vector3(0,0,-100);
			hit = false;
			
			
		}
			
		
		if(_key->isKeyDown(OIS::KC_L)){
			for (int i = 0; i < (sizeof(wallRightCoord) / sizeof(wallRightCoord[0])); i++) // Loop through the entities
			{
				if(collisionWall(_nodeChasis01,wallRightCoord[i],radiusCar,radiusWall)) hit = true;
				if(collisionWall(_nodeChasis01,wallRightSecondCoord[i],radiusCar,radiusWall)) hit = true;

			}
			if (!hit) mov += Ogre::Vector3(-100,0,0);
			hit = false;
		}
			
		

		_nodeChasis01->translate(mov*evt.timeSinceLastFrame);
		

		//camara control
		float rotX = _mouse->getMouseState().X.rel * evt.timeSinceLastFrame*-1;
		float rotY = _mouse->getMouseState().Y.rel * evt.timeSinceLastFrame*-1;
		_cam->yaw(Ogre::Radian(rotX));
		_cam->pitch(Ogre::Radian(rotY));
		_cam->moveRelative(tcam*movSpeed*evt.timeSinceLastFrame);

		//animationState->addTime(evt.timeSinceLastFrame);
		
		for (int i = 0; i < (sizeof(nodeObst1) / sizeof(nodeObst1[0])); i++){
			rotateObstacle(nodeObst1[i],4 * evt.timeSinceLastFrame);
			if (collision(_nodeChasis01,nodeObst1[i],radiusCar,radiusObstacles) && !isColliding){
				isColliding = true;
				//Do collision stuff
			}
			else isColliding = false;
		};

		for (int i = 0; i < (sizeof(nodeObst2) / sizeof(nodeObst2[0])); i++){
			rotateObstacle(nodeObst2[i],4 * evt.timeSinceLastFrame);
			moveObstacle(nodeObst2[i], movingObstSpeed[i]  * evt.timeSinceLastFrame, movingObstDir[i]);
			if (collision(_nodeChasis01,nodeObst2[i],radiusCar,radiusObstacles) && !isColliding){
				isColliding = true;
				//Do collision stuff
			}
			else isColliding = false;
		};
		for (int i = 0; i < (sizeof(nodeObst3) / sizeof(nodeObst3[0])); i++){
			rotateObstacle(nodeObst3[i],4 * evt.timeSinceLastFrame);
			if (collision(_nodeChasis01,nodeObst3[i],radiusCar,radiusObstacles) && !isColliding){
				isColliding = true;
				//Do collision stuff
			}
			else isColliding = false;
		};

		for (int i = 0; i < (sizeof(nodeRock01) / sizeof(nodeRock01[0])); i++){
			rotateRock(nodeRock01[i],15 * evt.timeSinceLastFrame);
			rotateRock(nodeRock02[i],17 * evt.timeSinceLastFrame);
			rotateRock(nodeRock03[i],19 * evt.timeSinceLastFrame);
			rotateRock(nodeRock04[i],32 * evt.timeSinceLastFrame);
			if ((collision(_nodeChasis01,nodeRock01[i],radiusCar,radiusObstacles) ||
				collision(_nodeChasis01,nodeRock02[i],radiusCar,radiusObstacles)  ||
				collision(_nodeChasis01,nodeRock03[i],radiusCar,radiusObstacles)  ||
				collision(_nodeChasis01,nodeRock04[i],radiusCar,radiusObstacles)) && !isColliding){
				isColliding = true;
				//Do collision stuff
			}
			else isColliding = false;
		};

		Ogre::String scoreString;
		for (int i = 0; i < (sizeof(nodeCoin) / sizeof(nodeCoin[0])); i++){
			if (collision(_nodeChasis01,nodeCoin[i],radiusCar,radiusCoin) && coinsCollected[i] == 0) {
				nodeParticle->setPosition(nodeCoin[i]->getPosition());
				partSystem->setEmitting(true);
				nodeCoin[i]->setVisible(false);
				coinsCollected[i] = 1;
				time = 0;
				score += 10;
				scoreString = Ogre::StringConverter::toString(score); 
				scoreText->setCaption("Score: " + scoreString);
				overlay->show();
			}
		};

		time += evt.timeSinceLastFrame;
		if (time > 0.5) partSystem->setEmitting(false);
		
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

		OverlayManager& overlayManager = OverlayManager::getSingleton();
 
		// Create a panel
		OverlayContainer* panel = static_cast<OverlayContainer*>(
			overlayManager.createOverlayElement("Panel", "PanelName"));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setPosition(10, 10);
		panel->setDimensions(100, 100);
		//panel->setMaterialName("MaterialName"); // Optional background material
 
		// Create a text area
		scoreText = static_cast<TextAreaOverlayElement*>(
			overlayManager.createOverlayElement("TextArea", "TextAreaName"));
		scoreText->setMetricsMode(Ogre::GMM_PIXELS);
		scoreText->setPosition(0, 0);
		scoreText->setDimensions(100, 100);
		scoreText->setCaption("Score: 0");
		scoreText->setCharHeight(35);
		scoreText->setColourBottom(ColourValue(1, 1, 1));
		scoreText->setColourTop(ColourValue(0.5, 0.7, 0.5));
 
		// Create an overlay, and add the panel
		overlay = overlayManager.create("OverlayName");
		overlay->add2D(panel);
 
		// Add the text area to the panel
		panel->addChild(scoreText);
 
		// Show the overlay
		overlay->show();
		
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

		

		/*Ogre::Entity* entEsferaLuz01 = mSceneMgr->createEntity("EsferaLuz01","sphere.mesh");
		Ogre::SceneNode* nodeEsfera01 = mSceneMgr->createSceneNode("nodeEsferaLuz01");
		mSceneMgr->getRootSceneNode()->addChild(nodeEsfera01);
		nodeEsfera01->attachObject(entEsferaLuz01);
		Ogre::SceneNode* nodeLuzP01 = mSceneMgr->createSceneNode("nodeLuzPoint01");*/

		//Ogre::Light* LuzPuntual03 = mSceneMgr->createLight("Luz03");
		//LuzPuntual03->setType(Ogre::Light::LT_POINT);
		//LuzPuntual03->setPosition(0,25,0);
		//LuzPuntual03->setDiffuseColour(1.0, 1.0, 1.0);      //color the light orange 
		//LuzPuntual03->setAttenuation(3250, 1.0, 0.0014, 0.000007);
		//LuzPuntual03->setCastShadows(false);

		//Ogre::Light* LuzPuntual04 = mSceneMgr->createLight("Luz04");
		//LuzPuntual04->setType(Ogre::Light::LT_POINT);
		//LuzPuntual04->setPosition(0,25,3100);
		//LuzPuntual04->setDiffuseColour(1.0, 1.0, 1.0);      //color the light orange 
		//LuzPuntual04->setAttenuation( 500, 1.0f, 4.5/500, 75.0f/(500*500) );
		//LuzPuntual04->setCastShadows(false);

		//Ogre::Light* LuzPuntual05 = mSceneMgr->createLight("Luz05");
		//LuzPuntual05->setType(Ogre::Light::LT_POINT);
		//LuzPuntual05->setPosition(0,25,4500);
		//LuzPuntual05->setDiffuseColour(1.0, 1.0, 1.0);      //color the light orange 
		//LuzPuntual05->setAttenuation(500, 1.0f, 2.5/500, 50.0f/(500*500));
		//LuzPuntual05->setCastShadows(false);

		/*nodeLuzP01->attachObject(LuzPuntual03);
		nodeEsfera01->addChild(nodeLuzP01);
		nodeEsfera01->setScale(0.05,0.05,0.05);
		nodeEsfera01->setPosition(0,25,3000);*/
		
		//Ogre::Light* LuzPuntual04 = mSceneMgr->createLight("Luz04");
		//LuzPuntual04->setType(Ogre::Light::LT_SPOTLIGHT);
		//LuzPuntual04->setPosition(0,10,0);
		//LuzPuntual04->setDiffuseColour(1, 1, 0);      //color the light orange 
		//LuzPuntual04->setSpecularColour(1, 1, 0);    //yellow highlights
		//LuzPuntual04->setDirection(Ogre::Vector3( 0, -1, 0 ));
		//LuzPuntual04->setSpotlightInnerAngle(Ogre::Degree(10.f));
		//LuzPuntual04->setSpotlightOuterAngle(Ogre::Degree(40.f));
		//LuzPuntual04->setSpotlightFalloff(1.0f);
		//LuzPuntual04->setCastShadows(false);
		////LuzPuntual04->setAttenuation(600, 0.0, 0.001, 0.0001);

		// Particula
		nodeParticle = mSceneMgr->getRootSceneNode()->createChildSceneNode("test");
		partSystem = mSceneMgr->createParticleSystem("Smoke","PurpleFountain");
		nodeParticle->attachObject(partSystem);
		partSystem->setEmitting(false);

		// Colision pared
		for (int i = 0; i < (sizeof(wallLeftCoord) / sizeof(wallLeftCoord[0])); i++) // Loop through the entities
		{
			wallLeftCoord[i].x = 200 - i * radiusWall/4;
			wallLeftCoord[i].y = 5;
			wallLeftCoord[i].z = 2350 + i * radiusWall;
		}

		for (int i = 0; i < (sizeof(wallRightCoord) / sizeof(wallRightCoord[0])); i++) // Loop through the entities
		{
			wallRightCoord[i].x = -200 + i * radiusWall/4;
			wallRightCoord[i].y = 5;
			wallRightCoord[i].z = 2350 + i * radiusWall;
		}
		for (int i = 0; i < (sizeof(wallRightSecondCoord) / sizeof(wallRightSecondCoord[0])); i++) // Loop through the entities
		{
			wallRightSecondCoord[i].x = -56 - i * radiusWall/3.5;
			wallRightSecondCoord[i].y = 5;
			wallRightSecondCoord[i].z = 4940 + i * radiusWall;
		}
		for (int i = 0; i < (sizeof(wallLeftSecondCoord) / sizeof(wallLeftSecondCoord[0])); i++) // Loop through the entities
		{
			wallLeftSecondCoord[i].x = 56 + i * radiusWall/3.5;
			wallLeftSecondCoord[i].y = 5;
			wallLeftSecondCoord[i].z = 4940 + i * radiusWall;
		}


		//Rueda
		Ogre::SceneNode* _nodeRueda01 = mSceneMgr->createSceneNode("Rueda01");
		mSceneMgr->getRootSceneNode()->addChild(_nodeRueda01);
			
		Ogre::Entity* _entRueda01 = mSceneMgr->createEntity("entRueda01", "ruedaDetallada.mesh");
		_nodeRueda01->translate(-5.77,3.517,-9.462);
		_entRueda01->setMaterialName("shRueda02");
		_nodeRueda01->attachObject(_entRueda01);

		//Chasis
		_nodeChasis01 = mSceneMgr->createSceneNode("Chasis01");
		mSceneMgr->getRootSceneNode()->addChild(_nodeChasis01);
			
		Ogre::Entity* _entChasis01 = mSceneMgr->createEntity("entChasis01", "chasisCarro.mesh");
		_entChasis01->setMaterialName("shCarro01");
		_nodeChasis01->attachObject(_entChasis01);
		

		//BordePista
		Ogre::SceneNode* _nodeBPista = mSceneMgr->createSceneNode("BordePista");
		mSceneMgr->getRootSceneNode()->addChild(_nodeBPista);
				
		Ogre::Entity* _entBPista = mSceneMgr->createEntity("BordePista01", "bordePista.mesh");
		_nodeBPista->attachObject(_entBPista);
		_entBPista->setMaterialName("pared");

		//PisoObstaculo
		Ogre::SceneNode* _nodePObstaculo = mSceneMgr->createSceneNode("PistaObstaculo");
		mSceneMgr->getRootSceneNode()->addChild(_nodePObstaculo);
				
		Ogre::Entity* _entPObstaculo = mSceneMgr->createEntity("PistaObstaculo", "pisoObstaculo01.mesh");
		_nodePObstaculo->attachObject(_entPObstaculo);
		_entPObstaculo->setMaterialName("sueloObs");

		//PisoNOObstaculo
		Ogre::SceneNode* _nodePNObstaculo = mSceneMgr->createSceneNode("PistaNoObstaculo");
		mSceneMgr->getRootSceneNode()->addChild(_nodePNObstaculo);
				
		Ogre::Entity* _entPNOObstaculo = mSceneMgr->createEntity("PistaNoObstaculo", "pisoNoObstaculo01.mesh");
		_nodePNObstaculo->attachObject(_entPNOObstaculo);
		_entPNOObstaculo->setMaterialName("suelo");

		//PosterInicioFinal
		Ogre::SceneNode* _nodePoster = mSceneMgr->createSceneNode("PosterInicioFinal");
		mSceneMgr->getRootSceneNode()->addChild(_nodePoster);
			
		Ogre::Entity* _entPoster = mSceneMgr->createEntity("PosterInicioFinal", "posterInicioFinal.mesh");
		_nodePoster->attachObject(_entPoster);
		_entPoster->setMaterialName("LightBlue2");
				
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
		_entBanderaF->setMaterialName("final");
		_nodeBFinal->attachObject(_entBanderaF);



		//Monedas
		Ogre::Entity* entityCoin[coinNumber];

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
		nodeCoin[21]->translate(95,25,25);
		nodeCoin[22]->translate(-50,-30,25);
		nodeCoin[23]->translate(-100,-40,25);
		nodeCoin[24]->translate(-15,15,25);
		nodeCoin[25]->translate(0,-50,25);
		nodeCoin[26]->translate(70,0,25);
		nodeCoin[27]->translate(15,80,25);
		nodeCoin[28]->translate(-55,25,25);
		nodeCoin[29]->translate(0,0,25);

		//Decoracion

		//Decor 1
		Ogre::SceneNode *nodeDecor01 = mSceneMgr->createSceneNode();
		mSceneMgr->getRootSceneNode()->addChild(nodeDecor01);

		Ogre::Entity *subEntityDecor101 = mSceneMgr->createEntity("subEntityDecor101", "usb_dodecaedro.mesh");
		subEntityDecor101->setMaterialName("Grey2");
		Ogre::SceneNode *subNodeDecor101 = mSceneMgr->createSceneNode();
		subNodeDecor101->attachObject(subEntityDecor101);
		subNodeDecor101->setPosition(0,-5,0);
		subNodeDecor101->setScale(1,2.5,1);
	  
	  

		Ogre::Entity *subEntityDecor102 = mSceneMgr->createEntity("subEntityDecor102", "usb_torus.mesh");
		subEntityDecor102->setMaterialName("DarkBlue2");
		Ogre::SceneNode *subNodeDecor102 = mSceneMgr->createSceneNode();
		subNodeDecor102->attachObject(subEntityDecor102);
		subNodeDecor102->setPosition(0,20,0);
		subNodeDecor102->setScale(2.5,2.5,2.5);
		subNodeDecor102->pitch(Ogre::Degree(90));

		Ogre::Entity *subEntityDecor103 = mSceneMgr->createEntity("subEntityDecor103", "sphere.mesh");
		subEntityDecor103->setMaterialName("LightBlue2");
		Ogre::SceneNode *subNodeDecor103 = mSceneMgr->createSceneNode();
		subNodeDecor103->attachObject(subEntityDecor103);
		subNodeDecor103->setPosition(0,20,0);
		subNodeDecor103->setScale(0.06,0.06,0.06);

		Ogre::Entity *subEntityDecor104 = mSceneMgr->createEntity("subEntityDecor104", "usb_cubomod01.mesh");
		subEntityDecor104->setMaterialName("DarkGrey2");
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
		subEntityDecor201->setMaterialName("Grey2");
		Ogre::SceneNode *subNodeDecor201 = mSceneMgr->createSceneNode();
		subNodeDecor201->attachObject(subEntityDecor201);
		subNodeDecor201->setPosition(0,-5,0);
		subNodeDecor201->setScale(1,2.5,1);
	  
	  
		
		Ogre::Entity *subEntityDecor202 = mSceneMgr->createEntity("subEntityDecor202", "usb_torus.mesh");
		subEntityDecor202->setMaterialName("DarkBlue2");
		Ogre::SceneNode *subNodeDecor202 = mSceneMgr->createSceneNode();
		subNodeDecor202->attachObject(subEntityDecor202);
		subNodeDecor202->setPosition(0,20,0);
		subNodeDecor202->setScale(2.5,2.5,2.5);
		subNodeDecor202->pitch(Ogre::Degree(90));

		Ogre::Entity *subEntityDecor203 = mSceneMgr->createEntity("subEntityDecor203", "sphere.mesh");
		subEntityDecor203->setMaterialName("LightBlue2");
		Ogre::SceneNode *subNodeDecor203 = mSceneMgr->createSceneNode();
		subNodeDecor203->attachObject(subEntityDecor203);
		subNodeDecor203->setPosition(0,20,0);
		subNodeDecor203->setScale(0.06,0.06,0.06);

		Ogre::Entity *subEntityDecor204 = mSceneMgr->createEntity("subEntityDecor204", "usb_cubomod01.mesh");
		subEntityDecor204->setMaterialName("DarkGrey2");
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
			entityObst1[i]->setMaterialName("Brown2");
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

		Ogre::Entity* entityObst2[8];
		
		temp = 1;
		for (int i = 0; i < (sizeof(entityObst2) / sizeof(entityObst2[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityObst2[i] = mSceneMgr->createEntity("Obst2 " + number, "spine.mesh");
			entityObst2[i]->setMaterialName("Brown2");
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
			entityObst3[i]->setMaterialName("Brown2");
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
		
		for (int i = 0; i < 8; i++){
			movingObstDir[i] = rand() % 2;
			movingObstSpeed[i] = rand() % 20 + 115;
		};


		//Rocks 01

		Ogre::Entity* entityRock01[8];
		
		temp = 1;
		for (int i = 0; i < (sizeof(entityRock01) / sizeof(entityRock01[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityRock01[i] = mSceneMgr->createEntity("Rock01 " + number, "roca01.mesh");
			entityRock01[i]->setMaterialName("matRoca01");
			nodeRock01[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeRock01 " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			temp *= -1;
			//nodeObst3[i]->rotate(Quaternion (Degree(-temp*90), Vector3::UNIT_Y));
			nodeRock01[i]->setPosition(rand() % 250 - 150 , rand() % 250 - 150 , i * 250 + 7000);
			//nodeRock01[i]->setPosition(0,0 , i * 250 + 7000);

			//nodeRock01[i]->setScale(0.5,0.5,0.5);
			nodeRock01[i]->attachObject(entityRock01[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		//Rocks 02

		Ogre::Entity* entityRock02[8];
		
		temp = 1;
		for (int i = 0; i < (sizeof(entityRock02) / sizeof(entityRock02[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityRock02[i] = mSceneMgr->createEntity("Rock02 " + number, "roca02.mesh");
			entityRock02[i]->setMaterialName("matRoca02");
			nodeRock02[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeRock02 " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			temp *= -1;
			//nodeObst3[i]->rotate(Quaternion (Degree(-temp*90), Vector3::UNIT_Y));
			nodeRock02[i]->setPosition(rand() % 250 - 150 , rand() % 250 - 150 , i * 350 + 6800);
			//nodeRock02[i]->setPosition(0,0 , i * 350 + 6800);

			//nodeRock01[i]->setScale(0.5,0.5,0.5);
			nodeRock02[i]->attachObject(entityRock02[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		//Rocks 03

		Ogre::Entity* entityRock03[8];
		
		temp = 1;
		for (int i = 0; i < (sizeof(entityRock03) / sizeof(entityRock03[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityRock03[i] = mSceneMgr->createEntity("Rock03 " + number, "roca03.mesh");
			entityRock03[i]->setMaterialName("matRoca03");
			nodeRock03[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeRock03 " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			temp *= -1;
			//nodeObst3[i]->rotate(Quaternion (Degree(-temp*90), Vector3::UNIT_Y));
			nodeRock03[i]->setPosition(rand() % 250 - 150 , rand() % 250 - 150 , i * 315 + 7500);
			//nodeRock03[i]->setPosition(0,0 , i * 315 + 7500);

			//nodeRock01[i]->setScale(0.5,0.5,0.5);
			nodeRock03[i]->attachObject(entityRock03[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		//Rocks 04

		Ogre::Entity* entityRock04[8];
		
		temp = 1;
		for (int i = 0; i < (sizeof(entityRock04) / sizeof(entityRock04[0])); i++) // Loop through the entities
		{
			// Since array elements start from 0, we add 1, so the entity and node names start from 1 :)
			Ogre::String number = Ogre::StringConverter::toString(i + 1); 
 
			// Add the current element number to the entity/scene node name to avoid confusion
			entityRock04[i] = mSceneMgr->createEntity("Rock04 " + number, "roca04.mesh");
			entityRock04[i]->setMaterialName("matRoca04");
			nodeRock04[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeRock04 " + number);
 
			// Distance the nodes from each other, so they aren't at the same place, and then attach them
			temp *= -1;
			//nodeObst3[i]->rotate(Quaternion (Degree(-temp*90), Vector3::UNIT_Y));
			nodeRock04[i]->setPosition(rand() % 250 - 150 , rand() % 250 - 150 , i * 375 + 7700);
			//nodeRock04[i]->setPosition(0,0 , i * 375 + 7700);

			//nodeRock01[i]->setScale(0.5,0.5,0.5);
			nodeRock04[i]->attachObject(entityRock04[i]);
			// Let us know how many entities we have on screen, completely unnecessary
			printf("Created Entity No. %i \n", i);
		}

		
		////Animacion Torreta 4
		//float duration2 = 4.0;
		//Ogre::Animation* animationTurret401 = mSceneMgr->createAnimation("AnimTurret401",duration2);
		//animationTurret401->setRotationInterpolationMode(Animation::RIM_LINEAR);
		//Ogre::NodeAnimationTrack* Turret401Track = animationTurret401->createNodeTrack(0,nodeObst1[1]);
		//Ogre::TransformKeyFrame* keyTurret04;

		//keyTurret04 = Turret401Track->createNodeKeyFrame(0.0);
		//keyTurret04->setRotation(Quaternion (Degree(0), Vector3::UNIT_Y));
		//keyTurret04->setScale(nodeObst1[1]->getScale());
		//keyTurret04->setTranslate(nodeObst1[1]->getPosition());

		//keyTurret04 = Turret401Track->createNodeKeyFrame(1.0);
		//keyTurret04->setRotation(Quaternion (Degree(90), Vector3::UNIT_Y));
		//keyTurret04->setScale(nodeObst1[1]->getScale());
		//keyTurret04->setTranslate(nodeObst1[1]->getPosition());

		//keyTurret04 = Turret401Track->createNodeKeyFrame(2.0);
		//keyTurret04->setRotation(Quaternion (Degree(180), Vector3::UNIT_Y));
		//keyTurret04->setScale(nodeObst1[1]->getScale());
		//keyTurret04->setTranslate(nodeObst1[1]->getPosition());

		//keyTurret04 = Turret401Track->createNodeKeyFrame(3.0);
		//keyTurret04->setRotation(Quaternion (Degree(270), Vector3::UNIT_Y));
		//keyTurret04->setScale(nodeObst1[1]->getScale());
		//keyTurret04->setTranslate(nodeObst1[1]->getPosition());

		//keyTurret04 = Turret401Track->createNodeKeyFrame(4.0);
		//keyTurret04->setRotation(Quaternion (Degree(0), Vector3::UNIT_Y));
		//keyTurret04->setScale(nodeObst1[1]->getScale());
		//keyTurret04->setTranslate(nodeObst1[1]->getPosition());

		//animationState = mSceneMgr->createAnimationState("AnimTurret401");
		//animationState->setEnabled(true);
		//animationState->setLoop(true);

	}

};


int main (void)
{

	Example1 app;
	app.go();
	return 0; 
}