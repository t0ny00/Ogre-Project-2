#include "Ogre\ExampleApplication.h"

#define coinNumber 30


class Example1 : public ExampleApplication
{

public:

	void createCamera() {

		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(0,100,-200);
		mCamera->lookAt(0,0,100);
		mCamera->setNearClipDistance(1);

	}

	void createScene()
	{

		mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0));
		mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
		
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
		nodeCoin[3]->translate(30,0,0);
		nodeCoin[4]->translate(15,0,0);
		nodeCoin[6]->translate(-70,0,0);
		nodeCoin[7]->translate(-25,0,0);
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

	}

};


int main (void)
{

	Example1 app;
	app.go();
	return 0; 
}