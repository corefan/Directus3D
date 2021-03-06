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

//= LINKING =========================
#pragma comment(lib, "fmod64_vc.lib")
//===================================

//= INCLUDES =======================
#include "../Core/Subsystem.h"
#include "fmod.hpp"
#include "../Components/Transform.h"
#include "AudioClip.h"
#include <memory>
//==================================

namespace Directus
{
	class Audio : public Subsystem
	{
	public:
		Audio(Context* context);
		~Audio();

		// SUBSYSTEM =============
		virtual bool Initialize();
		//========================

		bool Update();

		std::weak_ptr<AudioClip> CreateAudioClip();
		void SetListenerTransform(Transform* transform);

	private:
		FMOD_RESULT m_result;
		FMOD::System* m_fmodSystem;
		int m_maxChannels;
		float m_distanceFactor;
		bool m_initialized;

		//= LISTENER =========
		Transform* m_listener;
		FMOD_VECTOR m_pos;
		FMOD_VECTOR m_vel;
		FMOD_VECTOR m_for;
		FMOD_VECTOR m_up;
		//====================

		std::vector<std::shared_ptr<AudioClip>> m_audioHandles;
	};
}