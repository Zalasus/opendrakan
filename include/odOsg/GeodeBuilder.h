/*
 * GeodeBuilder.h
 *
 *  Created on: 26 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_GEODEBUILDER_H_
#define INCLUDE_GEODEBUILDER_H_

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osg/Vec4i>
#include <osg/Geode>

#include <odCore/db/Asset.h>
#include <odCore/db/Model.h>

namespace odDb
{
	class AssetProvider;
}

namespace odOsg
{

	/**
	 * Class for constructing geodes from Riot engine model data. It splits models into multiple segments
	 * with only one texture per segment. VBOs are shared among all segments and vertices are reused where possible.
	 * Automatically generates normals and duplicates vertices if neccessary.
	 *
	 * Construction is O(n*log(n)).
	 */
	class GeodeBuilder
	{
	public:

		GeodeBuilder(const std::string &modelName, odDb::AssetProvider &assetProvider);

		inline void setBuildSmoothNormals(bool b) { mSmoothNormals = b; }
		inline void setModelColor(const osg::Vec4 &color) { mColors->at(0) = color; }
		inline void setNormalsFromCcw(bool b) { mNormalsFromCcw = b; }
		inline osg::Vec3Array *getBuiltVertexArray() { return mVertices; }
		inline osg::Vec3Array *getBuiltNormalArray() { return mNormals; }
		inline osg::Vec4Array *getBuiltColorArray() { return mColors; }

		void setVertexVector(std::vector<glm::vec3>::iterator begin, std::vector<glm::vec3>::iterator end);
		void setPolygonVector(std::vector<odDb::Model::Polygon>::iterator begin, std::vector<odDb::Model::Polygon>::iterator end);
		void setBoneAffectionVector(std::vector<odDb::Model::BoneAffection>::iterator begin, std::vector<odDb::Model::BoneAffection>::iterator end);
		void setClampTextures(bool b) { mClampTextures = b; }

		void build(osg::Geode *geode);


	private:

		struct Triangle
		{
			size_t vertexIndices[3];
			glm::vec2 uvCoords[3];
			odDb::AssetRef texture;
		};

		void _buildNormals();
		void _makeIndicesUniqueAndGenerateUvs();
		void _disambiguateAndGenerateUvs();

		std::string mModelName;
		odDb::AssetProvider &mAssetProvider;

		bool mClampTextures;
		bool mSmoothNormals;
		bool mNormalsFromCcw;

		std::vector<Triangle> mTriangles;

		std::vector<glm::vec3> mVertices;
		std::vector<glm::vec3> mNormals;
		std::vector<glm::vec4> mColors;
		std::vector<glm::vec2> mUvCoords;

		bool mHasBoneInfo;
		std::vector<glm::vec4> mBoneIndices;
		std::vector<glm::vec4> mBoneWeights;
	};

}

#endif /* INCLUDE_GEODEBUILDER_H_ */
