/*
 * Copyright (c) 2018 Confetti Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/

#ifdef _WIN32

#include "../Interfaces/IFileSystem.h"
#include "../Interfaces/ILogManager.h"
#include "../Interfaces/IOperatingSystem.h"
#include "../Interfaces/IMemoryManager.h"

#if defined(DIRECT3D12)
	#define RESOURCE_DIR "Shaders/PCDX12"
#elif defined(DIRECT3D11)
	#define RESOURCE_DIR "Shaders/PCDX11"
#elif defined(VULKAN)
	#define RESOURCE_DIR "Shaders/PCVulkan"
#endif

const char* pszRoots[FSR_Count] =
{
	RESOURCE_DIR "/Binary/",			// FSR_BinShaders
	RESOURCE_DIR "/",					// FSR_SrcShaders
	RESOURCE_DIR "/Binary/",			// FSR_BinShaders_Common
	RESOURCE_DIR "/",					// FSR_SrcShaders_Common
	"Textures/",						// FSR_Textures
	"Meshes/",							// FSR_Meshes
	"Fonts/",							// FSR_Builtin_Fonts
	"GPUCfg/",							// FSR_GpuConfig
	"Animation/",							// FSR_Animation
	"",									// FSR_OtherFiles
	RESOURCE_DIR "/",					// FSR_Lib0_SrcShaders
};

FileHandle _openFile(const char* filename, const char* flags)
{
	FILE* fp;
	fopen_s(&fp, filename, flags);
	return fp;
}

void _closeFile(FileHandle handle)
{
	fclose((::FILE*)handle);
}

void _flushFile(FileHandle handle)
{
	fflush((::FILE*)handle);
}

size_t _readFile(void *buffer, size_t byteCount, FileHandle handle)
{
	return fread(buffer, 1, byteCount, (::FILE*)handle);
}

bool _seekFile(FileHandle handle, long offset, int origin)
{
	return fseek((::FILE*)handle, offset, origin) == 0;
}

long _tellFile(FileHandle handle)
{
	return ftell((::FILE*)handle);
}

size_t _writeFile(const void *buffer, size_t byteCount, FileHandle handle)
{
	return fwrite(buffer, byteCount, 1, (::FILE*)handle);
}

size_t _getFileLastModifiedTime(const char* _fileName)
{
	struct stat fileInfo;

	if (!stat(_fileName, &fileInfo))
	{
		return (size_t)fileInfo.st_mtime;
	}
	else
	{
		// return an impossible large mod time as the file doesn't exist
		return ~0;
	}
}

tinystl::string _getCurrentDir()
{
	char curDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, curDir);
	return tinystl::string (curDir);
}

tinystl::string _getExePath()
{
	char exeName[MAX_PATH];
	exeName[0] = 0;
	GetModuleFileNameA(0, exeName, MAX_PATH);
	return tinystl::string(exeName);
}

tinystl::string _getAppPrefsDir(const char *org, const char *app)
{
	/*
	* Vista and later has a new API for this, but SHGetFolderPath works there,
	*  and apparently just wraps the new API. This is the new way to do it:
	*
	*	SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE,
	*						 NULL, &wszPath);
	*/

	char path[MAX_PATH];
	size_t new_wpath_len = 0;
	BOOL api_result = FALSE;

	if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, path))) {
		return NULL;
	}

	new_wpath_len = strlen(org) + strlen(app) + strlen(path) + 3;

	if ((new_wpath_len + 1) > MAX_PATH)
	{
		return NULL;
	}

	strcat(path, "\\");
	strcat(path, org);

	api_result = CreateDirectoryA(path, NULL);
	if (api_result == FALSE) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {
			return NULL;
		}
	}

	strcat(path, "\\");
	strcat(path, app);

	api_result = CreateDirectoryA(path, NULL);
	if (api_result == FALSE) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {
			return NULL;
		}
	}

	strcat(path, "\\");
	return tinystl::string (path);
}

tinystl::string _getUserDocumentsDir()
{
	char pathName[MAX_PATH];
	pathName[0] = 0;
	SHGetSpecialFolderPathA(0, pathName, CSIDL_PERSONAL, 0);
	return tinystl::string(pathName);
}

void _setCurrentDir(const char* path)
{
	SetCurrentDirectoryA(path);
}

void _getFilesWithExtension(const char* dir, const char* ext, tinystl::vector<tinystl::string>& filesOut)
{
	tinystl::string path = FileSystem::GetNativePath(FileSystem::AddTrailingSlash(dir));
	WIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA(path + "*" + ext, &fd);
	uint32_t fileIndex = (uint32_t)filesOut.size();
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			filesOut.resize(fileIndex + 1);
			//copy the strings to avoid the memory being cleaned up by windows.
			filesOut[fileIndex] = "";
			filesOut[fileIndex++] = path + fd.cFileName;
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}
}

#endif
