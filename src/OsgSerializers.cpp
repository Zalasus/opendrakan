/*
 * OsgSerializers.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#include <osg/Vec2f>
#include <osg/Vec3f>
#include <osg/Quat>

#include "DataStream.h"

/**
 * Serializer/deserializer functions for osg objects
 */

namespace od
{

	template <>
	DataReader &DataReader::operator >> <osg::Vec2f>(osg::Vec2f &v)
	{
		float x;
		float y;

		(*this) >> x >> y;

		v.set(x, y);

		return *this;
	}

	template <>
	DataReader &DataReader::operator >> <osg::Vec3f>(osg::Vec3f &v)
	{
		float x;
		float y;
		float z;

		(*this) >> x >> y >> z;

		v.set(x,y,z);

		return *this;
	}

	template <>
	DataReader &DataReader::operator >> <osg::Quat>(osg::Quat &q)
	{
	    float x;
	    float y;
	    float z;
	    float w;

	    (*this) >> x >> y >> z >> w;

	    q.set(x,y,z,w);

	    return *this;
	}

}
