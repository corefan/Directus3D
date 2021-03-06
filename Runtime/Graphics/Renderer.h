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

//= INCLUDES ===========================
#include <memory>
#include <vector>
#include "D3D11/D3D11GraphicsDevice.h"
#include "../Core/SubSystem.h"
#include "../Math/Matrix.h"
#include "../Resource/ResourceManager.h"
#include "../Core/Settings.h"
//======================================

namespace Directus
{
	class GameObject;
	class MeshFilter;
	class Camera;
	class Skybox;
	class LineRenderer;
	class Light;
	class MeshFilter;	
	class GBuffer;
	class Rectangle;
	class DeferredShader;
	class Shader;
	class Texture;
	class ResourceManager;
	class D3D11RenderTexture;
	class D3D11GraphicsDevice;
	class Font;
	class Grid;
	class Variant;

	namespace Math
	{
		class Frustrum;
	}

	enum RenderFlags : unsigned long
	{
		Render_Albedo				= 1UL << 0,
		Render_Normal				= 1UL << 1,
		Render_Depth				= 1UL << 2,
		Render_Material				= 1UL << 3,
		Render_Physics				= 1UL << 4,
		Render_Bounding_Boxes		= 1UL << 5,
		Render_Mouse_Picking_Ray	= 1UL << 6,
		Render_Grid					= 1UL << 7,
		Render_Performance_Metrics	= 1UL << 8,
		Render_Light				= 1UL << 9
	};

	class DLL_API Renderer : public Subsystem
	{
	public:
		Renderer(Context* context);
		~Renderer();

		//= Subsystem ============
		virtual bool Initialize();
		//========================

		void Render();

		// Resolution
		void SetResolution(int width, int height);

		// Viewport
		Math::Vector2 GetViewport() { return GET_VIEWPORT; }
		void SetViewport(float width, float height);

		unsigned long GetRenderFlags() { return m_renderFlags; }
		void SetRenderFlags(unsigned long renderFlags) { m_renderFlags = renderFlags; }

		void Clear();
		const std::vector<weakGameObj>& GetRenderables() { return m_renderables; }

	private:
		//= HELPER FUNCTIONS ========================
		void AcquireRenderables(Variant renderables);
		void DirectionalLightDepthPass();
		void GBufferPass();
		void DeferredPass();
		void PostProcessing();
		void DebugDraw();
		const Math::Vector4& GetClearColor();
		//===========================================
	
		std::unique_ptr<GBuffer> m_GBuffer;

		// GAMEOBJECTS ========================
		std::vector<weakGameObj> m_renderables;
		std::vector<Light*> m_lights;
		Light* m_directionalLight;
		//=====================================

		//= RENDER TEXTURES ================================
		std::unique_ptr<D3D11RenderTexture> m_renderTexPing;
		std::unique_ptr<D3D11RenderTexture> m_renderTexPong;
		//==================================================

		//= SHADERS =====================================
		std::unique_ptr<DeferredShader> m_shaderDeferred;
		std::unique_ptr<Shader> m_shaderDepth;
		std::unique_ptr<Shader> m_shaderLine;
		std::unique_ptr<Shader> m_shaderGrid;
		std::unique_ptr<Shader> m_shaderFont;
		std::unique_ptr<Shader> m_shaderTexture;
		std::unique_ptr<Shader> m_shaderFXAA;
		std::unique_ptr<Shader> m_shaderSharpening;
		std::unique_ptr<Shader> m_shaderBlur;
		//==============================================

		//= DEBUG ==========================================
		std::unique_ptr<Font> m_font;
		std::unique_ptr<Grid> m_grid;
		std::unique_ptr<Texture> m_gizmoTexLightDirectional;
		std::unique_ptr<Texture> m_gizmoTexLightPoint;
		std::unique_ptr<Texture> m_gizmoTexLightSpot;
		std::unique_ptr<Rectangle> m_gizmoRectLight;
		unsigned long m_renderFlags;
		//==================================================

		//= MISC =========================================
		std::vector<ID3D11ShaderResourceView*> m_texArray;
		ID3D11ShaderResourceView* m_texEnvironment;
		std::unique_ptr<Texture> m_texNoiseMap;
		std::unique_ptr<Rectangle> m_fullScreenRect;
		//================================================

		//= PREREQUISITES ================================
		Camera* m_camera;
		Skybox* m_skybox;
		LineRenderer* m_lineRenderer;
		Math::Matrix mView;
		Math::Matrix mProjection;
		Math::Matrix mViewProjection;
		Math::Matrix mOrthographicProjection;
		Math::Matrix mBaseView;
		float m_nearPlane;
		float m_farPlane;
		std::vector<ID3D11ShaderResourceView*> m_textures;
		Graphics* m_graphics;
		ResourceManager* m_resourceMng;
		//================================================
	};
}