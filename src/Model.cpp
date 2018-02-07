/*
 * Model.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include "Model.h"

#include <osg/Geometry>

#include "Asset.h"
#include "Exception.h"
#include "Database.h"
#include "ModelFactory.h"
#include "Texture.h"

namespace od
{

	Model::Model(RecordId modelId)
	: Asset(modelId)
	, mModelName("")
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
	}

	void Model::loadFaces(ModelFactory &factory, DataReader dr)
	{
		uint16_t faceCount;
		dr >> faceCount;

		mFaces.reserve(faceCount);
		for(size_t i = 0; i < faceCount; ++i)
		{
			Face f;

			dr >> DataReader::Ignore(2)
			   >> f.vertexCount
			   >> f.materialIndex;

			for(size_t i = 0; i < f.vertexCount; ++i)
			{
				float u;
				float v;

				dr >> f.vertexIndices[i]
				   >> u
				   >> v;

				f.vertexUvCoords[i] = osg::Vec2(u,v);
			}

			if(f.vertexCount != 3 && f.vertexCount != 4)
			{
				throw UnsupportedException("Can't load model with non-triangle/on-quad primitives");
			}

			mFaces.push_back(f);
		}
	}

	void Model::loadTextures(ModelFactory &factory, DataReader dr)
	{
		uint32_t textureCount;
		dr >> textureCount;

		mTextures.reserve(textureCount);
		for(size_t i = 0; i < textureCount; ++i)
		{
			AssetRef textureRef;
			dr >> textureRef;

			TexturePtr tex = factory.getDatabase().getAssetAsTexture(textureRef);
			mTextures.push_back(osg::ref_ptr<osg::Texture2D>(new osg::Texture2D(tex)));
		}
	}

	void Model::buildGeometry()
	{
		// for simplicity, create a geometry for every face. is just want to see how it looks right now
		for(Face f : mFaces)
		{
			osg::ref_ptr<osg::Geometry> geometry(new osg::Geometry);

			osg::ref_ptr<osg::Vec3Array> vertices(new osg::Vec3Array);
			osg::ref_ptr<osg::Vec2Array> uvCoords(new osg::Vec2Array);
			vertices->reserve(f.vertexCount);
			uvCoords->reserve(f.vertexCount);
			for(size_t i = 0; i < f.vertexCount; ++i)
			{
				vertices->push_back(mVertices[f.vertexIndices[i]]);
				uvCoords->push_back(f.vertexUvCoords[i]);
			}
			geometry->setVertexArray(vertices);
			geometry->setTexCoordArray(0, uvCoords);

			if(f.vertexCount == 3)
			{
				geometry->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::TRIANGLES, 0, 3));

			}else if(f.vertexCount == 4)
			{
				geometry->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::QUADS, 0, 4));
			}

			geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, mTextures[f.materialIndex]);

			this->addDrawable(geometry);
		}
	}
}

