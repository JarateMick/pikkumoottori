#if defined(_WIN32) || defined(__EMSCRIPTEN__) || defined(_LINUX) || defined(__linux)
// #include <string>
#include <stdio.h>
#include <stdlib.h>
// #include <fstream>
#else
#include <core/ResourceManager.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

// #include <stdlib.h>
// #include <core/Utils.h>
char* io_read_file(const char* path)
{
	char *result = 0;
	FILE *file = fopen(path, "r");
	if (file)
	{
		fseek(file, 0L, SEEK_END);
		size_t fileSize = ftell(file);

		fseek(file, 0L, SEEK_SET);
		result = malloc(fileSize + 1);

		fread(result, sizeof(char), fileSize, file);
		result[fileSize] = '\0';

		fclose(file);
	}
	else
	{

		printf("failed  %s path \n", path);
		ASSERT(0);
		// logging
	}
	printf("%s\n", result);
	return result;
}

#if 0


#if defined(_WIN32) || defined(__EMSCRIPTEN__) ||  defined(_LINUX)
char* ReadEntireFile(const char* path)
{
	char *result = 0;
	FILE *file = fopen(path, "r");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		size_t fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		result = (char *)malloc(fileSize + 1);
		fread(result, fileSize, 1, file);
		result[fileSize] = '\0';

		fclose(file);
	}
	else
	{

		printf("failed  %s path \n", path);
		ASSERT(false);
		// logging
	}
	printf("%s\n", result);
	return result;

	// size_t fileLength = AAsset_getLength(assetDescriptor);
}

char* ReadEntireFile(const char* path, int* size)
{
	char *result = 0;
	FILE *file = fopen(path, "r");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		size_t fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		result = (char *)malloc(fileSize + 1);
		fread(result, fileSize, 1, file);
		result[fileSize] = '\0';

		*size = (int)fileSize;
		fclose(file);
	}
	else
	{
		printf("failed  %s path \n", path);
		ASSERT(false);
		// logging
	}
	// printf("%s\n", result);
	return result;
}



#ifndef DLL_FIX
std::string ReadEntireFileToString(const char* path)
{
#if 0
	char* buffer;

	FILE *file = fopen(path, "r");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		size_t fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		buffer = (char *)malloc(fileSize + 1);
		fread(buffer, fileSize, 1, file);
		buffer[fileSize] = '\0';

		fclose(file);
	}
	else
	{
		ASSERT(false);
		// logging
		return {};
	}

	std::string result(buffer);
	free(buffer);
	return result;
#else
	std::ifstream ifile(path);
	// std::string filetext;
	//while (ifile.good()) {
	//	std::string line;
	//	std::getline(ifile, line);
	//	filetext.append(line + "\n");
	//}

	std::FILE *fp = std::fopen(path, "rb");
	if (fp)
	{
		std::string contents;
		std::fseek(fp, 0, SEEK_END);
		contents.resize(std::ftell(fp));
		std::rewind(fp);
		std::fread(&contents[0], 1, contents.size(), fp);
		std::fclose(fp);
		return(contents);
	}
	else
	{
		LOGI("CAN'T read file %s \n", path);
		ASSERT(false);
	}
	return { "lol" };
#endif
}
#endif


void WriteBufferToFile(const char* path, const char* buffer, size_t bufferSize)
{
	char *result = 0;
	FILE *file = fopen(path, "w");
	if (file)
	{
		fwrite(buffer, bufferSize, 1, file);
		fclose(file);
	}
	else
	{
		ASSERT(false);
	}

	// Resource
}

#else // _WIN32 android
char* ReadEntireFile(const char* path)
{
	AAssetManager* mgr = ResourceManager::assetManager;

	char *result = 0;
	FILE *file = fopen(path, "r");

	// AAsset* file = AssetManager_open(mgr, path, )


	AAsset* assetDescriptor = AAssetManager_open(mgr,
		path,
		AASSET_MODE_BUFFER);

	// ASSERT(assetDescriptor, "%s does not exist in %s",
	//	   assetName.c_str(), __FUNCTION__);

	size_t fileLength = AAsset_getLength(assetDescriptor);
	result = (char*)malloc(fileLength * sizeof(char));

	// buf.resize(fileLength);
	int64_t readSize = AAsset_read(assetDescriptor, result, fileLength);

	AAsset_close(assetDescriptor);

	// TODO: ASSERT
	// return (readSize == buf.size());
	return result;
}

// TODO: optimiointi mahdollisuuus
std::string ReadEntireFileToString(const char* path)
{
	char* file = ReadEntireFile(path);
	std::string result(file);
	free(file);
	return result;
}

char* ReadEntireFile(const char* path, int* sizeOut)
{
	AAssetManager* mgr = ResourceManager::assetManager;

	char *result = 0;
	FILE *file = fopen(path, "r");

	// AAsset* file = AssetManager_open(mgr, path, )


	AAsset* assetDescriptor = AAssetManager_open(mgr,
		path,
		AASSET_MODE_BUFFER);

	// ASSERT(assetDescriptor, "%s does not exist in %s",
	//	   assetName.c_str(), __FUNCTION__);

	size_t fileLength = AAsset_getLength(assetDescriptor);
	result = (char*)malloc(fileLength * sizeof(char));

	// buf.resize(fileLength);
	int64_t readSize = AAsset_read(assetDescriptor, result, fileLength);

	AAsset_close(assetDescriptor);
	*sizeOut = fileLength;
	// TODO: ASSERT
	// return (readSize == buf.size());
	return result;
}
#endif

#endif