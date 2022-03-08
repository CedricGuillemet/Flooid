#include "common.h"
#include "bgfx_utils.h"
#include "imgui/imgui.h"
#include "Shaders.h"
#include "Flooid.h"

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

		HideLoader();
	}

	virtual int shutdown() override
	{
		imguiDestroy();
		bgfx::shutdown();
		return 0;
	}

    void editTransform(const float* view, const float* projection, float* world)
    {
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
        if (ImGui::IsKeyPressed(90))
           mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(69))
           mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(82)) // r Key
           mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
          mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
           mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
          mCurrentGizmoOperation = ImGuizmo::SCALE;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(world, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, world);

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
          if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
             mCurrentGizmoMode = ImGuizmo::LOCAL;
          ImGui::SameLine();
          if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
             mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        static bool useSnap(false);
        if (ImGui::IsKeyPressed(83))
          useSnap = !useSnap;
        ImGui::Checkbox("Snap", &useSnap);
        ImGui::SameLine();
        static float snap[3] = {1, 1, 1};
        switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
           ImGui::InputFloat3("Snap", snap);
           break;
        case ImGuizmo::ROTATE:
           ImGui::InputFloat("Angle Snap", snap);
           break;
        case ImGuizmo::SCALE:
           ImGui::InputFloat("Scale Snap", snap);
           break;
        default:
            break;
        }
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(view, projection, mCurrentGizmoOperation, mCurrentGizmoMode, world, NULL, useSnap ? snap : NULL);
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
			
            bool usingGUI = m_flooid.UI();
        	
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
			parameters.lButDown = !usingGUI && io.MouseDown[0];
			parameters.rButDown = !usingGUI && io.MouseDown[1];
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
