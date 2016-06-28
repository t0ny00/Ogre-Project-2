#include "Ogre\ExampleApplication.h"

#define coinNumber 30

float radiusCar = 12.5;
float radiusCoin = 0.5;
float radiusObstacles = 0.7;
bool isColliding = false;

// car 12.5
// coin 0.5
//obs 0.7

Ogre::AnimationState* animationStateExpRightWing;
Ogre::AnimationState* animationStateExpLeftWing;
Ogre::AnimationState* animationStateRollWheel1;
Ogre::AnimationState* animationStateRollWheel2;
Ogre::AnimationState* animationStateRollWheel3;
Ogre::AnimationState* animationStateRollWheel4;
Ogre::AnimationState* animationStateTurbine;

Ogre::SceneNode* _nodeChasis01;
Ogre::SceneNode* nodeObst1[7];
Ogre::SceneNode* nodeObst2[8];
Ogre::SceneNode* nodeObst3[6];
Ogre::SceneNode* nodeRock01[8];
Ogre::SceneNode* nodeRock02[8];
Ogre::SceneNode* nodeRock03[8];
Ogre::SceneNode* nodeRock04[8];
Ogre::SceneNode* nodeCoin[coinNumber];

Ogre::SceneNode* nodePlayer;
Ogre::SceneNode* rotatorRueda01;
Ogre::SceneNode* rotatorRueda02;
Ogre::SceneNode* rotatorRueda03;
Ogre::SceneNode* rotatorRueda04;
Ogre::SceneNode* nodeRueda01;
Ogre::SceneNode* nodeRueda02;
Ogre::SceneNode* nodeRueda03;
Ogre::SceneNode* nodeRueda04;

int movingObstDir[8];
int movingObstSpeed[8];

float inertia_time = 0.0;
float inertia_speed = 0.0;
float turned_right_time = 0.0;
float turned_left_time = 0.0;
int turnLeftCounter = 21;
int unturnLeftCounter = 21;
bool turnedLeft = false;

int turnRightCounter = 21;
int unturnRightCounter = 21;
bool turnedRight = false;
int colliding = 1;
int orientation = 1;
float bounce_back_time = 0.0;

float rolled_right_time = 0.0;
float rolled_left_time = 0.0;
int rollLeftCounter = 21;
int unrollLeftCounter = 21;
bool rolledLeft = false;
int rollRightCounter = 21;
int unrollRightCounter = 21;
bool rolledRight = false;


bool spaceship_mode = false;

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

void rotateWheel(Ogre::SceneNode *nodeWheel, float delta){
	nodeWheel->pitch(Ogre::Radian(nodeWheel->getOrientation().y + delta));
}

void rotateChasis(Ogre::SceneNode *nodeChasis, float delta){
	nodeChasis->roll(Ogre::Radian(nodeChasis->getOrientation().x + delta));
}


void turnRightWheels(Ogre::SceneNode *nodeWheel1,Ogre::SceneNode *nodeWheel2, float delta){
	nodeWheel1->yaw(Ogre::Radian(nodeWheel1->getOrientation().x + delta));
	nodeWheel2->yaw(Ogre::Radian(nodeWheel2->getOrientation().x + delta));
}


void turnLeftWheels(Ogre::SceneNode *nodeWheel1,Ogre::SceneNode *nodeWheel2, float delta){
	
	nodeWheel1->yaw(Ogre::Radian(nodeWheel1->getOrientation().x + delta));
	nodeWheel2->yaw(Ogre::Radian(nodeWheel2->getOrientation().x + delta));
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
		Vector3 mov(0,0,0);
		if(_key->isKeyDown(OIS::KC_I)){
			orientation = 1;
			inertia_time = 2.0;
			inertia_speed = 1.0;
		}
		
		if(_key->isKeyDown(OIS::KC_J)){
			if(inertia_speed > 0) mov += Ogre::Vector3(colliding*80*inertia_speed,0,0);
			if (spaceship_mode){
				if (!rolledLeft){
					rolled_left_time = 0.2;
					rolledLeft = true;
					rollLeftCounter = 0;
					unrollLeftCounter = 0;
				} 
				rolled_left_time = 0.2;
			} else {
				if (!turnedLeft){
					turned_left_time = 0.2;
					turnedLeft = true;
					turnLeftCounter = 0;
					unturnLeftCounter = 0;
				} 
				turned_left_time = 0.2;
			}
		}

		if(_key->isKeyDown(OIS::KC_K)){
			inertia_time = 2.0;
			inertia_speed = 1.0;
			orientation = -1;
		}
		
		if(_key->isKeyDown(OIS::KC_L)){
			if(inertia_speed > 0) mov += Ogre::Vector3(colliding*-80*inertia_speed,0,0);
			if(spaceship_mode){
				if (!rolledRight){
					rolled_right_time = 0.2;
					rolledRight = true;
					rollRightCounter = 0;
					unrollRightCounter = 0;
				}
				rolled_right_time = 0.2;
			} else {
				if (!turnedRight){
					turned_right_time = 0.2;
					turnedRight = true;
					turnRightCounter = 0;
					unturnRightCounter = 0;
				}
				turned_right_time = 0.2;
			}
		}

		if(nodePlayer->getPosition().z >= 653){
			spaceship_mode = true;
			animationStateExpRightWing->setEnabled(true);
			animationStateExpLeftWing->setEnabled(true);
			animationStateRollWheel1->setEnabled(true);
			animationStateRollWheel2->setEnabled(true);
			animationStateRollWheel3->setEnabled(true);
			animationStateRollWheel4->setEnabled(true);
			animationStateTurbine->setEnabled(true);
			
		}
		

		
		

		//camara control
		float rotX = _mouse->getMouseState().X.rel * evt.timeSinceLastFrame*-1;
		float rotY = _mouse->getMouseState().Y.rel * evt.timeSinceLastFrame*-1;
		_cam->yaw(Ogre::Radian(rotX));
		_cam->pitch(Ogre::Radian(rotY));
		_cam->moveRelative(tcam*movSpeed*evt.timeSinceLastFrame);

		//animationState->addTime(evt.timeSinceLastFrame);

		

		
		for (int i = 0; i < (sizeof(nodeObst1) / sizeof(nodeObst1[0])); i++){
			rotateObstacle(nodeObst1[i],4 * evt.timeSinceLastFrame);
			if (collision(nodePlayer,nodeObst1[i],radiusCar,radiusObstacles) && !isColliding){
				isColliding = true;
				//Do collision stuff
				inertia_speed += 0.2;
				bounce_back_time = 0.3;
				colliding = -1;
			}
			else isColliding = false;
		};

		for (int i = 0; i < (sizeof(nodeObst2) / sizeof(nodeObst2[0])); i++){
			rotateObstacle(nodeObst2[i],4 * evt.timeSinceLastFrame);
			moveObstacle(nodeObst2[i], movingObstSpeed[i]  * evt.timeSinceLastFrame, movingObstDir[i]);
			if (collision(nodePlayer,nodeObst2[i],radiusCar,radiusObstacles) && !isColliding){
				isColliding = true;
				//Do collision stuff
				inertia_speed += 0.2;
				bounce_back_time = 0.3;
				colliding = -1;
			}
			else isColliding = false;
		};
		for (int i = 0; i < (sizeof(nodeObst3) / sizeof(nodeObst3[0])); i++){
			rotateObstacle(nodeObst3[i],4 * evt.timeSinceLastFrame);
			if (collision(nodePlayer,nodeObst3[i],radiusCar,radiusObstacles) && !isColliding){
				isColliding = true;
				//Do collision stuff
				inertia_speed += 0.2;
				bounce_back_time = 0.3;
				colliding = -1;
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

		for (int i = 0; i < (sizeof(nodeCoin) / sizeof(nodeCoin[0])); i++){
			if (collision(_nodeChasis01,nodeCoin[i],radiusCar,radiusCoin)) {
				//Do coin collision stuff
				nodeCoin[i]->setVisible(false);
			}
		};

		if (bounce_back_time > 0.0) bounce_back_time -= evt.timeSinceLastFrame;
		else colliding = 1;


		if(inertia_speed > 0.0){
			mov += Ogre::Vector3(0,0,orientation*colliding*100*inertia_speed);
			rotateWheel(nodeRueda01,-4 * orientation * inertia_speed * evt.timeSinceLastFrame);
			rotateWheel(nodeRueda02,4 * orientation * inertia_speed * evt.timeSinceLastFrame);
			rotateWheel(nodeRueda03,4 * orientation * inertia_speed * evt.timeSinceLastFrame);
			rotateWheel(nodeRueda04,-4 * orientation * inertia_speed * evt.timeSinceLastFrame);
			inertia_time -= evt.timeSinceLastFrame;
			inertia_speed -= 0.05;
		}

		if(spaceship_mode){
			if (rolledLeft){
				if (rollLeftCounter < 10) {
					rotateChasis(_nodeChasis01, -3 * 0.015);
					rollLeftCounter++;
				} else {
					if (rolled_left_time > 0.0){
						rolled_left_time -= evt.timeSinceLastFrame;
					} else {
						if(unrollLeftCounter < 10){
							rotateChasis(_nodeChasis01, 3 * 0.015);
							unrollLeftCounter++;
						} else {
							rolledLeft = false;
						}
					}
				}
			}
		} else {
			if (turnedLeft){
				if (turnLeftCounter < 10) {
					turnLeftWheels(rotatorRueda01,rotatorRueda02, 4 * 0.015);
					turnLeftCounter++;
				} else {
					if (turned_left_time > 0.0){
						turned_left_time -= evt.timeSinceLastFrame;
					} else {
						if(unturnLeftCounter < 10){
							turnLeftWheels(rotatorRueda01,rotatorRueda02, -4 * 0.015);
							unturnLeftCounter++;
						} else {
							turnedLeft = false;
						}
					}
				}
			}
		}

		if(spaceship_mode){
			if (rolledRight){
				if (rollRightCounter < 10) {
					rotateChasis(_nodeChasis01, 3 * 0.015);
					rollRightCounter++;
				} else {
					if (rolled_right_time > 0.0){
						rolled_right_time -= evt.timeSinceLastFrame;
					} else {
						if(unrollRightCounter < 10){
							rotateChasis(_nodeChasis01, - 3 * 0.015);
							unrollRightCounter++;
						} else {
							rolledRight = false;
						}
					}
				}
			}
		} else {
			if (turnedRight){
				if (turnRightCounter < 10) {
					turnRightWheels(rotatorRueda01,rotatorRueda02, -4 * 0.015);
					turnRightCounter++;
				} else {
					if (turned_right_time > 0.0){
						turned_right_time -= evt.timeSinceLastFrame;
					} else {
						if(unturnRightCounter < 10){
							turnRightWheels(rotatorRueda01,rotatorRueda02, 4 * 0.015);
							unturnRightCounter++;
						} else {
							turnedRight = false;
						}
					}
				}
			}
		}

		nodePlayer->translate(mov*evt.timeSinceLastFrame);
		if (animationStateExpRightWing->getEnabled()) animationStateExpRightWing->addTime(evt.timeSinceLastFrame);
		if (animationStateExpLeftWing->getEnabled()) animationStateExpLeftWing->addTime(evt.timeSinceLastFrame);
		if (animationStateRollWheel1->getEnabled()) animationStateRollWheel1->addTime(evt.timeSinceLastFrame);
		if (animationStateRollWheel2->getEnabled()) animationStateRollWheel2->addTime(evt.timeSinceLastFrame);
		if (animationStateRollWheel3->getEnabled()) animationStateRollWheel3->addTime(evt.timeSinceLastFrame);
		if (animationStateRollWheel4->getEnabled()) animationStateRollWheel4->addTime(evt.timeSinceLastFrame);
		if (animationStateTurbine->getEnabled()) animationStateTurbine->addTime(evt.timeSinceLastFrame);

		printf("\nX is %f\nY is %f\nZ is %f\n",nodePlayer->getPosition().x,nodePlayer->getPosition().y,nodePlayer->getPosition().z);



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

	ManualObject* generateWing1(String name, float g_x, float g_y, float g_z, float r_y, float f_h, float f_w1, float f_w2, float b_h, float b_w1,float b_w2, float d){

	 ManualObject* manual = mSceneMgr->createManualObject(name);
	  manual->begin("DarkGrey", RenderOperation::OT_TRIANGLE_STRIP);
	  

	  //      position(  R ,    G    , B  )

	  manual->position( g_x+ b_w1, g_y+ -b_h, g_z+ d);        //1 
	  manual->position( g_x+ b_w2, g_y+ b_h, g_z+ d);         //2 

	  manual->position( g_x+ -f_w1, r_y+ g_y+ -f_h, g_z+ -d); //3 
	  manual->position( g_x+ -f_w2,  r_y+ g_y+ f_h, g_z+ -d); //4 
	  manual->position( g_x+ f_w1,  r_y+ g_y+ -f_h, g_z+ -d); //5 
	  manual->position( g_x+ f_w2, r_y+ g_y+ f_h, g_z+ -d);   //6 

	  manual->position( g_x+ b_w1, g_y+ -b_h, g_z+ d);        //1 
	  manual->position( g_x+ b_w2, g_y+ b_h, g_z+ d);         //2 
	  manual->position( g_x+ -f_w2,  r_y+ g_y+ f_h, g_z+ -d); //4 
	  manual->position( g_x+ f_w2, r_y+ g_y+ f_h, g_z+ -d);   //6 
	  manual->position( g_x+ f_w1,  r_y+ g_y+ -f_h, g_z+ -d); //5 
	  manual->position( g_x+ b_w1, g_y+ -b_h, g_z+ d);        //1 
	  manual->position( g_x+ -f_w1, r_y+ g_y+ -f_h, g_z+ -d); //3 

 
	  manual->end();

	  return manual;
	}

	ManualObject* generateWing2(String name, float g_x, float g_y, float g_z, float r_y, float f_h, float f_w1, float f_w2, float b_h, float b_w1,float b_w2, float d){

	 ManualObject* manual = mSceneMgr->createManualObject(name);
	  manual->begin("DarkGrey", RenderOperation::OT_TRIANGLE_STRIP);
	  
	  
	  //      position(  R ,    G    , B  )

	  manual->position( g_x+ -b_w1, g_y+ -b_h, g_z+ d);       //1
	  manual->position( g_x+ -b_w2, g_y+  b_h, g_z+ d);       //2 

	  manual->position( g_x+ -f_w1, r_y+ g_y+ -f_h, g_z+ -d); //3 
	  manual->position( g_x+ -f_w2,  r_y+ g_y+ f_h, g_z+ -d); //4 
	  manual->position( g_x+ f_w1,  r_y+ g_y+ -f_h, g_z+ -d); //5 
	  manual->position( g_x+ f_w2, r_y+ g_y+ f_h, g_z+ -d);   //6 

	  manual->position( g_x+ -b_w1, g_y+ -b_h, g_z+ d);       //1
	  manual->position( g_x+ -b_w2, g_y+  b_h, g_z+ d);       //2 
	  manual->position( g_x+ -f_w2,  r_y+ g_y+ f_h, g_z+ -d); //4 
	  manual->position( g_x+ f_w2, r_y+ g_y+ f_h, g_z+ -d);   //6 
	  manual->position( g_x+ f_w1,  r_y+ g_y+ -f_h, g_z+ -d); //5 
	  manual->position( g_x+ -b_w1, g_y+ -b_h, g_z+ d);       //1
	  manual->position( g_x+ -f_w1, r_y+ g_y+ -f_h, g_z+ -d); //3 

 
	  manual->end();
	  return manual;
	}

	void createScene()
	{

		radiusCar = 12.5;
		

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

		Ogre::SceneNode* test = mSceneMgr->getRootSceneNode()->createChildSceneNode("test");
		Ogre::ParticleSystem* partSystem = mSceneMgr->createParticleSystem("Smoke","PurpleFountain");
		test->attachObject(partSystem);

		///*Ogre::Entity* entEsferaLuz01 = mSceneMgr->createEntity("EsferaLuz01","sphere.mesh");
		//Ogre::SceneNode* nodeEsfera01 = mSceneMgr->createSceneNode("nodeEsferaLuz01");
		//mSceneMgr->getRootSceneNode()->addChild(nodeEsfera01);
		//nodeEsfera01->attachObject(entEsferaLuz01);
		//Ogre::SceneNode* nodeLuzP01 = mSceneMgr->createSceneNode("nodeLuzPoint01");*/

		//Ogre::Light* LuzPuntual03 = mSceneMgr->createLight("Luz03");
		//LuzPuntual03->setType(Ogre::Light::LT_POINT);
		//LuzPuntual03->setPosition(0,25,5000);
		//LuzPuntual03->setDiffuseColour(1.0, 0, 0.0);      //color the light orange 
		//LuzPuntual03->setSpecularColour(1.0, 0.0, 0.0);    //yellow highlights
		//LuzPuntual03->setAttenuation(600, 0.0, 0.001, 0.0001);
		//LuzPuntual03->setCastShadows(false);

		///*nodeLuzP01->attachObject(LuzPuntual03);
		//nodeEsfera01->addChild(nodeLuzP01);
		//nodeEsfera01->setScale(0.05,0.05,0.05);
		//nodeEsfera01->setPosition(0,25,3000);*/

		//Ogre::Light* LuzPuntual04 = mSceneMgr->createLight("Luz04");
		//LuzPuntual04->setType(Ogre::Light::LT_SPOTLIGHT);
		//LuzPuntual04->setPosition(0,10,0);
		//LuzPuntual04->setDiffuseColour(1, 1, 0);      //color the light orange 
		//LuzPuntual04->setSpecularColour(1, 1, 0);    //yellow highlights
		//LuzPuntual04->setDirection(Ogre::Vector3( 0, -1, 0 ));
		//LuzPuntual04->setSpotlightInnerAngle(Ogre::Degree(5.f));
		//LuzPuntual04->setSpotlightOuterAngle(Ogre::Degree(20.f));
		//LuzPuntual04->setSpotlightFalloff(0.0f);
		//LuzPuntual04->setCastShadows(false);
		////LuzPuntual04->setAttenuation(600, 0.0, 0.001, 0.0001);

		nodePlayer = mSceneMgr->createSceneNode("Player");
		mSceneMgr->getRootSceneNode()->addChild(nodePlayer);

		
		//Chasis
		_nodeChasis01 = mSceneMgr->createSceneNode("Chasis01");
		nodePlayer->addChild(_nodeChasis01);
			
		Ogre::Entity* _entChasis01 = mSceneMgr->createEntity("entChasis01", "chasisCarro.mesh");
		_entChasis01->setMaterialName("shCarro01");
		_nodeChasis01->attachObject(_entChasis01);

		//turbine
		Ogre::SceneNode* turbine = mSceneMgr->createSceneNode("turbine");
		_nodeChasis01->addChild(turbine);

		Ogre::Entity* entTurbine = mSceneMgr->createEntity("entTurbine", "Barrel.mesh");\
		entTurbine->setMaterialName("DarkGrey");
		turbine->attachObject(entTurbine);
		turbine->rotate(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(1,0,0)));
		turbine->setScale(1.0,0.0,1.0);
		turbine->translate(0.0,6.0,0.0);

		


		//Wings
		Ogre::SceneNode* wing1 = mSceneMgr->createSceneNode("wing1");
		_nodeChasis01->addChild(wing1);
			
		ManualObject* wing1object;
		wing1object = generateWing1("wing1obj",0.0,0.0,0.0,0.0,0.1,4.0,4.0,0.1,4.0,4.0,2.0);
		wing1->attachObject(wing1object);
		wing1->setScale(0.0,3.0,2.0);
		wing1->translate(0.0,4.0,0.0);

		Ogre::SceneNode* wing2 = mSceneMgr->createSceneNode("wing2");
		_nodeChasis01->addChild(wing2);
		
		ManualObject* wing2object;
		wing2object = generateWing2("wing2obj",0.0,0.0,0.0,0.0,0.1,4.0,4.0,0.1,4.0,4.0,2.0);
		wing2->attachObject(wing2object);
		wing2->setScale(0.0,3.0,2.0);
		wing2->translate(0.0,4.0,0.0);

		//Rotators
		rotatorRueda01 = mSceneMgr->createSceneNode("rotatorRueda01");
		rotatorRueda02 = mSceneMgr->createSceneNode("rotatorRueda02");
		rotatorRueda03 = mSceneMgr->createSceneNode("rotatorRueda03");
		rotatorRueda04 = mSceneMgr->createSceneNode("rotatorRueda04");
		_nodeChasis01->addChild(rotatorRueda01);
		_nodeChasis01->addChild(rotatorRueda02);
		_nodeChasis01->addChild(rotatorRueda03);
		_nodeChasis01->addChild(rotatorRueda04);
		rotatorRueda01->translate(-5.77,3.517,9.462);
		rotatorRueda02->translate(5.77,3.517,9.462);
		rotatorRueda02->rotate(Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3(0,1,0)));
		rotatorRueda03->translate(5.77,3.517,-9.462);
		rotatorRueda03->rotate(Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3(0,1,0)));
		rotatorRueda04->translate(-5.77,3.517,-9.462);

		//Rueda 1
		nodeRueda01 = mSceneMgr->createSceneNode("Rueda01");
		rotatorRueda03->addChild(nodeRueda01);
			
		Ogre::Entity* _entRueda01 = mSceneMgr->createEntity("entRueda01", "ruedaDetallada.mesh");\
		_entRueda01->setMaterialName("shRueda02");
		nodeRueda01->attachObject(_entRueda01);

		//Rueda 2

		nodeRueda02 = mSceneMgr->createSceneNode("Rueda02");
		rotatorRueda04->addChild(nodeRueda02);
			
		Ogre::Entity* _entRueda02 = mSceneMgr->createEntity("entRueda02", "ruedaDetallada.mesh");
		_entRueda02->setMaterialName("shRueda02");
		nodeRueda02->attachObject(_entRueda02);

		//Rueda 3

		nodeRueda03 = mSceneMgr->createSceneNode("Rueda03");
		rotatorRueda01->addChild(nodeRueda03);
			
		Ogre::Entity* _entRueda03 = mSceneMgr->createEntity("entRueda03", "ruedaDetallada.mesh");
		//nodeRueda03->translate(-5.77,3.517,9.462);
		_entRueda03->setMaterialName("shRueda02");
		nodeRueda03->attachObject(_entRueda03);

		//Rueda 4

		nodeRueda04 = mSceneMgr->createSceneNode("Rueda04");
		rotatorRueda02->addChild(nodeRueda04);
		//nodeRueda04->yaw(Radian(180));
			
		Ogre::Entity* _entRueda04 = mSceneMgr->createEntity("entRueda04", "ruedaDetallada.mesh");
		//nodeRueda04->translate(7.95,3.517,9.462);
		_entRueda04->setMaterialName("shRueda02");
		nodeRueda04->attachObject(_entRueda04);

		/*extendido translate(0.0,6.0,-15.0) setScale(1.0,1.0,1.0)*/

		//Spaceship transformation animation
		float transform_anim_duration = 2.0;
		Ogre::Animation* animationExpandRightWing = mSceneMgr->createAnimation("expandRightWingAnimation",transform_anim_duration);
		Ogre::Animation* animationExpandLeftWing = mSceneMgr->createAnimation("expandLeftWingAnimation",transform_anim_duration);
		Ogre::Animation* animationRollWheel1 = mSceneMgr->createAnimation("rollWheel1Animation",transform_anim_duration);
		Ogre::Animation* animationRollWheel2 = mSceneMgr->createAnimation("rollWheel2Animation",transform_anim_duration);
		Ogre::Animation* animationRollWheel3 = mSceneMgr->createAnimation("rollWheel3Animation",transform_anim_duration);
		Ogre::Animation* animationRollWheel4 = mSceneMgr->createAnimation("rollWheel4Animation",transform_anim_duration);
		Ogre::Animation* animationTurbine = mSceneMgr->createAnimation("turbineAnimation",transform_anim_duration);
		animationExpandRightWing->setInterpolationMode(Animation::IM_SPLINE);
		animationExpandLeftWing->setInterpolationMode(Animation::IM_SPLINE);
		animationRollWheel1->setInterpolationMode(Animation::IM_SPLINE);
		animationRollWheel2->setInterpolationMode(Animation::IM_SPLINE);
		animationRollWheel3->setInterpolationMode(Animation::IM_SPLINE);
		animationRollWheel4->setInterpolationMode(Animation::IM_SPLINE);
		animationTurbine->setInterpolationMode(Animation::IM_SPLINE);
		Ogre::NodeAnimationTrack* expRightWingTrack = animationExpandRightWing->createNodeTrack(0,wing1);
		Ogre::NodeAnimationTrack* expLeftWingTrack = animationExpandLeftWing->createNodeTrack(0,wing2);
		Ogre::NodeAnimationTrack* rollWheel1Track = animationRollWheel1->createNodeTrack(0,nodeRueda01);
		Ogre::NodeAnimationTrack* rollWheel2Track = animationRollWheel2->createNodeTrack(0,nodeRueda02);
		Ogre::NodeAnimationTrack* rollWheel3Track = animationRollWheel3->createNodeTrack(0,nodeRueda03);
		Ogre::NodeAnimationTrack* rollWheel4Track = animationRollWheel4->createNodeTrack(0,nodeRueda04);
		Ogre::NodeAnimationTrack* turbineTrack = animationTurbine->createNodeTrack(0,turbine);

		Ogre::TransformKeyFrame* keyExpand1;
		Ogre::TransformKeyFrame* keyExpand2;
		Ogre::TransformKeyFrame* keyRoll1;
		Ogre::TransformKeyFrame* keyRoll2;
		Ogre::TransformKeyFrame* keyRoll3;
		Ogre::TransformKeyFrame* keyRoll4;
		Ogre::TransformKeyFrame* keyTurbine;

		keyExpand1 = expRightWingTrack->createNodeKeyFrame(0.0);
		keyExpand1->setScale(Ogre::Vector3(0.0,3.0,2.0));
		keyExpand1->setTranslate(Ogre::Vector3(0.0,4.0,0.0));
		keyExpand2 = expLeftWingTrack->createNodeKeyFrame(0.0);
		keyExpand2->setScale(Ogre::Vector3(0.0,3.0,2.0));
		keyExpand2->setTranslate(Ogre::Vector3(0.0,4.0,0.0));
		keyRoll1 = rollWheel1Track->createNodeKeyFrame(0.0);
		keyRoll1->setRotation(Ogre::Quaternion(Ogre::Degree(0), Ogre::Vector3(0,0,1)));
		keyRoll2 = rollWheel2Track->createNodeKeyFrame(0.0);
		keyRoll2->setRotation(Ogre::Quaternion(Ogre::Degree(0), Ogre::Vector3(0,0,1)));
		keyRoll3 = rollWheel3Track->createNodeKeyFrame(0.0);
		keyRoll3->setRotation(Ogre::Quaternion(Ogre::Degree(0), Ogre::Vector3(0,0,1)));
		keyRoll4 = rollWheel4Track->createNodeKeyFrame(0.0);
		keyRoll4->setRotation(Ogre::Quaternion(Ogre::Degree(0), Ogre::Vector3(0,0,1)));
		keyTurbine = turbineTrack->createNodeKeyFrame(0.0);
		keyTurbine->setScale(Ogre::Vector3(1.0,0.0,1.0));
		keyTurbine->setTranslate(Ogre::Vector3(0.0,6.0,0.0));
		keyTurbine->setRotation(Ogre::Quaternion(Ogre::Degree(0), Ogre::Vector3(1,0,0)));


		keyExpand1 = expRightWingTrack->createNodeKeyFrame(2.0);
		keyExpand1->setScale(Ogre::Vector3(2.0,3.0,2.0));
		keyExpand1->setTranslate(Ogre::Vector3(-10.0,4.0,0.0));
		keyExpand2 = expLeftWingTrack->createNodeKeyFrame(2.0);
		keyExpand2->setScale(Ogre::Vector3(2.0,3.0,2.0));
		keyExpand2->setTranslate(Ogre::Vector3(10.0,4.0,0.0));
		keyRoll1 = rollWheel1Track->createNodeKeyFrame(2.0);
		keyRoll1->setRotation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0,0,1)));
		keyRoll2 = rollWheel2Track->createNodeKeyFrame(2.0);
		keyRoll2->setRotation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0,0,1)));
		keyRoll3 = rollWheel3Track->createNodeKeyFrame(2.0);
		keyRoll3->setRotation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0,0,1)));
		keyRoll4 = rollWheel4Track->createNodeKeyFrame(2.0);
		keyRoll4->setRotation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(0,0,1)));
		keyTurbine = turbineTrack->createNodeKeyFrame(2.0);
		keyTurbine->setScale(Ogre::Vector3(1.0,1.0,1.0));
		keyTurbine->setTranslate(Ogre::Vector3(0.0,6.0,-15.0));
		keyTurbine->setRotation(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(1,0,0)));

		animationStateExpRightWing =  mSceneMgr->createAnimationState("expandRightWingAnimation");
		animationStateExpLeftWing =  mSceneMgr->createAnimationState("expandLeftWingAnimation");
		animationStateRollWheel1 =  mSceneMgr->createAnimationState("rollWheel1Animation");
		animationStateRollWheel2 =  mSceneMgr->createAnimationState("rollWheel2Animation");
		animationStateRollWheel3 =  mSceneMgr->createAnimationState("rollWheel3Animation");
		animationStateRollWheel4 =  mSceneMgr->createAnimationState("rollWheel4Animation");
		animationStateTurbine = mSceneMgr->createAnimationState("turbineAnimation");

		animationStateExpRightWing->setLoop(false);
		animationStateExpLeftWing->setLoop(false);
		animationStateRollWheel1->setLoop(false);
		animationStateRollWheel2->setLoop(false);
		animationStateRollWheel3->setLoop(false);
		animationStateRollWheel4->setLoop(false);
		animationStateTurbine->setLoop(false);



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

		Ogre::Entity* entityObst2[8];
		
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
			entityRock01[i]->setMaterialName("lambert1");
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
			entityRock02[i]->setMaterialName("lambert1");
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
			entityRock03[i]->setMaterialName("lambert1");
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
			entityRock04[i]->setMaterialName("lambert1");
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