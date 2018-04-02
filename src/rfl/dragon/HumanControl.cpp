/*
 * HumanControl.cpp
 *
 *  Created on: 2 Apr 2018
 *      Author: zal
 */

#include "rfl/dragon/HumanControl.h"

#include "rfl/Rfl.h"
#include "LevelObject.h"
#include "Level.h"
#include "Player.h"

namespace odRfl
{


	class PlayerUpdateCallback : public osg::NodeCallback
	{
	public:

		PlayerUpdateCallback(od::Player &player)
		: mPlayer(player)
		{
		}

		virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
		{
			traverse(node, nv);

			if(nv->getFrameStamp() != nullptr)
			{
				mPlayer.update(nv->getFrameStamp()->getSimulationTime());
			}
		}


	private:

		od::Player &mPlayer;

	};



	HumanControl::HumanControl()
    {

    }

    void HumanControl::probeFields(RflFieldProbe &probe)
    {
    	PlayerCommon::probeFields(probe);
    }

    void HumanControl::spawn(od::LevelObject &obj)
    {
    	// all aboard the indirection train!
    	obj.getLevel().getPlayer().setLevelObject(od::LevelObjectPtr(&obj));

    	Logger::verbose() << "Spawned Human Control at "
    			<< obj.getPosition().x() << "/"
				<< obj.getPosition().y() << "/"
				<< obj.getPosition().z();

    	obj.addUpdateCallback(new PlayerUpdateCallback(obj.getLevel().getPlayer()));
    }

    OD_REGISTER_RFL_CLASS(0x0009, "Human Control", HumanControl);

}
