/*
 * DebugDrawer.h
 *
 *  Created on: 17 Apr 2018
 *      Author: zal
 */

#ifndef INCLUDE_PHYSICS_DEBUGDRAWER_H_
#define INCLUDE_PHYSICS_DEBUGDRAWER_H_

#include <LinearMath/btIDebugDraw.h>

namespace od
{

	class DebugDrawer : public btIDebugDraw
	{
	public:

		virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color) override;

	};

}

#endif /* INCLUDE_PHYSICS_DEBUGDRAWER_H_ */
