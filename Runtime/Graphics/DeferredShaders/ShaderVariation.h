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

//= INCLUDES ============================
#include <memory>
#include "../D3D11/D3D11GraphicsDevice.h"
#include "../../Resource/Resource.h"
#include "../../Math/Vector2.h"
#include "../../Math/Matrix.h"
//=======================================

struct ID3D11ShaderResourceView;

namespace Directus
{
	class Light;
	class Camera;
	class Material;
	class D3D11ConstantBuffer;
	class D3D11Shader;

	enum ShaderFlags : unsigned long
	{
		Variaton_Albedo		= 1UL << 0,
		Variaton_Roughness	= 1UL << 1,
		Variaton_Metallic	= 1UL << 2,
		Variaton_Normal		= 1UL << 3,
		Variaton_Height		= 1UL << 4,
		Variaton_Occlusion	= 1UL << 5,
		Variaton_Emission	= 1UL << 6,
		Variaton_Mask		= 1UL << 7,
		Variaton_Cubemap	= 1UL << 8,
	};


	class ShaderVariation : public Resource
	{
	public:
		ShaderVariation();
		~ShaderVariation();

		void Initialize(Context* context, unsigned long shaderFlags);

		//= RESOURCE INTERFACE ========================
		bool LoadFromFile(const std::string& filePath);
		bool SaveToFile(const std::string& filePath);
		//=============================================

		void Set();
		void UpdatePerFrameBuffer(Light* directionalLight, Camera* camera);
		void UpdatePerMaterialBuffer(std::weak_ptr<Material> material);
		void UpdatePerObjectBuffer(const Math::Matrix& mWorld, const Math::Matrix& mView, const Math::Matrix& mProjection, bool receiveShadows);
		void UpdateTextures(const std::vector<ID3D11ShaderResourceView*>& textureArray);
		void Render(int indexCount);

		unsigned long GetShaderFlags() { return m_shaderFlags; }
		bool HasAlbedoTexture() { return m_shaderFlags & Variaton_Albedo; }
		bool HasRoughnessTexture() { return m_shaderFlags & Variaton_Roughness; }
		bool HasMetallicTexture() { return m_shaderFlags & Variaton_Metallic; }
		bool HasNormalTexture() { return m_shaderFlags & Variaton_Normal; }
		bool HasHeightTexture() { return m_shaderFlags & Variaton_Height; }
		bool HasOcclusionTexture() { return m_shaderFlags & Variaton_Occlusion; }
		bool HasEmissionTexture() { return m_shaderFlags & Variaton_Emission; }
		bool HasMaskTexture() { return m_shaderFlags & Variaton_Mask; }
		bool HasCubeMapTexture() { return m_shaderFlags & Variaton_Cubemap; }

	private:
		void AddDefinesBasedOnMaterial(std::shared_ptr<D3D11Shader> shader);
		void Compile(const std::string& filePath);

		//= PROPERTIES =======
		unsigned long m_shaderFlags;

		//= MISC ==================================================
		Graphics* m_graphics;
		std::shared_ptr<D3D11ConstantBuffer> m_perObjectBuffer;
		std::shared_ptr<D3D11ConstantBuffer> m_materialBuffer;
		std::shared_ptr<D3D11ConstantBuffer> m_miscBuffer;
		std::shared_ptr<D3D11Shader> m_D3D11Shader;

		//= BUFFERS ===============================================
		const static int cascades = 3;
		struct PerFrameBufferType
		{
			Math::Vector2 viewport;
			float nearPlane;
			float farPlane;
			Math::Matrix mLightViewProjection[cascades];
			Math::Vector4 shadowSplits;
			Math::Vector3 lightDir;
			float shadowMapResolution;
			float shadowMappingQuality;
			Math::Vector3 cameraPos;
		};

		struct PerMaterialBufferType
		{
			// Material
			Math::Vector4 matAlbedo;
			Math::Vector2 matTilingUV;
			Math::Vector2 matOffsetUV;
			float matRoughnessMul;
			float matMetallicMul;
			float matNormalMul;
			float matHeightMul;
			float matShadingMode;
			Math::Vector3 paddding;
		};
		PerMaterialBufferType perMaterialBufferCPU;

		struct PerObjectBufferType
		{
			Math::Matrix mWorld;
			Math::Matrix mWorldView;
			Math::Matrix mWorldViewProjection;
			float receiveShadows;
			Math::Vector3 padding;
		};
		PerObjectBufferType perObjectBufferCPU;
		//==========================================================
	};
}