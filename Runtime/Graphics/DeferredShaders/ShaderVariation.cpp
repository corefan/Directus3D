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

//= INCLUDES =============================
#include "ShaderVariation.h"
#include "../../Logging/Log.h"
#include "../../IO/StreamIO.h"
#include "../../Core/Settings.h"
#include "../../Components/Transform.h"
#include "../../Components/Camera.h"
#include "../../Components/Light.h"
#include "../Material.h"
#include "../D3D11/D3D11ConstantBuffer.h"
#include "../D3D11/D3D11Shader.h"
//=======================================

//= NAMESPACES ================
using namespace std;
using namespace Directus::Math;
//=============================

namespace Directus
{
	ShaderVariation::ShaderVariation()
	{
		// Resource
		InitializeResource(Shader_Resource);

		m_graphics = nullptr;
		m_shaderFlags = 0;
	}

	ShaderVariation::~ShaderVariation()
	{

	}

	void ShaderVariation::Initialize(Context* context, unsigned long shaderFlags)
	{
		m_context = context;
		m_graphics = m_context->GetSubsystem<Graphics>();

		// Save the properties of the material
		m_shaderFlags = shaderFlags;

		Compile(GetResourceFilePath());
	}

	bool ShaderVariation::LoadFromFile(const string& filePath)
	{
		if (!StreamIO::StartReading(filePath))
			return false;

		SetResourceName(StreamIO::ReadSTR());
		SetResourceFilePath(StreamIO::ReadSTR());
		m_shaderFlags = StreamIO::ReadULong();

		StreamIO::StopReading();

		return true;
	}

	bool ShaderVariation::SaveToFile(const string& filePath)
	{
		string savePath = filePath;

		if (savePath == RESOURCE_SAVE)
		{
			savePath = GetResourceFilePath();
		}

		// Add shader extension if missing
		if (FileSystem::GetExtensionFromFilePath(filePath) != SHADER_EXTENSION)
		{
			savePath += SHADER_EXTENSION;
		}

		if (!StreamIO::StartWriting(savePath))
			return false;

		StreamIO::WriteSTR(GetResourceName());
		StreamIO::WriteSTR(GetResourceFilePath());
		StreamIO::WriteULong(m_shaderFlags);
	
		StreamIO::StopWriting();

		return true;
	}

	void ShaderVariation::Set()
	{
		if (!m_D3D11Shader)
		{
			LOG_WARNING("Can't set uninitialized shader");
			return;
		}

		m_D3D11Shader->Set();
	}

	void ShaderVariation::UpdatePerFrameBuffer(Light* directionalLight, Camera* camera)
	{
		if (!m_D3D11Shader || !m_D3D11Shader->IsCompiled())
		{
			LOG_ERROR("Shader hasn't been loaded or failed to compile. Can't update per frame buffer.");
			return;
		}

		if (!directionalLight || !camera)
			return;

		//= BUFFER UPDATE ========================================================================
		PerFrameBufferType* buffer = (PerFrameBufferType*)m_miscBuffer->Map();

		buffer->viewport = GET_RESOLUTION;
		buffer->nearPlane = camera->GetNearPlane();
		buffer->farPlane = camera->GetFarPlane();
		buffer->mLightViewProjection[0] = directionalLight->ComputeViewMatrix() * directionalLight->ComputeOrthographicProjectionMatrix(0);
		buffer->mLightViewProjection[1] = directionalLight->ComputeViewMatrix() * directionalLight->ComputeOrthographicProjectionMatrix(1);
		buffer->mLightViewProjection[2] = directionalLight->ComputeViewMatrix() * directionalLight->ComputeOrthographicProjectionMatrix(2);
		buffer->shadowSplits = Vector4(directionalLight->GetShadowCascadeSplit(1), directionalLight->GetShadowCascadeSplit(2), 0, 0);
		buffer->lightDir = directionalLight->GetDirection();
		buffer->shadowMapResolution = directionalLight->GetShadowCascadeResolution();
		buffer->shadowMappingQuality = directionalLight->GetShadowTypeAsFloat();
		buffer->cameraPos = camera->g_transform->GetPosition();

		m_miscBuffer->Unmap();
		//========================================================================================

		// Set to shader slot
		m_miscBuffer->SetVS(0);
		m_miscBuffer->SetPS(0);
	}

	void ShaderVariation::UpdatePerMaterialBuffer(weak_ptr<Material> material)
	{
		if (material.expired())
			return;

		auto materialRaw = material._Get();

		if (!m_D3D11Shader->IsCompiled())
		{
			LOG_ERROR("Shader hasn't been loaded or failed to compile. Can't update per material buffer.");
			return;
		}

		// Determine if the material buffer needs to update
		bool update = false;
		update = perMaterialBufferCPU.matAlbedo != materialRaw->GetColorAlbedo() ? true : update;
		update = perMaterialBufferCPU.matTilingUV != materialRaw->GetTilingUV() ? true : update;
		update = perMaterialBufferCPU.matOffsetUV != materialRaw->GetOffsetUV() ? true : update;
		update = perMaterialBufferCPU.matRoughnessMul != materialRaw->GetRoughnessMultiplier() ? true : update;
		update = perMaterialBufferCPU.matMetallicMul != materialRaw->GetMetallicMultiplier() ? true : update;
		update = perMaterialBufferCPU.matNormalMul != materialRaw->GetNormalMultiplier() ? true : update;
		update = perMaterialBufferCPU.matShadingMode != float(materialRaw->GetShadingMode()) ? true : update;

		if (update)
		{
			//= BUFFER UPDATE =========================================================================
			PerMaterialBufferType* buffer = (PerMaterialBufferType*)m_materialBuffer->Map();

			buffer->matAlbedo = perMaterialBufferCPU.matAlbedo = materialRaw->GetColorAlbedo();
			buffer->matTilingUV = perMaterialBufferCPU.matTilingUV = materialRaw->GetTilingUV();
			buffer->matOffsetUV = perMaterialBufferCPU.matOffsetUV = materialRaw->GetOffsetUV();
			buffer->matRoughnessMul = perMaterialBufferCPU.matRoughnessMul = materialRaw->GetRoughnessMultiplier();
			buffer->matMetallicMul = perMaterialBufferCPU.matMetallicMul = materialRaw->GetMetallicMultiplier();
			buffer->matNormalMul = perMaterialBufferCPU.matNormalMul = materialRaw->GetNormalMultiplier();
			buffer->matHeightMul = perMaterialBufferCPU.matNormalMul = materialRaw->GetHeightMultiplier();
			buffer->matShadingMode = perMaterialBufferCPU.matShadingMode = float(materialRaw->GetShadingMode());
			buffer->paddding = Vector3::Zero;

			m_materialBuffer->Unmap();
			//========================================================================================
		}

		// Set to shader slot
		m_materialBuffer->SetVS(1);
		m_materialBuffer->SetPS(1);
	}

	void ShaderVariation::UpdatePerObjectBuffer(const Matrix& mWorld, const Matrix& mView, const Matrix& mProjection, bool receiveShadows)
	{
		if (!m_D3D11Shader->IsCompiled())
		{
			LOG_ERROR("Shader hasn't been loaded or failed to compile. Can't update per object buffer.");
			return;
		}

		Matrix world = mWorld;
		Matrix worldView = mWorld * mView;
		Matrix worldViewProjection = worldView * mProjection;

		// Determine if the buffer actually needs to update
		bool update = false;
		update = perObjectBufferCPU.mWorld != world ? true : update;
		update = perObjectBufferCPU.mWorldView != worldView ? true : update;
		update = perObjectBufferCPU.mWorldViewProjection != worldViewProjection ? true : update;
		update = perObjectBufferCPU.receiveShadows != (float)receiveShadows ? true : update;

		if (update)
		{
			//= BUFFER UPDATE =======================================================================
			PerObjectBufferType* buffer = (PerObjectBufferType*)m_perObjectBuffer->Map();

			buffer->mWorld = perObjectBufferCPU.mWorld = world;
			buffer->mWorldView = perObjectBufferCPU.mWorldView = worldView;
			buffer->mWorldViewProjection = perObjectBufferCPU.mWorldViewProjection = worldViewProjection;
			buffer->receiveShadows = perObjectBufferCPU.receiveShadows = (float)receiveShadows;
			buffer->padding = Vector3::Zero;

			m_perObjectBuffer->Unmap();
			//=======================================================================================
		}

		// Set to shader slot
		m_perObjectBuffer->SetVS(2);
		m_perObjectBuffer->SetPS(2);
	}

	void ShaderVariation::UpdateTextures(const vector<ID3D11ShaderResourceView*>& textureArray)
	{
		if (!m_graphics)
		{
			LOG_INFO("GraphicsDevice is expired. Cant't update shader textures.");
			return;
		}

		m_graphics->GetDeviceContext()->PSSetShaderResources(0, (unsigned int)textureArray.size(), &textureArray.front());
	}

	void ShaderVariation::Render(int indexCount)
	{
		if (!m_graphics)
		{
			LOG_INFO("GraphicsDevice is expired. Cant't render with shader");
			return;
		}

		m_graphics->GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
	}

	void ShaderVariation::AddDefinesBasedOnMaterial(shared_ptr<D3D11Shader> shader)
	{
		if (!shader)
			return;

		// Define in the shader what kind of textures it should expect
		shader->AddDefine("ALBEDO_MAP", HasAlbedoTexture());
		shader->AddDefine("ROUGHNESS_MAP", HasRoughnessTexture());
		shader->AddDefine("METALLIC_MAP", HasMetallicTexture());
		shader->AddDefine("NORMAL_MAP", HasNormalTexture());
		shader->AddDefine("HEIGHT_MAP", HasHeightTexture());
		shader->AddDefine("OCCLUSION_MAP", HasOcclusionTexture());
		shader->AddDefine("EMISSION_MAP", HasEmissionTexture());
		shader->AddDefine("MASK_MAP", HasMaskTexture());
		shader->AddDefine("CUBE_MAP", HasCubeMapTexture());
	}

	void ShaderVariation::Compile(const string& filePath)
	{
		if (!m_graphics)
		{
			LOG_INFO("GraphicsDevice is expired. Cant't compile shader");
			return;
		}

		// Load and compile the vertex and the pixel shader
		m_D3D11Shader = make_shared<D3D11Shader>(m_graphics);
		AddDefinesBasedOnMaterial(m_D3D11Shader);
		m_D3D11Shader->Load(filePath);
		m_D3D11Shader->SetInputLayout(PositionTextureTBN);
		m_D3D11Shader->AddSampler(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS);
		m_D3D11Shader->AddSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_LESS_EQUAL);

		// Matrix Buffer
		m_perObjectBuffer = make_shared<D3D11ConstantBuffer>(m_graphics);
		m_perObjectBuffer->Create(sizeof(PerObjectBufferType));

		// Object Buffer
		m_materialBuffer = make_shared<D3D11ConstantBuffer>(m_graphics);
		m_materialBuffer->Create(sizeof(PerMaterialBufferType));

		// Object Buffer
		m_miscBuffer = make_shared<D3D11ConstantBuffer>(m_graphics);
		m_miscBuffer->Create(sizeof(PerFrameBufferType));
	}
}