//#include "assimp/scene.h"

#include "pch.h"
#include "ObjLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "VertexPositionNormalBinormalTangentColorTex.h"
#include "Game.h"

#include "TextureLoader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyObjLoader.h"


using namespace DirectX::SimpleMath;


struct TinyModelInfo
{
	ID3D11Buffer* vBuf;
	ID3D11Buffer* nBuf;
	ID3D11Buffer* tBuf;
	ID3D11Buffer* StrBuf;
	TinyMaterial* Materials;
	TinyShape* Shapes;
	int shapesCount;
	int materialCount;

	void AddRefs()
	{
		if (vBuf) vBuf->AddRef();
		if (nBuf) nBuf->AddRef();
		if (tBuf) tBuf->AddRef();
		if (StrBuf) StrBuf->AddRef();

		if (!Materials) return;
		for (int i = 0; i < materialCount; ++i)
		{
			Materials[i].DiffSRV->AddRef();
			Materials[i].DiffuseTexture->AddRef();
		}
	}
};

std::unordered_map<std::string, TinyModelInfo> TinyModelsCache = {};


// trim from start (in place)
static void ltrim(std::wstring& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
		}));
}


std::vector<std::wstring> tokenize(std::wstring& str, wchar_t delim)
{
	size_t start;
	size_t end = 0;

	std::vector<std::wstring> out;

	while ((start = str.find_first_not_of(delim, end)) != std::wstring::npos) {
		end = str.find(delim, start);
		out.emplace_back( str.substr(start, end - start));
	}
	
	return out;
}


Vector4 ParsePosition(std::vector<std::wstring>& words)
{
	float x = std::stof(words[1]);
	float y = std::stof(words[2]);
	float z = std::stof(words[3]);

	float w = 1.0f;

	if(words.size() > 4)
		w = std::stof(words[4]);

	return Vector4(x, y, z, w);
}

Vector4 ParseTex(std::vector<std::wstring>& words)
{
	auto u = std::stof(words[1]);
	auto v = std::stof(words[2]);

	return Vector4(u, v, 0, 0);
}

Vector4 ParseNormal(std::vector<std::wstring>& words)
{
	auto x = std::stof(words[1]);
	auto y = std::stof(words[2]);
	auto z = std::stof(words[3]);

	auto normal = Vector4(x, y, z, 0);
	normal.Normalize();

	return normal;
}

void ParseFaceElement(std::wstring& elem, int& posInd, int& texInd, int& norInd)
{
	posInd = texInd = norInd = -1;

	auto indeces = tokenize(elem, '/');

	
	if (indeces.size() == 0) {
		return;
	}

	if (indeces.size() == 1) { // Position only
		posInd = std::stoi(indeces[0]) - 1;
	}
	if (indeces.size() == 2) { // Position and texture coordinates provided
		posInd = std::stoi(indeces[0]) - 1;
		texInd = std::stoi(indeces[1]) - 1;
	}
	if (indeces.size() == 3) { // Position, texture coordinates (maybe) and normal provided
		posInd = std::stoi(indeces[0]) - 1;
		if (!indeces[1].empty()) texInd = std::stoi(indeces[1]) - 1;
		norInd = std::stoi(indeces[2]) - 1;
	}
}

VertexPositionNormalBinormalTangentColorTex GenerateVertex(std::wstring& elem, std::vector<Vector4>& positions, std::vector<Vector4>& texCoords, std::vector<Vector4>& normals)
{
	int p, t, n;
	ParseFaceElement(elem, p, t, n);
	VertexPositionNormalBinormalTangentColorTex v = {};
	v.Position	= positions[p];
	v.Normal	= n == -1 ? Vector4::Zero : normals[n];
	v.Tex		= t == -1 ? Vector4::Zero : texCoords[t];

	return v;
}



ObjLoader::ObjLoader(Game* inGame)
{
	game = inGame;

	DefaultMaterial.TexName = "DefaultDiffuseMap.jpg";

	const auto texFile = L"./content/DefaultDiffuseMap.jpg";
	game->TextureLoader->LoadTextureFromFile(texFile, DefaultMaterial.DiffuseTexture, DefaultMaterial.DiffSRV, false, false);
}

void ObjLoader::LoadObjModel(LPCWSTR fileName, ID3D11Buffer*& vBuffer, int& vCount)
{
	vBuffer = nullptr;
	vCount = 0;
	VertexPositionNormalBinormalTangentColorTex* vertices;
	
	LoadVertices(fileName, vertices, vCount);

	if (vCount == 0) return;

	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;
	bufDesc.ByteWidth = VertexPositionNormalBinormalTangentColorTex::Stride * vCount;

	D3D11_SUBRESOURCE_DATA positionsData = {};
	positionsData.pSysMem = vertices;
	positionsData.SysMemPitch = 0;
	positionsData.SysMemSlicePitch = 0;

	game->Device->CreateBuffer(&bufDesc, &positionsData, &vBuffer);

	delete[] vertices;
}



void ObjLoader::LoadVertices(LPCWSTR fileName, VertexPositionNormalBinormalTangentColorTex*& vertices, int& vCount) const
{
	auto positions	= new std::vector<Vector4>();
	auto normals	= new std::vector<Vector4>();
	auto texCoords	= new std::vector<Vector4>();

	auto verticesVec = std::vector<VertexPositionNormalBinormalTangentColorTex>();

	std::ifstream file(fileName);
	//std::string line;

	std::wstring str;

	file.seekg(0, std::ios::end);
	str.reserve(file.tellg());
	file.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());


	std::vector<std::wstring> lines;
	int start = 0, stop = 1;

	while(stop < str.size())
	{
		if(str[stop] == '\n')
		{
			lines.emplace_back(str.substr(start, stop - start));
			start = stop;
		}

		stop++;
	}
	
	for(auto& line : lines)
	{
		//if(line.empty()) continue;

		ltrim(line); // Trim start

		if (line.empty()) continue;

		if (line[0] == L'#') continue;

		std::vector<std::wstring> vecStr;
		std::wistringstream ss(line);
		std::wstring word;
		while (ss >> word)
		{
			vecStr.emplace_back(word);
		}

		if(vecStr[0] == L"v")
		{
			positions->emplace_back(ParsePosition(vecStr));
		}
		else if (vecStr[0] == L"vt")
		{
			texCoords->emplace_back(ParseTex(vecStr));
		}
		else if (vecStr[0] == L"vn")
		{
			normals->emplace_back(ParseNormal(vecStr));
		}
		else if (vecStr[0] == L"f")
		{
			if (vecStr.size() < 4) continue;

			auto v0 = GenerateVertex(vecStr[1], *positions, *texCoords, *normals);

			for (int fInd = 2; fInd < vecStr.size() - 1; fInd++) {
				auto v1 = GenerateVertex(vecStr[fInd],	*positions, *texCoords, *normals);
				auto v2 = GenerateVertex(vecStr[fInd + 1], *positions, *texCoords, *normals);

				verticesVec.emplace_back(v0);
				verticesVec.emplace_back(v1);
				verticesVec.emplace_back(v2);
			}
		}
		
	}

	vCount = verticesVec.size();
	vertices = new VertexPositionNormalBinormalTangentColorTex[vCount];
	std::copy(verticesVec.begin(), verticesVec.end(), vertices);
	//for (int i = 0; i < verticesVec.size(); i++)
	//	vertices1[i] = verticesVec[i];

	delete positions;
	delete normals;
	delete texCoords;
}



void ObjLoader::LoadTinyModel(const char* fileName, 
	ID3D11Buffer*& vBuf, 
	ID3D11Buffer*& nBuf, 
	ID3D11Buffer*& tBuf, 
	ID3D11Buffer*& strBuf, 
	TinyMaterial*& outMaterials, 
	TinyShape*& outShapes, 
	int& shapesCount)
{
	auto fileNameStr = std::string(fileName);
	if(TinyModelsCache.contains(fileNameStr))
	{
		auto& model = TinyModelsCache[fileNameStr];
		model.AddRefs();

		vBuf			= model.vBuf;
		nBuf			= model.nBuf;
		tBuf			= model.tBuf;
		strBuf			= model.StrBuf;
		outMaterials	= model.Materials;
		outShapes		= model.Shapes;
		shapesCount = model.shapesCount;

		std::cout << "Model loaded from cache: " << fileNameStr << "\n";
		return;
	}


	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	std::string directory;
	std::string fName(fileName);
	
	const size_t last_slash_idx = fName.rfind('/');
	if (std::string::npos != last_slash_idx) {
		directory = fName.substr(0, last_slash_idx);
	}
	
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName, directory.c_str());

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	D3D11_BUFFER_DESC constBufDesc = {};
	constBufDesc.Usage = D3D11_USAGE_IMMUTABLE;
	constBufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	constBufDesc.CPUAccessFlags = 0;
	constBufDesc.MiscFlags = 0;
	constBufDesc.StructureByteStride = 0;
	constBufDesc.ByteWidth = sizeof(float) * attrib.vertices.size();

	D3D11_SUBRESOURCE_DATA resData = {};
	resData.pSysMem				= &attrib.vertices[0];
	resData.SysMemPitch			= 0;
	resData.SysMemSlicePitch	= 0;

	game->Device->CreateBuffer(&constBufDesc, &resData, &vBuf);


	constBufDesc.ByteWidth = sizeof(float) * attrib.normals.size();
	resData.pSysMem = &attrib.normals[0];

	game->Device->CreateBuffer(&constBufDesc, &resData, &nBuf);


	constBufDesc.ByteWidth = sizeof(float) * attrib.texcoords.size();
	resData.pSysMem = &attrib.texcoords[0];

	game->Device->CreateBuffer(&constBufDesc, &resData, &tBuf);


	shapesCount = shapes.size();
	outShapes = new	TinyShape[shapes.size()];
	std::vector< tinyobj::index_t> indexes;
	int ind = 0;
	for (auto& shape : shapes) {

		outShapes[ind].StartIndex	= indexes.size();
		outShapes[ind].Count		= shape.mesh.indices.size();
		outShapes[ind].MaterialInd = shape.mesh.material_ids[0];
		
		indexes.insert(std::end(indexes), std::begin(shape.mesh.indices), std::end(shape.mesh.indices));
		
		++ind;
	}

	constBufDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	constBufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	constBufDesc.Usage = D3D11_USAGE_DEFAULT;
	constBufDesc.StructureByteStride = sizeof(tinyobj::index_t);
	constBufDesc.ByteWidth = sizeof(tinyobj::index_t) * indexes.size();
	resData.pSysMem = &indexes[0];

	game->Device->CreateBuffer(&constBufDesc, &resData, &strBuf);

	outMaterials = new TinyMaterial[materials.size()];

	for (UINT i = 0; i < materials.size(); i++) {
		outMaterials[i].TexName = materials[i].diffuse_texname.c_str();

		std::string diffName = materials[i].diffuse_texname;
		if (diffName.empty()) diffName = "DefaultDiffuseMap.jpg";

		auto texFile = (directory + "/" + diffName);
		std::wstring stemp = std::wstring(texFile.begin(), texFile.end());
		
		game->TextureLoader->LoadTextureFromFile(stemp.c_str(), outMaterials[i].DiffuseTexture, outMaterials[i].DiffSRV, false, true);
	}

	TinyModelsCache[fileNameStr] = TinyModelInfo{vBuf, nBuf, tBuf, strBuf, outMaterials, outShapes, shapesCount, static_cast<int>(materials.size())};
}


void ObjLoader::GatherLoadResults(const char* fileName, float*& positions, int& positionsCount, int*& triangles, int& trianglesCount)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	std::string directory;
	std::string fName(fileName);

	const size_t last_slash_idx = fName.rfind('/');
	if (std::string::npos != last_slash_idx) {
		directory = fName.substr(0, last_slash_idx);
	}

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName, directory.c_str());


	int indecesCount = 0;
	for (auto& shape : shapes) {
		indecesCount += shape.mesh.indices.size();
		//indexes.insert(std::end(indexes), std::begin(shape.mesh.indices), std::end(shape.mesh.indices));
	}
	std::vector<int> indexes;
	indexes.reserve(indecesCount);
	for (auto& shape : shapes) {
		for(auto& i : shape.mesh.indices)
		{
			indexes.push_back(i.vertex_index);
		}
	}
	triangles = new int[indecesCount];
	memcpy(triangles, &indexes[0], indecesCount*4);
	trianglesCount = indecesCount / 3;


	positionsCount = attrib.vertices.size() / 3;
	size_t posByteSize = sizeof(float) * attrib.vertices.size();
	positions = new float[attrib.vertices.size()];
	memcpy(positions, &attrib.vertices[0], posByteSize);
}


