#pragma once

#include "IFileManager.h"
#include <android/asset_manager.h> 

class FileManagerAndroid : public IFileManager
{
private:
	
public:
	void init(AAssetManager * asset);

	AAssetManager *getAssetManager();

	vector<string> getFilesFromResource();

	vector<string> getFilesFromFolder(string folder, string suffix = "");

	string readTextFile(const char * filename);

	char* readBinaryFile(const char * filename, size_t& size);

	bool exists(const char * filename);

	IFile* open(string filename);
};

