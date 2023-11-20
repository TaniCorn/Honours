#include "AModel.h"

AModel::AModel(ID3D11Device* ldevice, const std::string& file)
{
	device = ldevice;
	importModel(file);
}

AModel::~AModel()
{

}

void AModel::initBuffers(ID3D11Device* device)
{
	
}

void AModel::importModel(const std::string& pFile)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType|
		aiProcess_MakeLeftHanded|
		aiProcess_FlipUVs);
	// If the import failed, report it
	/*if (!scene)
	{
		DoTheErrorLogging(importer.GetErrorString());
		return false;
	}*/
	// Now we can access the file's contents.
	//modelProcessing(scene);#

	if (scene)
	{
		processNode(scene->mRootNode, scene);
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* (int)vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* (int)indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	//delete vertices;
	//vertices = 0;

	//delete indices;
	//indices = 0;
	vertexCount = (int)vertices.size();
	indexCount = (int)indices.size();

	//vertices.clear();
	//indices.clear();
}

void AModel::modelProcessing(const aiScene* scene)
{
	////std::vector<VertexType> vertices;
	////std::vector<unsigned long> indices;
	//
	//// 6 vertices per quad, res*res is face, times 6 for each face
	////vertexCount = ((6 * resolution)*resolution) * 6;

	////indexCount = vertexCount;

	//// Create the vertex and index array.
	////vertices = new VertexType[vertexCount];
	////indices = new unsigned long[indexCount];

	//for (UINT i = 0; i < scene->mRootNode->mNumMeshes; i++)
	//{
	//	//aiMesh** meshes = scene->mMeshes;
	//	aiMesh* mesh = scene->mMeshes[scene->mRootNode->mMeshes[i]];
	//	// Walk through each of the mesh's vertices
	//	for (UINT i = 0; i < mesh->mNumVertices; i++)
	//	{
	//		XMFLOAT3 vert;
	//		XMFLOAT2 text;
	//		XMFLOAT3 norm;

	//		vert.x = mesh->mVertices[i].x;
	//		vert.y = mesh->mVertices[i].y;
	//		vert.z = mesh->mVertices[i].z;

	//		if (mesh->HasTextureCoords(i))
	//		{
	//			text.x = (float)mesh->mTextureCoords[0][i].x;
	//			text.y = (float)mesh->mTextureCoords[0][i].y;
	//		}

	//		if (mesh->HasNormals())
	//		{
	//			norm.x = mesh->mNormals[i].x;
	//			norm.y = mesh->mNormals[i].y;
	//			norm.z = mesh->mNormals[i].z;
	//		}

	//		VertexType vertex;
	//		vertex.position = vert;
	//		vertex.texture = text;
	//		vertex.normal = norm;
	//		vertices.push_back(vertex);
	//	}

	//	for (UINT i = 0; i < mesh->mNumFaces; i++)
	//	{
	//		aiFace face = mesh->mFaces[i];

	//		for (UINT j = 0; j < face.mNumIndices; j++)
	//			indices.push_back(face.mIndices[j]);
	//	}
	//}


	//// Set up the description of the static vertex buffer.
	//D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	//D3D11_SUBRESOURCE_DATA vertexData, indexData;
	//vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//vertexBufferDesc.ByteWidth = sizeof(VertexType)* (int)vertices.size();
	//vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vertexBufferDesc.CPUAccessFlags = 0;
	//vertexBufferDesc.MiscFlags = 0;
	//vertexBufferDesc.StructureByteStride = 0;
	//// Give the subresource structure a pointer to the vertex data.
	//vertexData.pSysMem = vertices.data();
	//vertexData.SysMemPitch = 0;
	//vertexData.SysMemSlicePitch = 0;
	//// Now create the vertex buffer.
	//device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	//// Set up the description of the static index buffer.
	//indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//indexBufferDesc.ByteWidth = sizeof(unsigned long)* (int)indices.size();
	//indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//indexBufferDesc.CPUAccessFlags = 0;
	//indexBufferDesc.MiscFlags = 0;
	//indexBufferDesc.StructureByteStride = 0;
	//// Give the subresource structure a pointer to the index data.
	//indexData.pSysMem = indices.data();
	//indexData.SysMemPitch = 0;
	//indexData.SysMemSlicePitch = 0;
	//// Create the index buffer.
	//device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	//// Release the arrays now that the vertex and index buffers have been created and loaded.
	////delete vertices;
	////vertices = 0;

	////delete indices;
	////indices = 0;
	//indexCount = (int)indices.size();
}

void AModel::processScene(const aiScene* scene)
{

}
void AModel::processNode(const aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		//meshes.push_back(this->processMesh(mesh, scene));
		processMesh(mesh, scene);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}
void AModel::processMesh(const aiMesh* mesh, const aiScene* scene)
{
	/*for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.X = mesh->mVertices[i].x;
		vertex.Y = mesh->mVertices[i].y;
		vertex.Z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}*/

	//---------------------------------

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		XMFLOAT3 vert;
		XMFLOAT2 text;
		XMFLOAT3 norm;

		vert.x = mesh->mVertices[i].x;
		vert.y = mesh->mVertices[i].y;
		vert.z = mesh->mVertices[i].z;

		if (mesh->HasTextureCoords(0))
		{
			text.x = (float)mesh->mTextureCoords[0][i].x;
			text.y = (float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->HasNormals())
		{
			norm.x = mesh->mNormals[i].x;
			norm.y = mesh->mNormals[i].y;
			norm.z = mesh->mNormals[i].z;
		}

		VertexType vertex;
		vertex.position = vert;
		vertex.texture = text;
		vertex.normal = norm;
		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
}

//vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName, const aiScene * scene)
//{
//	vector<Texture> textures;
//	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
//	{
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//		bool skip = false;
//		for (UINT j = 0; j < textures_loaded.size(); j++)
//		{
//			if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
//			{
//				textures.push_back(textures_loaded[j]);
//				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
//				break;
//			}
//		}
//		if (!skip)
//		{   // If texture hasn't been loaded already, load it
//			HRESULT hr;
//			Texture texture;
//			if (textype == "embedded compressed texture")
//			{
//				int textureindex = getTextureIndex(&str);
//				texture.texture = getTextureFromModel(scene, textureindex);
//			}
//			else
//			{
//				string filename = string(str.C_Str());
//				filename = directory + '/' + filename;
//				wstring filenamews = wstring(filename.begin(), filename.end());
//				hr = CreateWICTextureFromFile(dev, devcon, filenamews.c_str(), nullptr, &texture.texture);
//				if (FAILED(hr))
//					MessageBox(hwnd, "Texture couldn't be loaded", "Error!", MB_ICONERROR | MB_OK);
//			}
//			texture.type = typeName;
//			texture.path = str.C_Str();
//			textures.push_back(texture);
//			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
//		}
//	}
//	return textures;
//}