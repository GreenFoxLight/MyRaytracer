//
// Real time raytracer for demo purposes
//
// Author: Kevin Trogant
//

#include <windows.h>
#include <cstdint>
#include "myrt.h"
#include "myrt_math.h"

#include "raytracing.cpp"

global_variable const int WINDOW_WIDTH  = 1024;
global_variable const int WINDOW_HEIGHT = 768;

struct window_backbuffer {
    HDC WindowDC;
    HBITMAP Bitmap;
    uint32_t *Pixels;
    
    
    BITMAPINFO BitmapInfo;
    
    uint32_t Width;
    uint32_t Height;
};

global_variable window_backbuffer Backbuffer = {0};

window_backbuffer
CreateBackbuffer(HWND Window,
                 uint32_t Width,
                 uint32_t Height)
{
    window_backbuffer Backbuffer;
    Backbuffer.WindowDC = GetDC(Window);
    Backbuffer.Width = Width;
    Backbuffer.Height = Height;
    
    Backbuffer.BitmapInfo.bmiColors[0] = {0};
    Backbuffer.BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    Backbuffer.BitmapInfo.bmiHeader.biWidth = Width;
    Backbuffer.BitmapInfo.bmiHeader.biHeight = -Height;
    Backbuffer.BitmapInfo.bmiHeader.biPlanes = 1;
    Backbuffer.BitmapInfo.bmiHeader.biBitCount = 32;
    Backbuffer.BitmapInfo.bmiHeader.biCompression = BI_RGB;
    Backbuffer.BitmapInfo.bmiHeader.biSizeImage = 0;
    Backbuffer.BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    Backbuffer.BitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    Backbuffer.BitmapInfo.bmiHeader.biClrUsed = 0;
    Backbuffer.BitmapInfo.bmiHeader.biClrImportant = 0;
    
    Backbuffer.Bitmap = CreateDIBSection(Backbuffer.WindowDC,
                                         &Backbuffer.BitmapInfo,
                                         DIB_RGB_COLORS,
                                         (void**)&Backbuffer.Pixels,
                                         NULL, // don't map to file
                                         0);
    
    return(Backbuffer);
}

// Spawn a new thread and make sure
// that it runs on the specified processor.
internal HANDLE
SpawnThread(LPTHREAD_START_ROUTINE Entry,
            unsigned int ProcessorIdx,
            void *Parameter)
{
    DWORD ThreadId;
    HANDLE Thread = CreateThread(NULL, // Default security options
                                 0, // Default stack size (1MB)
                                 Entry,
                                 Parameter,
                                 CREATE_SUSPENDED,
                                 &ThreadId);
    SYSTEM_INFO SysInfo;
    GetSystemInfo(&SysInfo);
    if (ProcessorIdx < SysInfo.dwNumberOfProcessors) {
        SetThreadIdealProcessor(Thread, ProcessorIdx);
    }
    return Thread;
}

internal LRESULT CALLBACK
WindowProc(HWND Window,
           UINT Message,
           WPARAM WParam,
           LPARAM LParam)
{
    switch (Message) {
        case WM_DESTROY: {
            PostQuitMessage(0);
        } break;
        case WM_PAINT: {
            RECT UpdateRegion, SourceRegion;
            BOOL NeedBeginPaint = GetUpdateRect(Window,
                                                &UpdateRegion,
                                                FALSE);
            SourceRegion.top = 0;
            SourceRegion.left = 0;
            SourceRegion.right = Backbuffer.Width;
            SourceRegion.bottom = Backbuffer.Height;
            
            PAINTSTRUCT PaintInfo;
            HDC DeviceContext = Backbuffer.WindowDC;
            if (NeedBeginPaint)
                DeviceContext = BeginPaint(Window, &PaintInfo);
            StretchDIBits(
                DeviceContext,
                UpdateRegion.left,
                UpdateRegion.top,
                UpdateRegion.right - UpdateRegion.left,
                UpdateRegion.bottom - UpdateRegion.top,
                SourceRegion.left,
                SourceRegion.top,
                SourceRegion.right - SourceRegion.left,
                SourceRegion.bottom - SourceRegion.top,
                Backbuffer.Pixels,
                &Backbuffer.BitmapInfo,
                DIB_RGB_COLORS,
                SRCCOPY);
            
            if (NeedBeginPaint)
                EndPaint(Window, &PaintInfo);
        } break;
    }
    return DefWindowProc(Window, Message, WParam, LParam);
}

int CALLBACK
WinMain(
HINSTANCE Instance,
HINSTANCE PrevInstance,
LPSTR     CmdLine,
int       CmdShow)
{
    // Create a window
    WNDCLASS WndClass = {
        CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT,
        WindowProc,
        0, // extra class memory
        0, // extra window memory
        Instance,
        NULL, // default icon
        NULL, // default cursor
        NULL, // we paint our own background
        NULL, // no default menu
        "MyRTWindowClass" // class name
    };
    ATOM ClassAtom = RegisterClass(&WndClass);
    
    HWND Window = CreateWindow("MyRTWindowClass", // used class
                               "My Raytracer", // title
                               WS_OVERLAPPEDWINDOW, // window style
                               CW_USEDEFAULT, // default x pos
                               CW_USEDEFAULT, // default y pos
                               WINDOW_WIDTH,
                               WINDOW_HEIGHT,
                               NULL, // no parent menu
                               NULL, // no menu
                               Instance,
                               NULL);
    if (!Window) {
        UnregisterClass("MyRTWindowClass", Instance);
        return(1);
    }
    Backbuffer = CreateBackbuffer(Window, 1024, 768);
    
    // Setup work queue
    
    // Spawn worker threads
    
    // In main loop:
    // Spawn work for each pixel region
    // Wait until all threads are done
    // Blit backbuffer bitmap to front bitmap
    
    ShowWindow(Window, SW_SHOW);
    
    raytracer_world RaytracerWorld;
    RaytracerWorld.TheSphere = v3 { 0.0f, 0.0f, 100.0f };
    RaytracerWorld.SphereRadius = 10.0f;
    raytracer_params RaytracerParams;
    RaytracerParams.World = &RaytracerWorld;
    RaytracerParams.BackbufferWidth = 1024;
    RaytracerParams.BackbufferHeight = 768;
    RaytracerParams.Pixels = Backbuffer.Pixels;
    
    LONGLONG LastRenderTime = 0;
    bool KeepRunning = true;
    while (KeepRunning) {
        MSG Message;
        while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
            if (Message.message == WM_QUIT) {
                KeepRunning = false;
                break;
            }
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        
        LARGE_INTEGER CurrentTime;
        QueryPerformanceCounter(&CurrentTime);
        // Render with 60fps
        if (CurrentTime.QuadPart - LastRenderTime > 16) {
            // Draw to backbuffer
            GdiFlush();
            RaytracerMain(&RaytracerParams);
            
            InvalidateRect(Window, NULL, 0);
            UpdateWindow(Window);
            LastRenderTime = CurrentTime.QuadPart;
        }
    }
    
    DeleteObject(Backbuffer.Bitmap);
    DestroyWindow(Window);
    UnregisterClass("MyRTWindowClass", Instance);
    
    return(0);
}