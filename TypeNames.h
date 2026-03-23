#pragma once

#include <iostream>
#include "wininet.h"
#include "winhttp.h"

#define aslr(addy) (addy - 0x400000 + (DWORD)GetModuleHandle(NULL))

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

struct OSContext
{
	void* hWnd;
	int width;
	int height;
};

typedef enum
{
	UnknownGraphicsMode = 0,
	AutoGraphicsMode = 1,
	Direct3D11 = 2,
	Direct3D9 = 3,
	OpenGL,
	NoGraphics
} GraphicsMode;

typedef int (*lua_CFunction) (uintptr_t L);

typedef void(__cdecl* lua_pushcclosure_t)(uintptr_t L, lua_CFunction fn, int n);
typedef void(__cdecl* lua_setfield_t)(uintptr_t L, int idx, const char* k);
typedef const char* (__cdecl* lua_checklstring_t)(uintptr_t L, int numArg, size_t* l);
typedef int(__cdecl* lua_checkint_t)(uintptr_t L, int arg); // 0x83E6E0

typedef void(__thiscall* T_openstate)(DWORD* thisPtr);

typedef int(__cdecl* T_findWorkspace)(DWORD* ScriptContext);

typedef void(__fastcall* RenderView__bindWorkspace_t)(DWORD* thisPtr, DWORD* unused, int datamodel, int a4);
typedef void(__fastcall* T_renderThumb)(DWORD* thisPtr, DWORD* unused);
typedef void(__fastcall* deleteView_t)(DWORD* thisPtr, DWORD* idk, int a3);

typedef int(__cdecl* T_CRenderSingleton)();

typedef int(__cdecl* T_CreateView)(GraphicsMode mode, OSContext* context, int renderSettings);
	
typedef void(__fastcall* T_ShaderSetup)(DWORD* thisInt, DWORD* idk);
typedef void(__fastcall* T_LightingSetup)(DWORD* datamodel, DWORD* unused);

typedef void(__fastcall* T_saveRenderTarget)(DWORD* thisPtr, DWORD* unused, const std::string& filename);

typedef int(__fastcall* T_setImageView)(DWORD* thisPtr, DWORD* unused, int a3, DWORD* workspace);
// hi!
typedef void(__thiscall* T_clearTerrain)(DWORD* thisPtr);

typedef void(__fastcall* CProcessPerfCounter_t)(void* thisPtr);

//0x4BB850
namespace Lua {
	extern lua_pushcclosure_t lua_pushcclosure;
	extern lua_setfield_t lua_setfield;
	extern lua_checklstring_t lua_checklstring;
	extern lua_checkint_t lua_checkint;
}

namespace SaveFile {
	extern T_saveRenderTarget saveRenderTarget;
}

namespace Lighting {
	extern T_ShaderSetup setupShaders;
	extern T_LightingSetup setupLighting;
}

namespace Workspace {
	extern T_clearTerrain clearTerrain;
	extern T_setImageView setImageServerView;
}

namespace ScriptContext {
	extern T_openstate OpenState;
	
	extern T_openstate original_OpenState;
}

namespace WorkspaceFuncs {
	extern T_findWorkspace findWorkspace;
}

namespace RenderView {
	extern T_renderThumb renderThumb;
}

namespace BindW
{
	extern RenderView__bindWorkspace_t bindWorkspace;
}

namespace ViewBase {
	extern T_CreateView CreateView;
	extern deleteView_t DeleteView;
}

namespace CRenderSettings {
	extern T_CRenderSingleton GetRenderSettings;
}

namespace CProcessPerfCounter {
	extern CProcessPerfCounter_t CProcessPerfCounter;
}

