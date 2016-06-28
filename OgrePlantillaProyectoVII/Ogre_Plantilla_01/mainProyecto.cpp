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
bool finished = true;


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
Ogre::SceneNode* turbine;
Ogre::SceneNode* wing1;
Ogre::SceneNode* wing2;

Ogre::SceneNode* nodeCam;

int movingObstDir[8];
int movingObstSpeed[8];
Vector3 wallLeftCoord[119];
Vector3 wallRightCoord[119];
Vector3 wallRightSecondCoord[119];
Vector3 wallLeftSecondCoord[119];

Ogre::SceneNode* nodeParticle;
Ogre::ParticleSystem* partSystem;

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
		nodeCam->attachObject(_cam);
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
		float xLimit1;

		if (nodePlayer->getPosition().z > -50 && nodePlayer->getPosition().z < 390 ) xLimit1 = 122;
		else if (nodePlayer->getPosition().z > 390 && nodePlayer->getPosition().z < 2320 ) xLimit1 = 204;
		else if (nodePlayer->getPosition().z > 2940 && nodePlayer->getPosition().z < 4930 ) xLimit1 = 32;
		else if (nodePlayer->getPosition().z > 5505 && nodePlayer->getPosition().z < 6530 ) xLimit1 = 204;
		else xLimit1 = 1000;

		//Camara
		if(_key->isKeyDown(OIS::KC_R) && finished == true){
			spaceship_mode = false;
			animationStateExpRightWing->setTimePosition(0);
			animationStateExpLeftWing->setTimePosition(0);
			animationStateRollWheel1->setTimePosition(0);
			animationStateRollWheel2->setTimePosition(0);
			animationStateRollWheel3->setTimePosition(0);
			animationStateRollWheel4->setTimePosition(0);
			animationStateTurbine->setTimePosition(0);
			animationStateExpRightWing->setEnabled(false);
			animationStateExpLeftWing->setEnabled(false);
			animationStateRollWheel1->setEnabled(false);
			animationStateRollWheel2->setEnabled(false);
			animationStateRollWheel3->setEnabled(false);
			animationStateRollWheel4->setEnabled(false);
			animationStateTurbine->setEnabled(false);
			score = 0;
			scoreText->setCaption("Score: 0");
			overlay->show();
			rotatorRueda01->resetToInitialState();
			rotatorRueda02->resetToInitialState();
			rotatorRueda03->resetToInitialState();
			rotatorRueda04->resetToInitialState();
			nodeRueda01->resetToInitialState();
			nodeRueda02->resetToInitialState();
			nodeRueda03->resetToInitialState();
			nodeRueda04->resetToInitialState();
			turbine->resetToInitialState();
			wing2->resetToInitialState();
			wing1->resetToInitialState();
			nodePlayer->resetToInitialState();
			nodeCam->resetToInitialState();
			for (int i = 0; i < (sizeof(nodeCoin) / sizeof(nodeCoin[0])); i++){
				nodeCoin[i]->setVisible(true);
				coinsCollected[i] = 0;
			}
			
		}

		if (_key->isKeyDown(OIS::KC_ESCAPE))
			return false;


		bool hit = false;
		Vector3 mov(0,0,0);

		if(_key->isKeyDown(OIS::KC_W)){
			for (int i = 0; i < (sizeof(wallLeftCoord) / sizeof(wallLeftCoord[0])); i++) // Loop through the entities
			{
				if(collisionWall(nodePlayer,wallLeftCoord[i],radiusCar,radiusWall)) hit = true;
				if(collisionWall(nodePlayer,wallRightCoord[i],radiusCar,radiusWall)) hit = true;
			}
			if (!hit){
				orientation = 1;
				inertia_time = 2.0;
				inertia_speed = 1.0;
				hit = false;
			}
			if ( nodePlayer->getPosition().y > 300 && spaceship_mode) 
				nodePlayer->setPosition(nodePlayer->getPosition().x,nodePlayer->getPosition().y-20,nodePlayer->getPosition().z);
			if ( nodePlayer->getPosition().y < -300 && spaceship_mode) 
				nodePlayer->setPosition(nodePlayer->getPosition().x,nodePlayer->getPosition().y+20,nodePlayer->getPosition().z);
			if ( nodePlayer->getPosition().x > 300 && spaceship_mode) 
				nodePlayer->setPosition(nodePlayer->getPosition().x-20,nodePlayer->getPosition().y,nodePlayer->getPosition().z);
			if ( nodePlayer->getPosition().x < -300 && spaceship_mode) 
				nodePlayer->setPosition(nodePlayer->getPosition().x+20,nodePlayer->getPosition().y,nodePlayer->getPosition().z);
			if ( nodePlayer->getPosition().z < 6520 && spaceship_mode) 
				nodePlayer->setPosition(nodePlayer->getPosition().x,nodePlayer->getPosition().y,nodePlayer->getPosition().z+20);
		}
		
		for (int i = 0; i < (sizeof(wallLeftCoord) / sizeof(wallLeftCoord[0])); i++) // Loop through the entities
		{
				if(collisionWall(nodePlayer,wallLeftSecondCoord[i],radiusCar,radiusWall)) hit = true;
				if(collisionWall(nodePlayer,wallRightSecondCoord[i],radiusCar,radiusWall)) hit = true;
		}
		

		if(_key->isKeyDown(OIS::KC_A) && nodePlayer->getPosition().x < xLimit1 && !hit){
			if(inertia_speed > 0 && !hit) mov += Ogre::Vector3(colliding*80*inertia_speed,0,0);
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

		if(_key->isKeyDown(OIS::KC_S)){
			for (int i = 0; i < (sizeof(wallLeftCoord) / sizeof(wallLeftCoord[0])); i++) // Loop through the entities
			{
				if(collisionWall(nodePlayer,wallRightSecondCoord[i],radiusCar,radiusWall)) hit = true;
				if(collisionWall(nodePlayer,wallLeftSecondCoord[i],radiusCar,radiusWall)) hit = true;
			}
			if (!hit){
				inertia_time = 2.0;
				inertia_speed = 1.0;
				orientation = -1;
				hit = false;
			}
			if ( nodePlayer->getPosition().y > 300 ) 
				nodePlayer->setPosition(nodePlayer->getPosition().x,nodePlayer->getPosition().y-20,nodePlayer->getPosition().z);
			if ( nodePlayer->getPosition().y < -300) 
				nodePlayer->setPosition(nodePlayer->getPosition().x,nodePlayer->getPosition().y+20,nodePlayer->getPosition().z);
			if ( nodePlayer->getPosition().z < 6520 && spaceship_mode) 
				nodePlayer->setPosition(nodePlayer->getPosition().x,nodePlayer->getPosition().y,nodePlayer->getPosition().z+20);
			if ( nodePlayer->getPosition().x > 300 && spaceship_mode) 
				nodePlayer->setPosition(nodePlayer->getPosition().x-20,nodePlayer->getPosition().y,nodePlayer->getPosition().z);
			if ( nodePlayer->getPosition().x < -300 && spaceship_mode) 
				nodePlayer->setPosition(nodePlayer->getPosition().x+20,nodePlayer->getPosition().y,nodePlayer->getPosition().z);
			if ( nodePlayer->getPosition().x  < -122 && nodePlayer->getPosition().x  > -204 && nodePlayer->getPosition().z  < 415 ) 
				nodePlayer->setPosition(nodePlayer->getPosition().x,nodePlayer->getPosition().y,nodePlayer->getPosition().z+10);
			if ( nodePlayer->getPosition().x  > 122 && nodePlayer->getPosition().x  < 204 && nodePlayer->getPosition().z  < 415 ) 
				nodePlayer->setPosition(nodePlayer->getPosition().x,nodePlayer->getPosition().y,nodePlayer->getPosition().z+10);
		}
		
		if(_key->isKeyDown(OIS::KC_D) && nodePlayer->getPosition().x > -xLimit1 && !hit){
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

		

		if(nodePlayer->getPosition().z >= 6520){
			spaceship_mode = true;
			animationStateExpRightWing->setEnabled(true);
			animationStateExpLeftWing->setEnabled(true);
			animationStateRollWheel1->setEnabled(true);
			animationStateRollWheel2->setEnabled(true);
			animationStateRollWheel3->setEnabled(true);
			animationStateRollWheel4->setEnabled(true);
			animationStateTurbine->setEnabled(true);
			
		}	
		
		if(nodePlayer->getPosition().z >= 3000 && nodePlayer->getPosition().z < 4950) nodeCam->setPosition(0,-70,40);
		else nodeCam->setPosition(0,0,0);

		//camara control
		float rotX = _mouse->getMouseState().X.rel * evt.timeSinceLastFrame*-1;
		float rotY = _mouse->getMouseState().Y.rel * evt.timeSinceLastFrame*-1;
		if (spaceship_mode) nodePlayer->pitch(Ogre::Radian(rotY));
		if (spaceship_mode) nodeCam->pitch(-Ogre::Radian(rotY));

		

		
		for (int i = 0; i < (sizeof(nodeObst1) / sizeof(nodeObst1[0])); i++){
			rotateObstacle(nodeObst1[i],4 * evt.timeSinceLastFrame);
			if (collision(nodePlayer,nodeObst1[i],radiusCar,radiusObstacles) && !isColliding){
				isColliding = true;
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
			if ((collision(nodePlayer,nodeRock01[i],radiusCar,radiusObstacles + 0.2) ||
				collision(nodePlayer,nodeRock02[i],radiusCar,radiusObstacles + 0.2)  ||
				collision(nodePlayer,nodeRock03[i],radiusCar,radiusObstacles + 0.2)  ||
				collision(nodePlayer,nodeRock04[i],radiusCar,radiusObstacles + 0.2)) && !isColliding){
				isColliding = true;
				inertia_speed += 0.2;
				bounce_back_time = 0.3;
				colliding = -1;
			}
			else isColliding = false;
		};

		Ogre::String scoreString;
		for (int i = 0; i < (sizeof(nodeCoin) / sizeof(nodeCoin[0])); i++){
			if (collision(nodePlayer,nodeCoin[i],radiusCar,radiusCoin) && coinsCollected[i] == 0) {
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

		if (nodePlayer->getPosition().z > 10325 ){
			finished = true;
			scoreString = Ogre::StringConverter::toString(score); 
			scoreText->setCaption("Final Score: " + scoreString + "\n\n PRESS 'R' TO RESTART GAME");
			overlay->show();
		}

		nodePlayer->translate(nodePlayer->getOrientation()*mov*evt.timeSinceLastFrame);
		if (animationStateExpRightWing->getEnabled()) animationStateExpRightWing->addTime(evt.timeSinceLastFrame);
		if (animationStateExpLeftWing->getEnabled()) animationStateExpLeftWing->addTime(evt.timeSinceLastFrame);
		if (animationStateRollWheel1->getEnabled()) animationStateRollWheel1->addTime(evt.timeSinceLastFrame);
		if (animationStateRollWheel2->getEnabled()) animationStateRollWheel2->addTime(evt.timeSinceLastFrame);
		if (animationStateRollWheel3->getEnabled()) animationStateRollWheel3->addTime(evt.timeSinceLastFrame);
		if (animationStateRollWheel4->getEnabled()) animationStateRollWheel4->addTime(evt.timeSinceLastFrame);
		if (animationStateTurbine->getEnabled()) animationStateTurbine->addTime(evt.timeSinceLastFrame);



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
	  manual->begin("LightBlue3", RenderOperation::OT_TRIANGLE_STRIP);
	  

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
	  manual->begin("LightBlue3", RenderOperation::OT_TRIANGLE_STRIP);
	  
	  
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

		OverlayManager& overlayManager = OverlayManager::getSingleton();
 
		// Create a panel
		OverlayContainer* panel = static_cast<OverlayContainer*>(
		overlayManager.createOverlayElement("Panel", "PanelName"));
		panel->setMetricsMode(Ogre::GMM_PIXELS);
		panel->setPosition(10, 10);
		panel->setDimensions(100, 100);
 
		// Create a text area
		scoreText = static_cast<TextAreaOverlayElement*>(
		overlayManager.createOverlayElement("TextArea", "TextAreaName"));
		scoreText->setMetricsMode(Ogre::GMM_PIXELS);
		scoreText->setPosition(0, 0);
		scoreText->setDimensions(100, 100);
		scoreText->setCaption("Score: 0");
		scoreText->setCharHeight(35);
		scoreText->setFontName("AntonioScaramazza-CarlosMartinez-Font");
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


		nodePlayer = mSceneMgr->createSceneNode("Player");
		mSceneMgr->getRootSceneNode()->addChild(nodePlayer);
		nodeCam = mSceneMgr->createSceneNode("Cam");
		nodePlayer->addChild(nodeCam);
		
		

		//Chasis
		_nodeChasis01 = mSceneMgr->createSceneNode("Chasis01");
		nodePlayer->addChild(_nodeChasis01);
			
		Ogre::Entity* _entChasis01 = mSceneMgr->createEntity("entChasis01", "chasisCarro.mesh");
		_entChasis01->setMaterialName("shCarro01");
		_nodeChasis01->attachObject(_entChasis01);


		//turbine
		turbine = mSceneMgr->createSceneNode("turbine");
		_nodeChasis01->addChild(turbine);

		Ogre::Entity* entTurbine = mSceneMgr->createEntity("entTurbine", "Barrel.mesh");\
		entTurbine->setMaterialName("LightBlue3");
		turbine->attachObject(entTurbine);
		turbine->rotate(Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3(1,0,0)));
		turbine->setScale(1.0,0.0,1.0);
		turbine->translate(0.0,6.0,0.0);
		
		


		//Wings
		wing1 = mSceneMgr->createSceneNode("wing1");
		_nodeChasis01->addChild(wing1);
			
		ManualObject* wing1object;
		wing1object = generateWing1("wing1obj",0.0,0.0,0.0,0.0,0.1,4.0,4.0,0.1,4.0,4.0,2.0);
		wing1->attachObject(wing1object);
		wing1->setScale(0.0,3.0,2.0);
		wing1->translate(0.0,4.0,0.0);

		wing2 = mSceneMgr->createSceneNode("wing2");
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
		rotatorRueda04->setInitialState();
		rotatorRueda03->setInitialState();
		rotatorRueda02->setInitialState();
		rotatorRueda01->setInitialState();
		
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
		_entRueda03->setMaterialName("shRueda02");
		nodeRueda03->attachObject(_entRueda03);

		//Rueda 4

		nodeRueda04 = mSceneMgr->createSceneNode("Rueda04");
		rotatorRueda02->addChild(nodeRueda04);
			
		Ogre::Entity* _entRueda04 = mSceneMgr->createEntity("entRueda04", "ruedaDetallada.mesh");
		_entRueda04->setMaterialName("shRueda02");
		nodeRueda04->attachObject(_entRueda04);


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
		Ogre::Entity* entityDecorBase3[8];
		Ogre::SceneNode* nodeDecor3[8];
		Ogre::SceneNode* nodeDecorBase3[8];
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
			nodeDecor3[i]->setPosition(temp*220 ,110, i * 250 + 500);
			nodeDecor3[i]->setScale(0.5,0.5,0.5);
			nodeDecor3[i]->attachObject(entityDecor3[i]);


			entityDecorBase3[i] = mSceneMgr->createEntity("DecorBase30 " + number, "cube.mesh");
			entityDecorBase3[i]->setMaterialName("lambert1");
			nodeDecorBase3[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeDecorBase3 " + number);
			nodeDecorBase3[i]->scale(0.3,0.7,0.3);
			nodeDecorBase3[i]->setPosition(temp*230 ,35, i * 250 + 500);
			nodeDecorBase3[i]->attachObject(entityDecorBase3[i]);
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
		Ogre::Entity* entityDecorBase6[5];
		Ogre::SceneNode* nodeDecorBase6[5];
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
			nodeDecor6[i]->setPosition(temp*220 ,110, i * 100 + 5700);
			nodeDecor6[i]->setScale(0.5,0.5,0.5);
			nodeDecor6[i]->attachObject(entityDecor6[i]);


			entityDecorBase6[i] = mSceneMgr->createEntity("DecorBase60 " + number, "cube.mesh");
			entityDecorBase6[i]->setMaterialName("lambert1");
			nodeDecorBase6[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("nodeDecorBase6 " + number);
			nodeDecorBase6[i]->scale(0.3,0.7,0.3);
			nodeDecorBase6[i]->setPosition(temp*230 ,35, i * 100 + 5700);
			nodeDecorBase6[i]->attachObject(entityDecorBase6[i]);
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
			

		}

		nodeObst1[1]->translate(-50,0,0);
		nodeObst1[2]->translate(45,0,0);
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
			nodeObst3[i]->setPosition(0 , 0 , i * 150 + 5600);
			nodeObst3[i]->setScale(0.5,0.5,0.5);
			nodeObst3[i]->attachObject(entityObst3[i]);
			

		}

		nodeObst3[1]->translate(-50,0,0);
		nodeObst3[2]->translate(45,0,0);
		nodeObst3[4]->translate(-35,0,0);
		nodeObst3[5]->translate(35,0,0);
		
		for (int i = 0; i < 8; i++){
			movingObstDir[i] = rand() % 2;
			movingObstSpeed[i] = rand() % 50 + 200;
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
			nodeRock01[i]->setPosition(rand() % 250 - 150 , rand() % 250 - 150 , i * 250 + 7000);
			nodeRock01[i]->attachObject(entityRock01[i]);
			

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

			nodeRock02[i]->setPosition(rand() % 250 - 150 , rand() % 250 - 150 , i * 350 + 6800);

			nodeRock02[i]->attachObject(entityRock02[i]);
			

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

			nodeRock03[i]->setPosition(rand() % 250 - 150 , rand() % 250 - 150 , i * 315 + 7500);

			nodeRock03[i]->attachObject(entityRock03[i]);
			

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

			nodeRock04[i]->setPosition(rand() % 250 - 150 , rand() % 250 - 150 , i * 375 + 7700);

			nodeRock04[i]->attachObject(entityRock04[i]);
			
			
		}


	}

};


int main (void)
{

	Example1 app;
	app.go();
	return 0; 
}