#include "FileAndroid.h"
#include "FileManagerAndroid.h"

FileAndroid::FileAndroid(string filename)
{
	FileManagerAndroid fileManager;
	AAssetManager * assetManager = fileManager.getAssetManager();

	asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
}

void FileAndroid::read(void* buffer, size_t size, size_t count)
{
	AAsset_read(asset, buffer, count * size);
}

void FileAndroid::seek(long offset)
{
	AAsset_seek(asset, offset, SEEK_SET);
}

void FileAndroid::close()
{
	if (asset != nullptr)
		AAsset_close(asset);
}

FileAndroid::~FileAndroid()
{
	close();
	asset = nullptr;
}
