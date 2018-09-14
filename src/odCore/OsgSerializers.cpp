/*
 * OsgSerializers.cpp
 *
 *  Created on: 26 Feb 2018
 *      Author: zal
 */

#include <osg/Vec2f>
#include <osg/Vec3f>
#include <osg/Quat>
#include <osg/BoundingSphere>
#include <osg/Matrixf>

#include <odCore/DataStream.h>

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

	template <>
	DataReader &DataReader::operator >> <osg::Matrixf>(osg::Matrixf &m)
	{
	    float l[9]; // linear thingy
	    osg::Vec3f t; // offset

	    for(size_t i = 0; i < sizeof(l)/sizeof(float); ++i)
	    {
	    	(*this) >> l[i];
	    }

	    (*this) >> t;

	    // that's how these work, right? because it doesn't make much sense to me to put the offset in row 4 rather than column 4
	    //  NOTE: it turns out OSG uses row-major notation with prefix operations v' = (v*M) and transposed vectors. so this makes sense
	    m.set(l[0], l[1], l[2], 0,
	    	  l[3], l[4], l[5], 0,
			  l[6], l[7], l[8], 0,
			  t[0], t[1], t[2], 1);

	    return *this;
	}

	template <>
	DataReader &DataReader::operator >> <osg::BoundingSpheref>(osg::BoundingSpheref &bs)
	{
		osg::Vec3f center;
		float radius;

		(*this) >> center >> radius;

		bs.set(center, radius);

		return *this;
	}

}
