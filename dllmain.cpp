#include "pch.h"
#include "windows.h"
#include "stdio.h"
#include <iostream>
#include <pdh.h>
#include "detours/detours.h"
#include "TypeNames.h"
#include <string>
#include "dllmain.h"
#include <fstream>
#include <vector>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
int lua_state = NULL;
bool isReady = false;
DWORD* gScriptContext = nullptr;
int pendingRenders = 0;

int Close(uintptr_t L);

// set the number of expected renders
int SetNumRenders(uintptr_t L) {
    pendingRenders = Lua::lua_checkint(L, 1);
    return 0;
}

// upload the image via http post
bool UploadImage(const std::vector<char>& fileData, const std::string& filename) {
    HINTERNET hSession = nullptr;
    HINTERNET hConnect = nullptr;
    HINTERNET hRequest = nullptr;
    BOOL receiveResult = FALSE;
    BOOL sendResult = FALSE;
    BOOL writeResult = FALSE;
    BOOL addResult = FALSE;
    DWORD bytesRead = 0;
    DWORD dwBytesWritten = 0;
    std::string response;
    hSession = WinHttpOpen(L"RbxRenderHook", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        std::cout << "Failed to open WinHTTP session." << std::endl;
        return false;
    }
    hConnect = WinHttpConnect(hSession, L"example.com", INTERNET_DEFAULT_HTTP_PORT, 0); // CHANGE THE WEBSITE TO YOUR OWN ONE. THIS IS THE BASEURL
    if (!hConnect) {
        std::cout << "Failed to connect to server." << std::endl;
        WinHttpCloseHandle(hSession);
        return false;
    }
    std::wstring urlPath = L"/thisisan/exampledirectory/uploadimage.php?accesskey=password123"; // CHANGE THE DIRECTORY TO YOUR OWN ONE. THIS IS WHERE THE IMAGE WILL BE UPLOADED.
    hRequest = WinHttpOpenRequest(hConnect, L"POST", urlPath.c_str(),
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) {
        std::cout << "Failed to open HTTP request." << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }
    // boundary
    const char* boundary = "----RbxRenderBoundary12345";
    std::string boundaryStr = std::string("--") + boundary + "\r\n";
    std::string endBoundaryStr = std::string("\r\n--") + boundary + "--\r\n";
    std::string contentType = "image/png";
    // headers for file
    std::string postData1 = boundaryStr +
        "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n" +
        "Content-Type: " + contentType + "\r\n\r\n";
    // closing boundary
    std::string postData2 = endBoundaryStr;
    // total size
    DWORD totalSize = static_cast<DWORD>(postData1.length() + fileData.size() + postData2.length());
    // set headers
    std::wstring headers = L"Content-Type: multipart/form-data; boundary=" + std::wstring(boundary, boundary + strlen(boundary)) + L"\r\n";
    addResult = WinHttpAddRequestHeaders(hRequest, headers.c_str(), (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD);
    if (!addResult) {
        std::cout << "Failed to add request headers." << std::endl;
        goto cleanup;
    }
    
    sendResult = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, totalSize, 0);
    if (!sendResult) {
        std::cout << "Failed to send request." << std::endl;
        goto cleanup;
    }
    // write data in parts
    writeResult = WinHttpWriteData(hRequest, postData1.c_str(), static_cast<DWORD>(postData1.length()), &dwBytesWritten);
    if (!writeResult) {
        std::cout << "Failed to write part 1." << std::endl;
        goto cleanup;
    }
    // write file data
    writeResult = WinHttpWriteData(hRequest, fileData.data(), static_cast<DWORD>(fileData.size()), &dwBytesWritten);
    if (!writeResult) {
        std::cout << "Failed to write file data." << std::endl;
        goto cleanup;
    }

    writeResult = WinHttpWriteData(hRequest, postData2.c_str(), static_cast<DWORD>(postData2.length()), &dwBytesWritten);
    if (!writeResult) {
        std::cout << "Failed to write part 2." << std::endl;
        goto cleanup;
    }
    // receive response
    receiveResult = WinHttpReceiveResponse(hRequest, NULL);
    if (!receiveResult) {
        std::cout << "Failed to receive response." << std::endl;
        goto cleanup;
    }

    char responseBuffer[1024];
    do {
        receiveResult = WinHttpReadData(hRequest, responseBuffer, sizeof(responseBuffer) - 1, &bytesRead);
        if (bytesRead > 0) {
            response.append(responseBuffer, bytesRead);
        }
    } while (receiveResult && bytesRead > 0);
    std::cout << "Server response: " << response << std::endl;
    std::cout << "Upload successful!" << std::endl;
cleanup:
    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);
    return receiveResult != 0;
}
// ARGS:
// render(path, x, y, isCharacter) (isCharacter will set Character View.)
int Render(uintptr_t L)
{
    if (pendingRenders == 0) {
        pendingRenders = 1; // Default to single render if not set
    }
    size_t length = 0;
    const char* path = Lua::lua_checklstring(L, 1, &length);
    std::string path_str(path);
    std::cout << path << std::endl;
    int x = Lua::lua_checkint(L, 2);
    int y = Lua::lua_checkint(L, 3);
    int isCharacter = Lua::lua_checkint(L, 4);
    std::cout << Lua::lua_checkint(L, 4) << std::endl;
    std::cout << isCharacter << std::endl;
    HWND window = FindWindowA(nullptr, "Roblox"); // this doesn't matter.
    SetWindowPos(window, 0, 0, 0, x, y, 0);
    OSContext context{ window , x , y };
    DWORD workspace = WorkspaceFuncs::findWorkspace(gScriptContext);
    DWORD datamodel = *(DWORD*)(workspace + 0x38);
    int singleton = CRenderSettings::GetRenderSettings() + 0x84;
    *(BYTE*)(singleton + 0xE1) = 0;
    // NOTE: IF YOU DO NOT USE OPENGL AND USE DIRECT3D9 INSTEAD, THE SKY WILL BE INVISIBLE. USE OPENGL.
    // ^ and do not use automatic graphics mode, set it to opengl.
    DWORD viewBase = ViewBase::CreateView(OpenGL, &context, singleton);
    if (isCharacter)
        Workspace::clearTerrain((DWORD*)workspace);
    Workspace::setImageServerView((DWORD*)workspace, 0, isCharacter ? 0 : 1, (DWORD*)(workspace + 0x160));
    Lighting::setupLighting((DWORD*)datamodel, 0);
    BindW::bindWorkspace((DWORD*)viewBase, 0, datamodel, 0);
    int count = 0;
    while (count < 100) { // bandaid ass fix from waterboi (allows all parts to load)
        RenderView::renderThumb((DWORD*)viewBase, 0);
        count++;
    }
    SaveFile::saveRenderTarget((DWORD*)viewBase, 0, path); // you could also use the screenshot function however that yields lower quality results
    std::cout << "saved!" << std::endl;
    Sleep(1000); // wait a sec before uploading
    ViewBase::DeleteView((DWORD*)viewBase + 2, nullptr, 1);
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open saved file for upload." << std::endl;
        return 0;
    }
    std::vector<char> fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    // get filename from path (simple: last part after last \ or /)
    std::string filename = path_str;
    size_t pos = filename.find_last_of("\\/");
    if (pos != std::string::npos) {
        filename = filename.substr(pos + 1);
    }
    // upload the bomb.
    if (!UploadImage(fileData, filename)) {
        std::cout << "Upload failed." << std::endl;
    }
    pendingRenders--;
    if (pendingRenders <= 0) {
        Close(L);
    }
    // optional: uncomment below if you wish to delete the local file
    // DeleteFileA(path);
    return 0;
}
int Close(uintptr_t L) {
    exit(1);
    return 0;
}
void __fastcall openStateHook(DWORD* thisPtr)
{
    std::cout << thisPtr << std::endl;
    ScriptContext::OpenState(thisPtr);
    if (gScriptContext != thisPtr) {
        gScriptContext = thisPtr;
        uintptr_t globalState = thisPtr[34]; // offset
        Lua::lua_pushcclosure(globalState, Render, 0);
        Lua::lua_setfield(globalState, -10002, "render");
        Lua::lua_pushcclosure(globalState, SetNumRenders, 0);
        Lua::lua_setfield(globalState, -10002, "setNumRenders");
        Lua::lua_pushcclosure(globalState, Close, 0);
        Lua::lua_setfield(globalState, -10002, "close");
        std::cout << "pushed lua functions" << std::endl;
    }
}
// makes the exe start really fast
PDH_HQUERY* __fastcall CProcessPerfCounterHook(void* thisPtr)
{
    static PDH_HQUERY dummy = nullptr;
    return &dummy;
}
__declspec(dllexport) void __cdecl paracetamol(bool) {};//import
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        freopen("conin$", "r", stdin);
        freopen("conout$", "w", stdout);
        freopen("conout$", "w", stderr);
        SetConsoleTitle(L"RbxRenderHook");
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)ScriptContext::OpenState, openStateHook);
        DetourAttach(&(PVOID&)CProcessPerfCounter::CProcessPerfCounter, CProcessPerfCounterHook);
        DetourTransactionCommit();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
