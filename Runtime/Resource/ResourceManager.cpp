/*
Copyright(c) 2016-2017 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ==========================
#include "ResourceManager.h"
#include "../Core/GameObject.h"
#include "../EventSystem/EventSystem.h"
//=====================================

//= NAMESPACES ================
using namespace std;
using namespace Directus::Math;
//=============================

namespace Directus
{
	ResourceManager::ResourceManager(Context* context) : Subsystem(context)
	{
		m_resourceCache = nullptr;
		SUBSCRIBE_TO_EVENT(EVENT_CLEAR_SUBSYSTEMS, EVENT_HANDLER(Clear));
	}

	bool ResourceManager::Initialize()
	{
		// Cache
		m_resourceCache = make_unique<ResourceCache>();

		// Importers
		m_imageImporter = make_shared<ImageImporter>();
		m_modelImporter = make_shared<ModelImporter>(m_context);
		m_fontImporter = make_shared<FontImporter>(m_context);
		m_fontImporter->Initialize();
		
		// Add engine standard resource directories
		AddStandardResourceDirectory(Texture_Resource, "Standard Assets//Textures//");
		AddStandardResourceDirectory(Font_Resource, "Standard Assets//Fonts//");
		AddStandardResourceDirectory(Shader_Resource, "Standard Assets//Shaders//");
		AddStandardResourceDirectory(Cubemap_Resource, "Standard Assets//Cubemaps//");
		AddStandardResourceDirectory(Script_Resource, "Standard Assets//Scripts//");
		AddStandardResourceDirectory(Model_Resource, "Standard Assets//Models//");
		AddStandardResourceDirectory(Material_Resource, "Standard Assets//Materials//");

		// Add project directory
		SetProjectDirectory("Project//");

		return true;
	}

	void ResourceManager::AddStandardResourceDirectory(ResourceType type, const string& directory)
	{
		m_standardResourceDirectories[type] = directory;
	}

	string ResourceManager::GetStandardResourceDirectory(ResourceType type)
	{
		for (auto& directory : m_standardResourceDirectories)
		{
			if (directory.first == type)
				return directory.second;
		}

		return NOT_ASSIGNED;
	}

	void ResourceManager::SetProjectDirectory(const string& directory)
	{
		if (!FileSystem::DirectoryExists(directory))
		{
			FileSystem::CreateDirectory_(directory);
		}

		m_projectDirectory = directory;
	}
}
