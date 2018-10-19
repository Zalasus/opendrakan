/*
 * Model.cpp
 *
 *  Created on: 2 Feb 2018
 *      Author: zal
 */

#include <odCore/db/Model.h>

#include <algorithm>
#include <limits>
#include <osg/Geometry>
#include <osg/LOD>
#include <osg/FrontFace>

#include <odCore/OdDefines.h>
#include <odCore/Exception.h>
#include <odCore/db/Asset.h>
#include <odCore/db/ModelFactory.h>
#include <odCore/db/Texture.h>
#include <odCore/db/Skeleton.h>
#include <odCore/ShaderManager.h>

#define OD_POLYGON_FLAG_DOUBLESIDED 0x02

namespace odDb
{

	Model::Model(AssetProvider &ap, od::RecordId modelId)
	: Asset(ap, modelId)
	, mModelName("")
	, mShadingType(ModelShadingType::None)
	, mBlendWithLandscape(false)
	, mShiny(false)
	, mUseAdditiveBlending(false)
	, mEnvironmentMapped(false)
	, mVerticesLoaded(false)
	, mTexturesLoaded(false)
	, mPolygonsLoaded(false)
	, mGeometryBuilt(false)
	{
	}

	void Model::loadNameAndShading(ModelFactory &factory, od::DataReader &&dr)
	{
		dr >> mModelName;
		this->setName(mModelName);

		uint32_t shadingType;
		dr >> shadingType;
		if((shadingType & 0x02) && !(shadingType & 0x01))
		{
		    mShadingType = ModelShadingType::Smooth;

		}else if((shadingType & 0x01) && !(shadingType & 0x02))
		{
		    mShadingType = ModelShadingType::Flat;

		}else
		{
		    // if none or both flags are set, disable shading (the latter case as a failsafe)
		    mShadingType = ModelShadingType::None;
		}

		mEnvironmentMapped = shadingType & 0x20;
        mUseAdditiveBlending = shadingType & 0x10;
        mShiny = shadingType & 0x08;
        mBlendWithLandscape = shadingType & 0x04;
	}

	void Model::loadVertices(ModelFactory &factory, od::DataReader &&dr)
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

	void Model::loadTextures(ModelFactory &factory, od::DataReader &&dr)
	{
		uint32_t textureCount;
		dr >> textureCount;

		mTextureRefs.reserve(textureCount);
		for(size_t i = 0; i < textureCount; ++i)
		{
			AssetRef textureRef;
			dr >> textureRef;

			if(!textureRef.isNull())
			{
				mTextureRefs.push_back(textureRef);
			}
		}

		mTexturesLoaded = true;
	}

	void Model::loadPolygons(ModelFactory &factory, od::DataReader &&dr)
	{
		if(!mTexturesLoaded || !mVerticesLoaded)
		{
			throw od::Exception("Must load vertices and textures before loading polygons!");
		}

		uint16_t polygonCount;
		dr >> polygonCount;

		mPolygons.reserve(polygonCount);
		for(size_t i = 0; i < polygonCount; ++i)
		{
			uint16_t flags;
			uint16_t textureIndex;
			uint16_t vertexCount;

			dr >> flags
			   >> vertexCount
			   >> textureIndex;

			od::Polygon poly;
			poly.doubleSided = flags & OD_POLYGON_FLAG_DOUBLESIDED;
			poly.texture = mTextureRefs[textureIndex];
			poly.vertexCount = vertexCount;

			if(poly.vertexCount != 3 && poly.vertexCount != 4)
			{
				throw od::UnsupportedException("Can't load model with non-triangle/non-quad primitives");
			}

			for(size_t i = 0; i < poly.vertexCount; ++i)
			{
				uint16_t vertexIndex;

				dr >> vertexIndex
				   >> poly.uvCoords[i];

				poly.vertexIndices[i] = vertexIndex; // TODO: instead of doing it this way, maybe add a "readAs<type>" modifier?
			}

			mPolygons.push_back(poly);
		}

		mPolygonsLoaded = true;
	}

	void Model::loadBoundingData(ModelFactory &factory, od::DataReader &&dr)
	{
		if(isCharacter())
		{
			throw od::Exception("Character models can't have bounds info");
		}

		osg::BoundingSpheref mainBs;
		odPhysics::OrientedBoundingBox mainObb;
		uint16_t shapeCount;
		uint16_t shapeType; // 0 = spheres, 1 = boxes

		dr >> mainBs
		   >> mainObb
		   >> shapeCount
		   >> shapeType;

		odPhysics::ModelBounds::ShapeType type = (shapeType == 0) ? odPhysics::ModelBounds::SPHERES : odPhysics::ModelBounds::BOXES;
		mModelBounds.reset(new odPhysics::ModelBounds(type, shapeCount));
		mModelBounds->setMainBounds(mainBs, mainObb);

		for(size_t i = 0; i < shapeCount; ++i)
		{
			uint16_t firstChildIndex;
			uint16_t nextSiblingIndex;
			dr >> firstChildIndex
			   >> nextSiblingIndex;

			mModelBounds->addHierarchyEntry(firstChildIndex, nextSiblingIndex);
		}

		for(size_t i = 0; i < shapeCount; ++i)
		{
			if(shapeType == 0)
			{
				osg::BoundingSpheref bs;
				dr >> bs;
				mModelBounds->addSphere(bs);

			}else
			{
				odPhysics::OrientedBoundingBox obb;
				dr >> obb;
				mModelBounds->addBox(obb);

				// ignore the field of words after each box
				uint16_t unkPolyCount;
				dr >> unkPolyCount;
				dr.ignore(unkPolyCount*2);
			}
		}

		//Logger::info() << "Bounding data for model " << mModelName;
		//mModelBounds->printInfo();

		mModelBounds->getSharedCollisionShape(); // to trigger building the shape
	}

	void Model::loadLodsAndBones(ModelFactory &factory, od::DataReader &&dr)
	{
		if(hasBounds())
		{
			throw od::Exception("Character models can't have bounds info");
		}

		uint16_t lodCount;
		std::vector<std::string> lodNames;

		dr >> od::DataReader::Ignore(16*4) // bounding info (16 floats)
		   >> lodCount;

		if(lodCount == 0)
		{
			throw od::Exception("Expected at least one LOD in model");
		}
		mLodMeshInfos.resize(lodCount);

		lodNames.resize(lodCount - 1);
		for(int32_t i = 0; i < lodCount - 1; ++i)
		{
			dr >> lodNames[i];
		}


		mSkeletonBuilder.reset(new SkeletonBuilder);

		// node info
		uint16_t nodeInfoCount;
		dr >> nodeInfoCount;
		for(size_t i = 0; i < nodeInfoCount; ++i)
		{
			char nodeName[33] = { 0 };
			int32_t jointInfoIndex;

			dr.read(nodeName, 32);
			dr >> jointInfoIndex;

			mSkeletonBuilder->addBoneNode(std::string(nodeName), jointInfoIndex);
		}

		// joint info
		uint16_t jointInfoCount;
		dr >> jointInfoCount;
		for(size_t jointIndex = 0; jointIndex < jointInfoCount; ++jointIndex)
		{
			osg::Matrixf inverseBoneTransform;
			int32_t meshIndex;
            int32_t firstChildIndex;
            int32_t nextSiblingIndex;

            dr >> inverseBoneTransform
			   >> meshIndex
			   >> firstChildIndex
			   >> nextSiblingIndex;

            mSkeletonBuilder->addJointInfo(inverseBoneTransform, meshIndex, firstChildIndex, nextSiblingIndex);

            // affected vertex lists, one for each LOD
            for(size_t lodIndex = 0; lodIndex < lodCount; ++lodIndex)
            {
            	std::vector<od::BoneAffection> &boneAffections = mLodMeshInfos[lodIndex].boneAffections;

				uint16_t affectedVertexCount;
				dr >> affectedVertexCount;
				boneAffections.reserve(boneAffections.size() + affectedVertexCount);
				for(size_t vertexIndex = 0; vertexIndex < affectedVertexCount; ++vertexIndex)
				{
					uint32_t affectedVertexIndex;
					float weight;
					dr >> affectedVertexIndex
					   >> weight;

					od::BoneAffection bAff;
					bAff.jointIndex = jointIndex;
					bAff.vertexIndex = affectedVertexIndex;
					bAff.vertexWeight = weight;
					boneAffections.push_back(bAff);
				}

				if(lodIndex == 0)
				{
					mSkeletonBuilder->pfffSetWeightCount(affectedVertexCount);
				}
            }


		}

		// lod info
		for(size_t lodIndex = 0; lodIndex < lodCount; ++lodIndex)
		{
			uint16_t meshCount;
			dr >> meshCount;

			if(meshCount != 1)
			{
				throw od::UnsupportedException("Multi-mesh-models currently unsupported");
			}

			for(size_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
			{
				LodMeshInfo &mesh = mLodMeshInfos[lodIndex];

				dr >> mesh.distanceThreshold
				   >> mesh.usage
				   >> mesh.nodeIndex
				   >> mesh.firstVertexIndex
				   >> mesh.vertexCount
				   >> mesh.firstPolygonIndex
				   >> mesh.polygonCount;

				mesh.lodName = (lodIndex == 0) ? mModelName : lodNames[lodIndex - 1];
			}
		}

		// animations refs
		uint16_t animCount;
		dr >> animCount;
		mAnimationRefs.resize(animCount);
		for(size_t animIndex = 0; animIndex < animCount; ++animIndex)
		{
			// the database index of this is always zero. however, i see no reason not to support loading
			//  animations cross-database, so we don't enforce this.
			dr >> mAnimationRefs[animIndex];
		}


		// channels
		uint16_t channelCount;
		dr >> channelCount;
		for(size_t channelIndex = 0; channelIndex < channelCount; ++channelIndex)
		{
			uint32_t jointIndex;
			osg::Matrixf xformA;
			osg::Matrixf xformB;
            uint16_t capCount;

            dr >> jointIndex
			   >> xformA
			   >> xformB
			   >> capCount;

            mSkeletonBuilder->makeChannel(jointIndex);

            for(size_t capIndex = 0; capIndex < capCount; ++capIndex)
            {
            	uint32_t firstCapPolygonIndex;
                uint32_t capPolygonCount;
                uint32_t firstPartPolygonIndex;
                uint32_t partPolygonCount;
                uint32_t unk;
                uint16_t affectedVertexCount;

                dr >> firstCapPolygonIndex
				   >> capPolygonCount
				   >> firstPartPolygonIndex
				   >> partPolygonCount
				   >> unk
				   >> affectedVertexCount;

				for(size_t vertexIndex = 0; vertexIndex < affectedVertexCount; ++vertexIndex)
				{
					uint32_t affectedVertexIndex;
					float weight;
					dr >> affectedVertexIndex
					   >> weight;
				}
            }
		}

		//Logger::info() << "Skel info for model " << mModelName;
		//mSkeletonBuilder->printInfo(std::cout);

		dr >> od::DataReader::Expect<uint16_t>(lodCount);
		for(size_t lodIndex = 0; lodIndex < lodCount; ++lodIndex)
		{
		    uint16_t shapeCount;
		    dr >> shapeCount;

		    for(size_t shapeIndex = 0; shapeIndex < shapeCount; ++shapeIndex)
		    {
		        uint16_t firstChild;
		        uint16_t nextSibling;
		        float radius;
		        uint16_t channelIndex;

		        dr >> firstChild
		           >> nextSibling
		           >> od::DataReader::Ignore(4)
		           >> radius
		           >> channelIndex
		           >> od::DataReader::Ignore(2);

		        // where do the spheres go? there is no field in the structure for that. are they placed exactly at their joint?

		        /*Logger::info() << "Model '" << mModelName << "' lod=" << lodIndex
		                       << " ch=" << channelIndex
		                       << " fc="
		                       << firstChild
		                       << " ns="
		                       << nextSibling
		                       << " r=" << radius;*/
		    }
		}
 	}

	void Model::buildGeometry(od::ShaderManager &shaderManager)
	{
	    if(mGeometryBuilt)
	    {
	        return;
	    }

		if(!mTexturesLoaded || !mVerticesLoaded || !mPolygonsLoaded)
		{
			throw od::Exception("Must load at least vertices, textures and polygons before building geometry");
		}

		if(mLodMeshInfos.size() > 0)
		{
			osg::ref_ptr<osg::LOD> lodNode(new osg::LOD);
			lodNode->setRangeMode(osg::LOD::DISTANCE_FROM_EYE_POINT);
			lodNode->setCenterMode(osg::LOD::USE_BOUNDING_SPHERE_CENTER);

			for(auto it = mLodMeshInfos.begin(); it != mLodMeshInfos.end(); ++it)
			{
				od::GeodeBuilder gb(it->lodName, this->getAssetProvider());
				gb.setBuildSmoothNormals(mShadingType != ModelShadingType::Flat);
				gb.setClampTextures(false);

				// the count fields in the mesh info sometimes do not cover all vertices and polygons. gotta be something with those "LOD caps"
				//  instead of using those values, use all vertices up until the next lod until we figure out how else to handle this
				size_t actualVertexCount = ((it+1 == mLodMeshInfos.end()) ? mVertices.size() : (it+1)->firstVertexIndex) - it->firstVertexIndex;
				size_t actualPolyCount = ((it+1 == mLodMeshInfos.end()) ? mPolygons.size() : (it+1)->firstPolygonIndex) - it->firstPolygonIndex;

				auto verticesBegin = mVertices.begin() + it->firstVertexIndex;
				auto verticesEnd = mVertices.begin() + actualVertexCount + it->firstVertexIndex;
				gb.setVertexVector(verticesBegin, verticesEnd);

				auto polygonsBegin = mPolygons.begin() + it->firstPolygonIndex;
				auto polygonsEnd = mPolygons.begin() + actualPolyCount + it->firstPolygonIndex;
				gb.setPolygonVector(polygonsBegin, polygonsEnd);

				auto bonesBegin = it->boneAffections.begin();
				auto bonesEnd = it->boneAffections.end();
				gb.setBoneAffectionVector(bonesBegin, bonesEnd);

				osg::ref_ptr<osg::Geode> newGeode(new osg::Geode);
				gb.build(newGeode);

				mCalculatedBoundingBox.expandBy(newGeode->getBoundingBox());

				float minDistance = it->distanceThreshold;
				float maxDistance = ((it+1) == mLodMeshInfos.end()) ? std::numeric_limits<float>::max() : (it+1)->distanceThreshold;
				lodNode->addChild(newGeode, minDistance, maxDistance);
			}

			this->addChild(lodNode);

		}else
		{
			od::GeodeBuilder gb(mModelName, this->getAssetProvider());
			gb.setBuildSmoothNormals(mShadingType != ModelShadingType::Flat);
			gb.setClampTextures(false);
			gb.setVertexVector(mVertices.begin(), mVertices.end());
			gb.setPolygonVector(mPolygons.begin(), mPolygons.end());

			osg::ref_ptr<osg::Geode> newGeode(new osg::Geode);
			gb.build(newGeode);

			mCalculatedBoundingBox.expandBy(newGeode->getBoundingBox());

			this->addChild(newGeode);
		}

		osg::StateSet *ss = this->getOrCreateStateSet();

        // model faces are oriented CW for some reason
        ss->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE), osg::StateAttribute::ON);

        osg::ref_ptr<osg::Program> modelProgram = shaderManager.makeProgram("model");
        ss->setAttribute(modelProgram, osg::StateAttribute::ON);

        if(mShadingType != ModelShadingType::None)
        {
            ss->setDefine("LIGHTING", osg::StateAttribute::ON);

            if(mShiny)
            {
                ss->setDefine("SPECULAR", osg::StateAttribute::ON);
            }
        }

        // if model is a character, bind the vertex attribute locations, but don't yet activate the RIGGING define.
        //  doing so would cause characters without an attached SkeletonAnimationPlayer to glitch because
        //  the shader would expect bone data to be uploaded but only receive all-zero-matrices.
        if(isCharacter())
        {
            modelProgram->addBindAttribLocation("influencingBones", OD_ATTRIB_INFLUENCE_LOCATION);
            modelProgram->addBindAttribLocation("vertexWeights", OD_ATTRIB_WEIGHT_LOCATION);
        }

        mGeometryBuilt = true;
	}
}

