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

#pragma once

//= INCLUDES ====================
#include <vector>
#include <memory>
#include <map>
#include "Texture.h"
#include "../Resource/Resource.h"
#include "../Math/Vector2.h"
#include "Rectangle.h"
//===============================

namespace Directus
{
	class ShaderVariation;
	class ShaderPool;
	class TexturePool;

	enum ShadingMode
	{
		Shading_PBR,
		Shading_Unlit,
		Shading_Skybox
	};

	enum MaterialType
	{
		Material_Imported,
		Material_Basic,
		Material_Skybox
	};

	class DLL_API Material : public Resource
	{
	public:
		Material(Context* context);
		~Material();

		//= I/O ===============================================
		bool Save(const std::string& filePath, bool overwrite);
		bool SaveToExistingDirectory();
		//=====================================================

		//= RESOURCE INTERFACE ======================================
		bool LoadFromFile(const std::string& filePath);
		bool SaveToFile(const std::string& filePath) { return true; }
		//===========================================================

		//= TEXTURES ==================================================================
		void SetTexture(std::weak_ptr<Texture> texture);
		std::weak_ptr<Texture> GetTextureByType(TextureType type);
		bool HasTextureOfType(TextureType type);
		bool HasTexture(const std::string& path);
		std::string GetTexturePathByType(TextureType type);
		std::vector<std::string> GetTexturePaths();
		//=============================================================================

		//= SHADER ====================================================================
		void AcquireShader();
		std::weak_ptr<ShaderVariation> FindMatchingShader(unsigned long shaderFlags);
		std::weak_ptr<ShaderVariation> CreateShaderBasedOnMaterial(unsigned long shaderFlags);
		std::weak_ptr<ShaderVariation>& GetShader() { return m_shader; }
		bool HasShader() { return GetShader().expired() ? false : true; }
		void** GetShaderResource(TextureType type);
		//=============================================================================

		//= PROPERTIES ================================================================	
		unsigned int GetModelID() { return m_modelID; }
		void SetModelID(unsigned int ID) { m_modelID = ID; }

		CullMode GetCullMode() { return m_cullMode; }
		void SetCullMode(CullMode cullMode) { m_cullMode = cullMode; }

		float GetOpacity() { return m_opacity; }
		void SetOpacity(float opacity)
		{
			m_opacity = opacity;
			m_alphaBlending = bool(opacity) ? true : false;
		}

		bool GetAlphaBlending() { return m_alphaBlending; }
		void SetAlphaBlending(bool alphaBlending) { m_alphaBlending = alphaBlending; }

		float GetRoughnessMultiplier() { return m_roughnessMultiplier; }
		void SetRoughnessMultiplier(float roughness) { m_roughnessMultiplier = roughness; }

		float GetMetallicMultiplier() { return m_metallicMultiplier; }
		void SetMetallicMultiplier(float metallic) { m_metallicMultiplier = metallic; }

		float GetNormalMultiplier() { return m_normalMultiplier; }
		void SetNormalMultiplier(float normal) { m_normalMultiplier = normal; }

		float GetHeightMultiplier() { return m_heightMultiplier; }
		void SetHeightMultiplier(float height) { m_heightMultiplier = height; }

		ShadingMode GetShadingMode() { return m_shadingMode; }
		void SetShadingMode(ShadingMode shadingMode) { m_shadingMode = shadingMode; }

		Math::Vector4&  GetColorAlbedo() { return m_colorAlbedo; }
		void SetColorAlbedo(const Math::Vector4& color) { m_colorAlbedo = color; }

		Math::Vector2&  GetTilingUV() { return m_uvTiling; }
		void SetTilingUV(const Math::Vector2& tiling) { m_uvTiling = tiling; }

		Math::Vector2& GetOffsetUV() { return m_uvOffset; }
		void SetOffsetUV(const Math::Vector2& offset) { m_uvOffset = offset; }

		bool IsEditable() { return m_isEditable; }
		void SetIsEditable(bool isEditable) { m_isEditable = isEditable; }
		//=============================================================================

	private:
		void TextureBasedMultiplierAdjustment();

		std::weak_ptr<ShaderVariation> m_shader;
		// The reason behind this mess it that materials can exists alone as a file, yet
		// they support some editing via the inspector, so some data must always be known
		// even if the actual textures haven't been loaded yet. For now it's just the TextureType.

		// Type, Texture, FilePath
		std::map<TextureType, std::pair<std::weak_ptr<Texture>, std::string>> m_textures;

		unsigned int m_modelID;
		CullMode m_cullMode;
		float m_opacity;
		bool m_alphaBlending;
		Math::Vector4 m_colorAlbedo;
		float m_roughnessMultiplier;
		float m_metallicMultiplier;
		float m_normalMultiplier;
		float m_heightMultiplier;
		Math::Vector2 m_uvTiling;
		Math::Vector2 m_uvOffset;
		ShadingMode m_shadingMode;
		bool m_isEditable;

		//= DEPENDENCIES ==
		Context* m_context;
		//=================
	};
}