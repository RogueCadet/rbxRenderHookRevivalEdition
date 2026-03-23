#include "pch.h"

#include "TypeNames.h"
#include <iostream>

T_openstate ScriptContext::OpenState = (T_openstate)(aslr(0x6B8760)); // UPDATED2
T_openstate ScriptContext::original_OpenState = (T_openstate)(nullptr);

T_findWorkspace WorkspaceFuncs::findWorkspace = (T_findWorkspace)(aslr(0x72AC90)); // UPDATED2

RenderView__bindWorkspace_t BindW::bindWorkspace = (RenderView__bindWorkspace_t)(aslr(0x4C1370)); // UPDATED2
T_renderThumb RenderView::renderThumb = (T_renderThumb)(aslr(0x4C07E0)); // UPDATED2

T_CRenderSingleton CRenderSettings::GetRenderSettings = (T_CRenderSingleton)(aslr(0x4190B0)); // UPDATED2

T_CreateView ViewBase::CreateView = (T_CreateView)(aslr(0x673FE0)); //UPDATED22
deleteView_t ViewBase::DeleteView = (deleteView_t)(aslr(0x4C1A60));

T_LightingSetup Lighting::setupLighting = (T_LightingSetup)(aslr(0x4BE910)); // UPDATED2

T_setImageView Workspace::setImageServerView = (T_setImageView)(aslr(0x731A80)); // UPDATED2
T_clearTerrain Workspace::clearTerrain = (T_clearTerrain)(aslr(0x728690)); // Hi

T_saveRenderTarget SaveFile::saveRenderTarget = (T_saveRenderTarget)(aslr(0x4BABD0)); // UPDATED2

lua_pushcclosure_t Lua::lua_pushcclosure = (lua_pushcclosure_t)(aslr(0x83CDF0)); //UPDATED
lua_setfield_t Lua::lua_setfield = (lua_setfield_t)(aslr(0x83D170)); // UPDATED
lua_checklstring_t Lua::lua_checklstring = (lua_checklstring_t)(aslr(0x83E5A0)); // UPDATED
lua_checkint_t Lua::lua_checkint = (lua_checkint_t)(aslr(0x83E6E0));

CProcessPerfCounter_t CProcessPerfCounter::CProcessPerfCounter = (CProcessPerfCounter_t)(aslr(0x98F300));