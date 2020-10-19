
#include <dragonRfl/classes/Fader.h>

#include <odCore/LevelObject.h>

namespace dragonRfl
{

	FaderFields::FaderFields()
	: fadeInTime(1.0)
	, fadedTime(2.0)
	, fadeOutTime(true)
	, startMode(StartMode::RUN_WHEN_TRIGGERED)
	, color(255, 255, 255)
	{
	}

	void FaderFields::probeFields(odRfl::FieldProbe &probe)
	{
		probe("Fader")
				(fadeInTime, "Fade In Time (s)")
                (fadedTime, "Faded Time (-1.0s = trigger)")
                (fadeOutTime, "Fade Out Time (s)")
                (startMode, "Start Mode")
                (color, "Color");
	}


	void Fader_Cl::onSpawned()
	{
        auto &obj = getLevelObject();
	    obj.setEnableUpdate(true);

        obj.setupRenderingAndPhysics(od::ObjectRenderMode::NO_LIGHTING, od::ObjectPhysicsMode::NO_PHYSICS);
	}

	void Fader_Cl::onUpdate(float relTime)
	{
    }

}
