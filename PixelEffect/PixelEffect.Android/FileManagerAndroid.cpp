#include <android/asset_manager.h> 

#include "Log.h"
#include "FileManagerAndroid.h"
#include "StringHelper.h"
#include "FileAndroid.h"

static AAssetManager *assetManager;

void FileManagerAndroid::init(AAssetManager *asset) 
{
	assetManager = asset;
}

AAssetManager * FileManagerAndroid::getAssetManager() 
{
	return assetManager;
}

vector<string> FileManagerAndroid::getFilesFromResource()
{	
	vector<string> files;
	const char* filename = (const char*) NULL;
	
	AAssetDir* assetDir = AAssetManager_openDir(assetManager, "");	

	while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL)
		files.push_back(filename);

	AAssetDir_close(assetDir);
	
	return files;
}

vector<string> FileManagerAndroid::getFilesFromFolder(string folder, string suffix)
{
	vector<string> files;
	const char* filename = (const char*)NULL;

	AAssetDir* assetDir = AAssetManager_openDir(assetManager, folder.c_str());

	while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) 
	{
		if (suffix.empty())
			files.push_back(filename);
		else
			if (StringHelper::endWith(filename, suffix.c_str()))
				files.push_back(filename);
	}

	AAssetDir_close(assetDir);

	return files;
}

string FileManagerAndroid::readTextFile(const char * filename)
{
	AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_UNKNOWN);

	if (asset == NULL) 
	{
		Log::error("ASSET_NOT_FOUND: " + string(filename));
		return nullptr;
	}

	long size = AAsset_getLength(asset);

	char* content = (char*) malloc( sizeof(char)*size );

	AAsset_read(asset, content, size);	
	AAsset_close(asset);

	return string(content);
}

char* FileManagerAndroid::readBinaryFile(const char * filename, size_t& size) 
{
	AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_UNKNOWN);

	if (asset == NULL)
	{
		Log::error("ASSET_NOT_FOUND: " + string(filename));
		return nullptr;
	}

	long length = AAsset_getLength(asset);
	size = sizeof(char) * length;

	char* content = (char*) malloc(size);

	AAsset_read(asset, content, size);
	AAsset_close(asset);

	return content;
}

bool FileManagerAndroid::exists(const char * filename)
{
	return false;
}

IFile* FileManagerAndroid::open(string filename) 
{
	IFile * file = new FileAndroid(filename);

	return file;
}
