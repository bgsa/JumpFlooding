#pragma once

#include <string>
#include <android/asset_manager.h> 

#include "IFile.h"

using namespace std;

class FileAndroid : public IFile
{
private:
	AAsset* asset;

public:
	FileAndroid(string filename);

	void read(void* buffer, size_t size, size_t count = 1);
	void seek(long offset);
	void close();

	~FileAndroid();
};

