#include "common.h"
#include "bgfx_utils.h"
#include "imgui/imgui.h"
#include "Shaders.h"
#include "Flooid.h"
#include <debugdraw/debugdraw.h>

namespace App
{
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
EM_JS(void, HideLoader, (), { document.getElementById("loader").style.display = "none"; });
#else
void HideLoader() {}
#endif

class App : public entry::AppI
{
public:
	App(const char* _name, const char* _description, const char* _url)
		: entry::AppI(_name, _description, _url)
	{
	}

	void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
	{
		Args args(_argc, _argv);

		m_width  = _width;
		m_height = _height;
		m_debug  = BGFX_DEBUG_NONE;
		m_reset  = BGFX_RESET_VSYNC;

		bgfx::Init init;
		init.type     = args.m_type;
		init.vendorId = args.m_pciId;
		init.resolution.width  = m_width;
		init.resolution.height = m_height;
		init.resolution.reset  = m_reset;
		bgfx::init(init);

		// Enable debug text.
		bgfx::setDebug(m_debug);

		// Set view 0 clear state.
		bgfx::setViewClear(0, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH, 0, 1.0f, 0);

		m_timeOffset = bx::getHPCounter();

		imguiCreate();
        
		m_flooid.Init();

		ddInit();

		HideLoader();
	}

	virtual int shutdown() override
	{
		ddShutdown();
		imguiDestroy();
		bgfx::shutdown();
		return 0;
	}

    
	bool update() override
	{
		if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState) )
		{
			imguiBeginFrame(m_mouseState.m_mx
				,  m_mouseState.m_my
				, (m_mouseState.m_buttons[entry::MouseButton::Left  ] ? IMGUI_MBUT_LEFT   : 0)
				| (m_mouseState.m_buttons[entry::MouseButton::Right ] ? IMGUI_MBUT_RIGHT  : 0)
				| (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
				,  m_mouseState.m_mz
				, uint16_t(m_width)
				, uint16_t(m_height)
				);

            // Transform dialog
			static Flooid::Parameters parameters{};
			
            m_flooid.UI();
        	
			imguiEndFrame();

			float time = (float)( (bx::getHPCounter()-m_timeOffset)/double(bx::getHPFrequency() ) );

			bgfx::setViewRect(0, 0, 0, m_width, m_height);
			bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x20304030, 1.0f, 0);
			bgfx::discard();
            bgfx::touch(0);

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height) );

			m_flooid.SetDisplaySize(uint16_t(m_width), uint16_t(m_height));

            ImGuiIO& io = ImGui::GetIO();
			parameters.x = io.MousePos.x / io.DisplaySize.x;
			parameters.y = io.MousePos.y / io.DisplaySize.y;
			parameters.dx = io.MouseDelta.x / io.DisplaySize.x;
			parameters.dy = io.MouseDelta.y / io.DisplaySize.y;
			parameters.lButDown = io.MouseDown[0];
			parameters.rButDown = io.MouseDown[1];
            parameters.enable = !io.WantCaptureMouse;
			m_flooid.Tick(parameters);

            bgfx::frame();

			return true;
		}

		return false;
	}

	entry::MouseState m_mouseState;

	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_debug;
	uint32_t m_reset;
	int64_t m_timeOffset;

    Flooid m_flooid;
};

} // namespace

ENTRY_IMPLEMENT_MAIN(
	  App::App
	, "App"
	, ""
	, ""
	);
