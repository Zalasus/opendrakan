/*
 * Model.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "db/Model.h"

#include <algorithm>
#include <osg/Geometry>
#include <osg/FrontFace>
#include <osgUtil/SmoothingVisitor>

#include "db/Asset.h"
#include "Exception.h"
#include "db/Database.h"
#include "OdDefines.h"
#include "db/ModelFactory.h"
#include "db/Texture.h"

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

	void Model::loadNameAndShading(ModelFactory &factory, DataReader &&dr)
	{
		dr >> mModelName;

		uint32_t shadingType;
		dr >> shadingType;
	}

	void Model::loadVertices(ModelFactory &factory, DataReader &&dr)
	{
		uint16_t vertexCount;
		dr >> vertexCount;

		mVertices.reserve(vertexCount);
		for(size_t i = 0; i < vertexCount; ++i)
		{
			osg::Vec3f vertex;

			dr >> vertex;

			mVertices.push_back(vertex);
		}

		mVerticesLoaded = true;
	}

	void Model::loadTextures(ModelFactory &factory, DataReader &&dr)
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

	void Model::loadFaces(ModelFactory &factory, DataReader &&dr)
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
				uint16_t vertexIndex;

				dr >> vertexIndex
				   >> f.vertexUvCoords[i];

				f.vertexIndices[i] = vertexIndex; // TODO: instead of doing it this way, maybe add a "readAs<type>" modifier?
			}

			mFaces.push_back(f);
		}

		mFacesLoaded = true;
	}

	void Model::loadCharacterData(ModelFactory &factory, DataReader &&dr)
	{

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

