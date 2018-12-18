#include <cmath>

struct raytracer_world {
    v3 TheSphere;
    float SphereRadius;
};

struct raytracer_params {
    raytracer_world *World;
    uint32_t BackbufferWidth;
    uint32_t BackbufferHeight;
    uint32_t *Pixels;
};

internal
THREAD_ENTRY_FUNC(RaytracerMain)
{
    raytracer_params *Params = (raytracer_params*)Parameter;
    raytracer_world *World = Params->World;
    v3 CameraPos = { 0 };
    v3 CameraDir = { 0, 0, 1.0f };
    float NearDist = 0.1f;
    float FarDist  = 1000.0f;
    uint32_t FarColor = 0x0033ccff;
    
    uint32_t BBWidth = Params->BackbufferWidth;
    uint32_t BBHeight = Params->BackbufferHeight;
    uint32_t *BBPixels = Params->Pixels;
    v3 TopLeft    = { -2.0f, 1.0f, 1.0f };
    v3 Vertical   = { 0.0f, -2.0f, 0.0f };
    v3 Horizontal = { 4.0f,  0.0f, 0.0f };
    
    for (uint32_t PixelY = 0; PixelY < BBHeight; ++PixelY) {
        for (uint32_t PixelX = 0; PixelX < BBWidth; ++PixelX) {
            
            BBPixels[PixelY * BBWidth + PixelX] = FarColor;
            
            float Px = (float)BBWidth / PixelX;
            float Py = (float)BBHeight / PixelY;
            
            v3 RayDir = TopLeft + Px * Horizontal + Py * Vertical;
            float RayDirLen = sqrtf(RayDir.X * RayDir.X
                                    + RayDir.Y * RayDir.Y
                                    + RayDir.Z * RayDir.Z);
            RayDir = RayDir / RayDirLen;
            v3 Pos = CameraPos;
            for (float T = 0.0f; T < FarDist; T += 1.0f) {
                v3 PosToSphere = Pos - World->TheSphere;
                float Dist = sqrtf(PosToSphere.X * PosToSphere.X
                                   + PosToSphere.Y * PosToSphere.Y 
                                   + PosToSphere.Z * PosToSphere.Z);
                if (Dist <= World->SphereRadius) {
                    BBPixels[PixelY * BBWidth + PixelX] = 0x00c2b280;
                    break;
                }
            }
        }
    }
    
    return(0);
}
