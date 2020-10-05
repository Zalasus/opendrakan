/*
 * ClassRegistry.cpp
 *
 *  Created on: Sep 8, 2019
 *      Author: zal
 */

#include <dragonRfl/RflDragon.h>

// #include <dragonRfl/classes/AnimationDemo.h>
#include <dragonRfl/classes/Building.h>
#include <dragonRfl/classes/CountDownToZero.h>
#include <dragonRfl/classes/Detector.h>
// #include <dragonRfl/classes/DragonControl.h>
#include <dragonRfl/classes/DynamicLight.h>
// #include <dragonRfl/classes/HealthPack.h>
#include <dragonRfl/classes/HumanControl.h>
// #include <dragonRfl/classes/Key.h>
// #include <dragonRfl/classes/Material.h>
// #include <dragonRfl/classes/PushableObject.h>
// #include <dragonRfl/classes/Sky.h>
// #include <dragonRfl/classes/SoundEffect.h>
#include <dragonRfl/classes/StaticLight.h>
#include <dragonRfl/classes/Timer.h>
#include <dragonRfl/classes/TrackingCamera.h>
// #include <dragonRfl/classes/UserInterfaceProperties.h>
// #include <dragonRfl/classes/VisibilityToggler.h>

namespace dragonRfl
{

    void DragonRfl::_registerClasses()
    {
        //registerClass<AnimationDemo>();
        registerClass<Building>();
        registerClass<CountDownToZero>();
        registerClass<Detector>();
        // registerClass<DragonControl>();
        registerClass<DynamicLight>();
        // registerClass<HealthPack>();
        registerClass<HumanControl>();
        // registerClass<Key>();
        // registerClass<Material>();
        // registerClass<BlendedMaterial>();
        // registerClass<PushableObject>();
        // registerClass<DomedSky>();
        // registerClass<SoundEffect>();
        registerClass<StaticLight>();
        registerClass<Timer>();
        registerClass<TrackingCamera>();
        // registerClass<UserInterfaceProperties>();
        // registerClass<VisibilityToggler>();
    }

}
