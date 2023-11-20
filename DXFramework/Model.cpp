// Model mesh and load
// Loads a .obj and creates a mesh object from the data
#include "model.h"

// load model datat, initialise buffers (with model data) and load texture.
Model::Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename)
{
	loadModel(filename);
	initBuffers(device);
}

// Release resources.
Model::~Model()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();

	if (model)
	{
		delete[] model;
		model = 0;
	}
}


// Initialise buffers with model data.
void Model::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
		
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];
	
	// Load the vertex array and index array with data.
	for (int i = 0; i<vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(model[i].x, model[i].y, -model[i].z);
		vertices[i].texture = XMFLOAT2(model[i].tu, model[i].tv);
		vertices[i].normal = XMFLOAT3(model[i].nx, model[i].ny, -model[i].nz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	
	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

//// Read model file and parse data.
//void Model::loadModel(WCHAR* filename)
//{
//	// Process model file
//	std::ifstream fileStream;
//	int fileSize = 0;
//
//	fileStream.open(filename, std::ifstream::in);
//
//	if (fileStream.is_open() == false)
//		MessageBox(NULL, filename, L"Missing Model File", MB_OK);
//
//	fileStream.seekg(0, std::ios::end);
//	fileSize = (int)fileStream.tellg();
//	fileStream.seekg(0, std::ios::beg);
//
//	if (fileSize <= 0)
//		MessageBox(NULL, filename, L"Model file empty", MB_OK);
//
//	char *buffer = new char[fileSize];
//
//	if (buffer == 0)
//		MessageBox(NULL, filename, L"Model buffer is to small", MB_OK);
//
//	memset(buffer, '\0', fileSize);
//
//	TokenStream tokenStream, lineStream, faceStream;
//	std::string tempLine, token;
//
//	fileStream.read(buffer, fileSize);
//	tokenStream.SetTokenStream(buffer);
//
//	delete[] buffer;
//
//	tokenStream.ResetStream();
//
//	std::vector<float> verts, norms, texC;
//	std::vector<int> faces;
//
//
//	char lineDelimiters[2] = { '\n', ' ' };
//
//	while (tokenStream.MoveToNextLine(&tempLine))
//	{
//		lineStream.SetTokenStream((char*)tempLine.c_str());
//		tokenStream.GetNextToken(0, 0, 0);
//
//		if (!lineStream.GetNextToken(&token, lineDelimiters, 2))
//			continue;
//
//		if (strcmp(token.c_str(), "v") == 0)
//		{
//			lineStream.GetNextToken(&token, lineDelimiters, 2);
//			verts.push_back((float)atof(token.c_str()));
//
//			lineStream.GetNextToken(&token, lineDelimiters, 2);
//			verts.push_back((float)atof(token.c_str()));
//
//			lineStream.GetNextToken(&token, lineDelimiters, 2);
//			verts.push_back((float)atof(token.c_str()));
//		}
//		else if (strcmp(token.c_str(), "vn") == 0)
//		{
//			lineStream.GetNextToken(&token, lineDelimiters, 2);
//			norms.push_back((float)atof(token.c_str()));
//
//			lineStream.GetNextToken(&token, lineDelimiters, 2);
//			norms.push_back((float)atof(token.c_str()));
//
//			lineStream.GetNextToken(&token, lineDelimiters, 2);
//			norms.push_back((float)atof(token.c_str()));
//		}
//		else if (strcmp(token.c_str(), "vt") == 0)
//		{
//			lineStream.GetNextToken(&token, lineDelimiters, 2);
//			texC.push_back((float)atof(token.c_str()));
//
//			lineStream.GetNextToken(&token, lineDelimiters, 2);
//			texC.push_back((float)atof(token.c_str()));
//		}
//		else if (strcmp(token.c_str(), "f") == 0)
//		{
//			char faceTokens[3] = { '\n', ' ', '/' };
//			std::string faceIndex;
//
//			faceStream.SetTokenStream((char*)tempLine.c_str());
//			faceStream.GetNextToken(0, 0, 0);
//
//			for (int i = 0; i < 3; i++)
//			{
//				faceStream.GetNextToken(&faceIndex, faceTokens, 3);
//				faces.push_back((int)atoi(faceIndex.c_str()));
//
//				faceStream.GetNextToken(&faceIndex, faceTokens, 3);
//				faces.push_back((int)atoi(faceIndex.c_str()));
//
//				faceStream.GetNextToken(&faceIndex, faceTokens, 3);
//				faces.push_back((int)atoi(faceIndex.c_str()));
//			}
//		}
//		else if (strcmp(token.c_str(), "#") == 0)
//		{
//			int a = 0;
//			int b = a;
//		}
//
//		token[0] = '\0';
//	}
//
//	// "Unroll" the loaded obj information into a list of triangles.
//
//	int vIndex = 0, nIndex = 0, tIndex = 0;
//	int numFaces = (int)faces.size() / 9;
//
//	//// Create the model using the vertex count that was read in.
//	vertexCount = numFaces * 3;
//	model = new ModelType[vertexCount];
//	
//	for (int f = 0; f < (int)faces.size(); f += 3)
//	{
//		model[vIndex].x = verts[(faces[f + 0] - 1) * 3 + 0];
//		model[vIndex].y = verts[(faces[f + 0] - 1) * 3 + 1];
//		model[vIndex].z = verts[(faces[f + 0] - 1) * 3 + 2];
//		model[vIndex].tu = texC[(faces[f + 1] - 1) * 2 + 0];
//		model[vIndex].tv = texC[(faces[f + 1] - 1) * 2 + 1];
//		model[vIndex].nx = norms[(faces[f + 2] - 1) * 3 + 0];
//		model[vIndex].ny = norms[(faces[f + 2] - 1) * 3 + 1];
//		model[vIndex].nz = norms[(faces[f + 2] - 1) * 3 + 2];
//
//		//increase index count
//		vIndex++;
//
//	}
//	indexCount = vIndex;
//
//	verts.clear();
//	norms.clear();
//	texC.clear();
//	faces.clear();
//}

// Modified from a mulit-threaded version by Mark Ropper (CGT).
void Model::loadModel(const char* filename)
{
	std::vector<XMFLOAT3> verts;
	std::vector<XMFLOAT3> norms;
	std::vector<XMFLOAT2> texCs;
	std::vector<unsigned int> faces;

	FILE* file;// = fopen(filename, "r");
	errno_t err;
	err = fopen_s(&file, filename, "r");
	if(err != 0)
	//if (file == NULL)
	{
		return;
	}

	while (true)
	{
		char lineHeader[128];

		// Read first word of the line
		int res = fscanf_s(file, "%s", lineHeader, (int)sizeof(lineHeader));
		if (res == EOF)
		{
			break; // exit loop
		}
		else // Parse
		{
			if (strcmp(lineHeader, "v") == 0) // Vertex
			{
				XMFLOAT3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verts.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) // Tex Coord
			{
				XMFLOAT2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				texCs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) // Normal
			{
				XMFLOAT3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				norms.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) // Face
			{
				unsigned int face[9];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &face[0], &face[1], &face[2],
																			&face[3], &face[4], &face[5],
																			&face[6], &face[7], &face[8]);
				if (matches != 9)
				{
					// Parser error, or not triangle faces
					return;
				}

				for (int i = 0; i < 9; i++)
				{
					faces.push_back(face[i]);
				}


			}
		}
	}

	int vIndex = 0, nIndex = 0, tIndex = 0;
	int numFaces = (int)faces.size() / 9;

	//// Create the model using the vertex count that was read in.
	vertexCount = numFaces * 3;
	model = new ModelType[vertexCount];

	// "Unroll" the loaded obj information into a list of triangles.
	for (int f = 0; f < (int)faces.size(); f += 3)
	{
		model[vIndex].x = verts[(faces[f + 0] - 1)].x;
		model[vIndex].y = verts[(faces[f + 0] - 1)].y;
		model[vIndex].z = verts[(faces[f + 0] - 1)].z;
		model[vIndex].tu = texCs[(faces[f + 1] - 1)].x;
		model[vIndex].tv = texCs[(faces[f + 1] - 1)].y;
		model[vIndex].nx = norms[(faces[f + 2] - 1)].x;
		model[vIndex].ny = norms[(faces[f + 2] - 1)].y;
		model[vIndex].nz = norms[(faces[f + 2] - 1)].z;

		//increase index count
		vIndex++;

	}
	indexCount = vIndex;


	verts.clear();
	norms.clear();
	texCs.clear();
	faces.clear();

}
