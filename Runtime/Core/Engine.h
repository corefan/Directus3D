/* Copyright (c) <2016> <Panos Karabelas>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions
of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE. */

#pragma once

//= INCLUDES =======
#include "Context.h"
//==================

#define ENGINE_VERSION "v0.3 (alpha)"

namespace Directus
{
	enum EngineMode
	{
		Editor,
		Game
	};

	class DLL_API Engine : public Subsystem
	{
	public:
		Engine(Context* context);
		~Engine() { Shutdown(); }

		// Sets a draw handle, input handle and a window instance for the engine to use
		void SetHandles(void* instance, void* mainWindowHandle, void* drawPaneHandle);

		//= SUBSYSTEM ============
		virtual bool Initialize();
		//========================

		// Performs a complete simulation cycle (used to run your game)
		void Update();

		// Returns whether the engine is running in editor or game mode
		EngineMode GetMode() { return m_mode; }
		void SetMode(EngineMode mode) { m_mode = mode; }

		// Returns the current context
		Context* GetContext() { return m_context; }

		// Shuts down the engine
		void Shutdown();

	private:
		void* m_hinstance;
		void* m_windowHandle;;
		void* m_drawHandle;
		EngineMode m_mode;
	};
}