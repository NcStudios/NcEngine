#include <windows.h>
#include <stdint.h>
#include <xinput.h>
#include <dsound.h>

#include "NCEngine/NCEngine.cpp"

struct Win32_Offscreen_Buffer
{
    //Pixels are 32-bit | Mem Order: BB GG RR XX | Little Endian: XX RR GG BB
    BITMAPINFO info;
    void *memory;
    int width;
    int height;
    int pitch;
};

struct Win32_Window_Dimension
{
    int width;
    int height;
};

bool Running = false;
Win32_Offscreen_Buffer backBuffer;


//Dynamic XINPUT loading
//XINPUT get state
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
X_INPUT_GET_STATE(XInputGetStateStub){
    return(1);
}
typedef X_INPUT_GET_STATE(x_input_get_state);
static x_input_get_state *DynamicXInputGetState = XInputGetStateStub;
//XInput set state
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub){
    return(1);
}
static x_input_set_state *DynamicXInputSetState = XInputSetStateStub;
//END Dynamic XINPUT loading

void Win32PollXInput(){
    for(DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ControllerIndex++){
            XINPUT_STATE ControllerState;
            if(DynamicXInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS){
                XINPUT_GAMEPAD *pad = &ControllerState.Gamepad;

                int16_t StickX = pad->sThumbLX;
                int16_t StickY = pad->sThumbLY;

                WORD buttonField   = pad->wButtons;
                bool DPadUp        = (buttonField & XINPUT_GAMEPAD_DPAD_UP);
                bool DPadDown      = (buttonField & XINPUT_GAMEPAD_DPAD_DOWN);
                bool DPadLeft      = (buttonField & XINPUT_GAMEPAD_DPAD_LEFT);
                bool DPadRight     = (buttonField & XINPUT_GAMEPAD_DPAD_RIGHT);
                bool Start         = (buttonField & XINPUT_GAMEPAD_START);
                bool Back          = (buttonField & XINPUT_GAMEPAD_BACK);
                bool AButton       = (buttonField & XINPUT_GAMEPAD_A);
                bool BButton       = (buttonField & XINPUT_GAMEPAD_B);
                bool XButton       = (buttonField & XINPUT_GAMEPAD_X);
                bool YButton       = (buttonField & XINPUT_GAMEPAD_Y);
                bool LeftShoulder  = (buttonField & XINPUT_GAMEPAD_LEFT_SHOULDER);
                bool RightShoulder = (buttonField & XINPUT_GAMEPAD_RIGHT_SHOULDER);
            } 
        }
}

//DSound Loading
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);
//End DSound Loading



static void Win32LoadXInput(void){
    HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if (!XInputLibrary){
        XInputLibrary = LoadLibraryA("xinput1_3.dll");
    }
    if (XInputLibrary){
        DynamicXInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        DynamicXInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}



static Win32_Window_Dimension Win32GetWindowDimension(HWND window){
    Win32_Window_Dimension result;
    RECT clientRect;
    GetClientRect(window, &clientRect);
    result.width = clientRect.right - clientRect.left;
    result.height = clientRect.bottom - clientRect.top;
    return result;
}


static void Win32InitDSound(HWND Window, int32_t SamplesPerSecond, int32_t BufferSize){
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if (!DSoundLibrary){
        return;
    }

    direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(DSoundLibrary, "DirectSoundCreate");
    if (!DirectSoundCreate){
        return;
    }

    // LPWAVEFORMATEX WaveFormat;
    // WaveFormat->wFormatTag      = WAVE_FORMAT_PCM;
    // WaveFormat->nChannels       = 2;
    // WaveFormat->nSamplesPerSec  = SamplesPerSecond;
    // WaveFormat->wBitsPerSample  = 16;
    // WaveFormat->nBlockAlign     = (WaveFormat->nChannels * WaveFormat->wBitsPerSample) / 8;
    // WaveFormat->nAvgBytesPerSec = WaveFormat->nSamplesPerSec * WaveFormat->nBlockAlign;
    // WaveFormat->cbSize          = 0;

    WAVEFORMATEX WaveFormat;
    WaveFormat.wFormatTag      = WAVE_FORMAT_PCM;
    WaveFormat.nChannels       = 2;
    WaveFormat.nSamplesPerSec  = SamplesPerSecond;
    WaveFormat.wBitsPerSample  = 16;
    WaveFormat.nBlockAlign     = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
    WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
    WaveFormat.cbSize          = 0;

    LPDIRECTSOUND DirectSound;

    if(SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0))){
        if(SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY))){
            LPDIRECTSOUNDBUFFER PrimaryBuffer;
            DSBUFFERDESC BufferDescription = {};
            BufferDescription.dwSize = sizeof(BufferDescription);
            BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
            if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0))){
                
                if(SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat))){

                }
            }
        }

        LPDIRECTSOUNDBUFFER SecondaryBuffer;
        DSBUFFERDESC BufferDescription = {};
        BufferDescription.dwSize = sizeof(BufferDescription);
        BufferDescription.dwBufferBytes = BufferSize;
        BufferDescription.lpwfxFormat = &WaveFormat;
        if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &SecondaryBuffer, 0))){
            
        }
        
    } 
    
}


static void RenderWeirdGradient(Win32_Offscreen_Buffer *buffer, int blueOffset, int greenOffset){
    uint8_t *Row = (uint8_t *)buffer->memory;
    
    for (int y = 0; y < buffer->height; y++){
        uint32_t *Pixel = (uint32_t *)Row;
        for (int x = 0; x < buffer->width; x++){
            uint8_t blue = x + blueOffset;
            uint8_t green = y + greenOffset;

            *Pixel++ = ((green << 8) | blue);
        }
        Row += buffer->pitch;
    }
}

static void Win32ResizeDIBSection(Win32_Offscreen_Buffer *buffer, int width, int height){
    if (buffer->memory){
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    int const bytesPerPixel = 4;

    buffer->width = width;
    buffer->height = height;
    
    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = buffer->width;
    buffer->info.bmiHeader.biHeight = -buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (buffer->width * buffer->height) * bytesPerPixel;
    buffer->memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    buffer->pitch = width*bytesPerPixel;
}

static void Win32DisplayBufferInWindow(HDC deviceContext, Win32_Offscreen_Buffer *buffer, int windowWidth, int windowHeight){

    StretchDIBits(deviceContext,                     //destination buffer (screen)
                  0, 0, windowWidth, windowHeight,   //destination rect
                  0, 0, buffer->width, buffer->height, //source rect
                  buffer->memory,                     //source buffer             
                  &buffer->info,                      //tells windows how source buffer is formatted
                  DIB_RGB_COLORS,                    //flat that tells windows source is RGB data
                  SRCCOPY);                          //flag that tells windows to only copy bits                                 
}


void Win32ProccessSystemMessages(){
    MSG message;
    while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)){
            if (message.message == WM_QUIT){
                Running == false;
            }

            TranslateMessage(&message);
            DispatchMessage(&message);
        }
}





LRESULT CALLBACK WndProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            bool WasDown = ((lParam & (1 << 30)) != 0);
            bool IsDown  = ((lParam & (1 << 31)) == 0);
            if (IsDown != WasDown){
                break;
            }

            uint32_t VKCode = wParam;
            switch(VKCode){
                case 'W':
                    break;
                case 'A':
                    break;


            }

            break;
        }
        case WM_CREATE:
            
            break;
        case WM_SIZE:

            break;
        case WM_CLOSE:
            Running = false;
            break;
        case WM_ACTIVATEAPP:

            break;
        case WM_DESTROY:
            Running = false;
            break;
        case WM_PAINT:
            {
                PAINTSTRUCT paint;
                HDC deviceContext = BeginPaint(window, &paint);
                Win32_Window_Dimension dimension = Win32GetWindowDimension(window);
                Win32DisplayBufferInWindow(deviceContext, &backBuffer, dimension.width, dimension.height);
                EndPaint(window, &paint);
            }
            break;
        default:
            return DefWindowProc(window, message, wParam, lParam);
	}
	return 0;
}





int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand )
{
    Win32LoadXInput();

	WNDCLASS WindowClass = {}; 

    Win32ResizeDIBSection(&backBuffer, 1280, 720);

    WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;  //OWNDC allows device context to be retreived once rather than get/dispose each frame
	WindowClass.lpfnWndProc = WndProc;                   //CALLBACK that windows will call for handling messages
    WindowClass.hInstance = instance;                    //for windows to callback to the function it must also know about our process instance
    WindowClass.lpszClassName = "FirstClass";

    if (!RegisterClass(&WindowClass)){
        OutputDebugString("WNDCLASS not registered\n");
        return 0;
    }

    HWND window = CreateWindowExA( 
                                    0,
                                    WindowClass.lpszClassName,
                                    "FirstClass",
                                    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    0,
                                    0,
                                    instance,
                                    0);
    if (!window){
        OutputDebugString("Window handle not found\n");
        return 0;
    }

    HDC deviceContext = GetDC(window);
    Win32InitDSound(window, 48000, 4800*sizeof(int16_t)*2);
    
    int xOffset = 0;
    int yOffset = 0;

    NCEngine::MainLoop(); 

    Running = true;
    while(Running){
        Win32ProccessSystemMessages();

        Win32PollXInput();
        
        RenderWeirdGradient(&backBuffer, xOffset, yOffset);

        Win32_Window_Dimension dimension = Win32GetWindowDimension(window);
        Win32DisplayBufferInWindow(deviceContext, &backBuffer, dimension.width, dimension.height);

        ++xOffset;
        yOffset += 2;
    }

	return 0;
}



