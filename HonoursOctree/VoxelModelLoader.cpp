#include "VoxelModelLoader.h"

VoxelModelLoader::VoxelModelLoader()
{
	mvoxModelLoader = new magicavoxel::VoxFile(true, true);
}

VoxelModelLoader::~VoxelModelLoader()
{
	delete mvoxModelLoader;
}

void VoxelModelLoader::loadModel(std::string name, std::string path)
{
	if (mVoxModels.find(name) == mVoxModels.end())
	{
		mvoxModelLoader->Load(path);
		mVoxModels[name] = mvoxModelLoader->sparseModels().size() - 1;
	}
}
std::vector<Voxel> VoxelModelLoader::getModelVoxels(std::string name)
{
	const magicavoxel::VoxSparseModel& sparseModel = mvoxModelLoader->sparseModels().at(mVoxModels[name]);// List of non-empty voxels
	const int amountOfVoxels = sparseModel.voxels().size();
	std::vector<Voxel> voxels;
	voxels.reserve(amountOfVoxels);
	//Voxel* voxels = new Voxel[amountOfVoxels];
	mvoxColPalette[name] = mvoxModelLoader->denseModels().at(0).palette();

	for (int i = 0; i < amountOfVoxels; i++)
	{
		uint8_t posX = sparseModel.voxels()[i].x;
		uint8_t posY = sparseModel.voxels()[i].z;
		uint8_t posZ = sparseModel.voxels()[i].y;
		uint8_t color = sparseModel.voxels()[i].color;
		//magicavoxel::Color col = mvoxColPalette.at(color);
		//XMFLOAT4 rgba = 

		Voxel vox;
		vox.color = color;
		vox.point.x = posX;
		vox.point.y = posY;
		vox.point.z = posZ;
		voxels.push_back(vox);
	}
	return voxels;
}

const magicavoxel::VoxSparseModel& VoxelModelLoader::getModel(std::string name)
{
	return mvoxModelLoader->sparseModels().at(mVoxModels[name]);// List of non-empty voxels
}

int VoxelModelLoader::getModelDimensions(std::string modelName)
{
	const magicavoxel::VoxSparseModel& sparseModel = mvoxModelLoader->sparseModels().at(mVoxModels[modelName]);


	int xRes = sparseModel.size().x;
	int yRes = sparseModel.size().y;
	int zRes = sparseModel.size().z;
	int res = max(xRes, yRes);
	res = max(res, zRes);
	int factorial = 0;
	//TODO: There has to be a quicker way of doing this. 
	//Find the next number that is higher than res and is a 2^x
	int exponent = ceil(log2(res));
	for (int i = 0; i < 10; i++)
	{
		int factorial = pow(2, i);
		if (factorial > res)
		{
			return factorial;
		}

	}
}

XMFLOAT3 VoxelModelLoader::getModelDimensionsExact(std::string modelName)
{
	const magicavoxel::VoxSparseModel& sparseModel = mvoxModelLoader->sparseModels().at(mVoxModels[modelName]);

	int xRes = sparseModel.size().x;
	int yRes = sparseModel.size().y;
	int zRes = sparseModel.size().z;
	return XMFLOAT3(xRes, yRes, zRes);
}

const magicavoxel::Color VoxelModelLoader::getColorFromPalette(std::string modelName, uint8_t colorIndex)
{
	return mvoxColPalette[modelName].at(colorIndex);
}

const magicavoxel::Palette VoxelModelLoader::getPalette(std::string modelName)
{
	return mvoxColPalette[modelName];
}

XMFLOAT4 VoxelModelLoader::getRGBAFromColor(magicavoxel::Color color)
{
	return XMFLOAT4(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}
