/*
 * SegmentedGeode.h
 *
 *  Created on: 10 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_SEGMENTEDGEODE_H_
#define INCLUDE_SEGMENTEDGEODE_H_

#include <vector>
#include <osg/Geode>

#include <odCore/db/Asset.h>

namespace od
{

	class AssetProvider;

	/**
	 * Geode for constructing geometries with mutiple textures that splits
	 * them into segments with the same texture and converts quads to triangles.
	 *
	 * Construction is O(n*log(n)).
	 *
	 * @deprecated This is a really ugly implementation of the model splitting we
	 * need. Look at GeodeBuilder for a better alternative that supports skinning.
	 */
	class SegmentedGeode : public osg::Geode
	{
	public:

		// TODO: implement copy constructor etc. in a meaningful way for this macro to work
		//META_Node(od, SegmentedGeode);

		struct Face
		{
			Face() : vertexCount(0), texture(0xffff,0xffff) {}

			size_t     vertexCount;
			AssetRef   texture;
			size_t     vertexIndices[4];
        	osg::Vec2f vertexUvCoords[4];
		};

		typedef std::vector<osg::Vec3f>::iterator VertexIter;
		typedef std::vector<Face>::iterator FaceIter;


		void build(AssetProvider &db, std::vector<osg::Vec3f> &vertexArray, std::vector<Face> &faceArray, size_t textureCount = 0);
		void build(AssetProvider &db, VertexIter vertexBegin, VertexIter vertexEnd, FaceIter faceBegin, FaceIter faceEnd, size_t textureCount = 0);
	};

}

#endif /* INCLUDE_SEGMENTEDGEODE_H_ */
