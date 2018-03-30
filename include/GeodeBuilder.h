/*
 * GeodeBuilder.h
 *
 *  Created on: 26 Mar 2018
 *      Author: zal
 */

#ifndef INCLUDE_GEODEBUILDER_H_
#define INCLUDE_GEODEBUILDER_H_

#include <vector>
#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osg/Vec4i>
#include <osg/Geode>

#include "db/Asset.h"

namespace od
{

	class AssetProvider;


	struct Polygon
	{
		size_t vertexCount;
		size_t vertexIndices[4];
		osg::Vec2f uvCoords[4];
		bool doubleSided;
		AssetRef texture;
	};

	struct BoneAffection
	{
		size_t jointIndex;
		size_t vertexIndex;
		float vertexWeight;
	};

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

		GeodeBuilder(const std::string &modelName, AssetProvider &assetProvider);

		void setVertexVector(std::vector<osg::Vec3f>::iterator begin, std::vector<osg::Vec3f>::iterator end);
		void setPolygonVector(std::vector<Polygon>::iterator begin, std::vector<Polygon>::iterator end);
		void setBoneAffectionVector(std::vector<BoneAffection>::iterator begin, std::vector<BoneAffection>::iterator end);
		void setClampTextures(bool b) { mClampTextures = b; }

		void build(osg::Geode *geode);


	private:

		struct Triangle
		{
			size_t vertexIndices[3];
			osg::Vec2f uvCoords[3];
			AssetRef texture;
		};

		void _buildNormals();
		void _disambiguateAndGenerateUvs();

		std::string mModelName;
		AssetProvider &mAssetProvider;

		// arrays for the shared VBO. IBOs are uniquely generated per geometry.
		osg::ref_ptr<osg::Vec3Array> mVertices;
		osg::ref_ptr<osg::Vec3Array> mNormals;
		osg::ref_ptr<osg::Vec2Array> mUvCoords;
		osg::ref_ptr<osg::Vec4Array> mBoneWeights;
		osg::ref_ptr<osg::Vec4Array> mBoneIndices; // NOTE: this was an unsigned int vector before. turns out OSG somehow fails
												   //  to upload that to the shader. or maybe the shader can't handle it. anyhow
												   //  it took me like 10 hours to debug that shit. don't change it!
		bool mClampTextures;
		std::vector<Triangle> mTriangles;
	};

}

#endif /* INCLUDE_GEODEBUILDER_H_ */
