#pragma once

#include <Phoenix/Commander.hpp>
#include <Phoenix/ContentLoader.hpp>
#include <Phoenix/Graphics/Camera.hpp>
#include <Phoenix/Graphics/ChunkMesher.hpp>
#include <Phoenix/Graphics/ChunkRenderer.hpp>
#include <Phoenix/Graphics/Window.hpp>
#include <Phoenix/ImGuiHelpers.hpp>
#include <Phoenix/Settings.hpp>
#include <Phoenix/Voxels/BlockRegistry.hpp>
#include <Phoenix/Voxels/Chunk.hpp>

#include <Phoenix/UI.hpp>

#include <imgui.h>

#include <sstream>
#include <string>

using namespace phx;

class Phoenix : public events::IEventListener
{
	gfx::Window*    m_window;
	gfx::FPSCamera* m_camera;

public:
	Phoenix();
	~Phoenix();

	void onEvent(const events::Event& event) override;
	void run();
};
