/*
 * Model.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "Model.h"

#include <algorithm>
#include <osg/Geometry>
#include <osg/FrontFace>
#include <osgUtil/SmoothingVisitor>

#include "Asset.h"
#include "Exception.h"
#include "Database.h"
#include "OdDefines.h"
#include "ModelFactory.h"
#include "Texture.h"

namespace od
{

	Model::Model(Database &db, RecordId modelId)
	: Asset(db, modelId)
	, mModelName("")
	, mTriangleCount(0)
	, mQuadCount(0)
	, mVerticesLoaded(false)
	, mTexturesLoaded(false)
	, mFacesLoaded(false)
	{
	}

	void Model::loadNameAndShading(ModelFactory &factory, DataReader dr)
	{
		dr >> mModelName;

		uint32_t shadingType;
		dr >> shadingType;
	}

	void Model::loadVertices(ModelFactory &factory, DataReader dr)
	{
		uint16_t vertexCount;
		dr >> vertexCount;

		mVertices.reserve(vertexCount);
		for(size_t i = 0; i < vertexCount; ++i)
		{
			float x;
			float y;
			float z;

			dr >> x
			   >> y
			   >> z;

			mVertices.push_back(osg::Vec3(x,y,z));
		}

		mVerticesLoaded = true;
	}

	void Model::loadTextures(ModelFactory &factory, DataReader dr)
	{
		uint32_t textureCount;
		dr >> textureCount;

		mTextureRefs.reserve(textureCount);
		for(size_t i = 0; i < textureCount; ++i)
		{
			AssetRef textureRef;
			dr >> textureRef;

			mTextureRefs.push_back(textureRef);
		}

		mTexturesLoaded = true;
	}

	void Model::loadFaces(ModelFactory &factory, DataReader dr)
	{
		if(!mTexturesLoaded || !mVerticesLoaded)
		{
			throw Exception("Must load vertices and textures before loading faces!");
		}

		uint16_t faceCount;
		dr >> faceCount;

		mFaces.reserve(faceCount);
		for(size_t i = 0; i < faceCount; ++i)
		{
			uint16_t textureIndex;
			uint16_t vertexCount;

			dr >> DataReader::Ignore(2)
			   >> vertexCount
			   >> textureIndex;

			Face f;
			f.texture = mTextureRefs[textureIndex];
			f.vertexCount = vertexCount;

			if(f.vertexCount == 3)
			{
				++mTriangleCount;

			}else if(f.vertexCount == 4)
			{
				++mQuadCount;

			}else
			{
				throw UnsupportedException("Can't load model with non-triangle/non-quad primitives");
			}

			for(size_t i = 0; i < f.vertexCount; ++i)
			{
				float u;
				float v;
				uint16_t vertexIndex;

				dr >> vertexIndex
				   >> u
				   >> v;

				f.vertexIndices[i] = vertexIndex;
				f.vertexUvCoords[i] = osg::Vec2(u,v);
			}

			mFaces.push_back(f);
		}

		mFacesLoaded = true;
	}

	void Model::buildGeometry()
	{
		if(!mTexturesLoaded || !mVerticesLoaded || !mFacesLoaded)
		{
			throw Exception("Must load at least vertices, textures and faces before building geometry");
		}

		Logger::debug() << "Building geometry for model " << std::hex << getAssetId() << std::dec;

		SegmentedGeode::build(getDatabase(), mVertices, mFaces, mTextureRefs.size());

		osgUtil::SmoothingVisitor sm;
        this->accept(sm);

        // model faces are oriented CW for some reason
        this->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE), osg::StateAttribute::ON);
	}
}

