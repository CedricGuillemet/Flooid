namespace App {
    bgfx::ProgramHandle LoadProgram(const char* vertexShaderName, const char* fragmentShaderName)
    {
        auto& caps = *bgfx::getCaps();
        bgfx::ShaderHandle vsh = createEmbeddedShader(embeddedShaders, caps.rendererType, vertexShaderName);
        bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
        if (nullptr != fragmentShaderName)
        {
            fsh = createEmbeddedShader(embeddedShaders, caps.rendererType, fragmentShaderName);

            BX_ASSERT(isValid(vsh) && isValid(fsh), "Failed to create Embedded shaders");
        }
        else 
        {
            BX_ASSERT(isValid(vsh), "Failed to create Embedded shaders");
        }

        return createProgram(vsh, fsh, true);
    }
}
