#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WINCE
#include <windows.h>
#endif

#include "gcu.h"


int main(int argc, char** argv)
{
    GCUenum result;
    GCUContext pContext;
    GCUSurface pSrcSurface = NULL;
    GCUSurface pDstSurface = NULL;

    GCU_RECT srcRect;
    GCU_RECT dstRect;

    GCUVirtualAddr      srcVirtAddr = 0;
    GCUVirtualAddr      srcVirtAddr_align = 0;
    GCUVirtualAddr      dstVirtAddr = 0;
    GCUPhysicalAddr     dstPhysAddr = 0;

    GCU_INIT_DATA initData;
    GCU_CONTEXT_DATA contextData;
    GCU_FILL_DATA fillData;
    GCU_BLT_DATA bltData;

    int width = 128;
    int height = 128;
    int i = 0;

    memset(&initData, 0, sizeof(initData));
    gcuInitialize(&initData);

    memset(&contextData, 0, sizeof(contextData));
    pContext = gcuCreateContext(&contextData);
    if(pContext == NULL)
    {
        result = gcuGetError();
        exit(0);
    }


    // use bmm to allocate a cacheable buffer to hold 128*128 RGBA data
    srcVirtAddr = (GCUVirtualAddr)malloc(width * height * 4 + 63);
    srcVirtAddr_align = (GCUVirtualAddr)(((GCUint)srcVirtAddr + 63) &~ 63);
    // create src surface from user-allocated buffer
    pSrcSurface = _gcuCreatePreAllocBuffer(pContext, width, height, GCU_FORMAT_ARGB8888, GCU_TRUE, srcVirtAddr_align,   GCU_FALSE, (GCUPhysicalAddr)GCU_NULL);

    // create dst surface from video memory
    pDstSurface = _gcuCreateBuffer(pContext, width, height, GCU_FORMAT_ARGB8888, &dstVirtAddr, &dstPhysAddr);

    if(pSrcSurface && pDstSurface)
    {
        while(i < 1)
            {
            // fill user-allocate buffer with some color
            int j , k;
            for(j = 0; j < height; j++)
            {
                unsigned char* line = (unsigned char*)srcVirtAddr_align + width * 4 * j;
                for(k = 0; k < width; k++)
                {
                    unsigned char* ele = line + k * 4;
                    ele[0] = 45;
                    ele[1] = 123;
                    ele[2] = 63;
                    ele[3] = 34;
                }
            }

            //flush cache to memory
            _gcuFlushCache(pContext, (void *)srcVirtAddr_align, width * height * 4, GCU_CLEAN);

            //gcu blit
            memset(&bltData, 0, sizeof(bltData));
            bltData.pSrcSurface = pSrcSurface;
            bltData.pDstSurface = pDstSurface;
            srcRect.left = 0;
            srcRect.top = 0;
            srcRect.right = width;
            srcRect.bottom = height;
            bltData.pSrcRect = &srcRect;
            dstRect.left = 0;
            dstRect.top = 0;
            dstRect.right = width;
            dstRect.bottom = height;
            bltData.pDstRect = &dstRect;
            bltData.rotation = GCU_ROTATION_0;
            gcuBlit(pContext, &bltData);

            gcuFinish(pContext);
            _gcuDumpSurface(pContext, pDstSurface, "bmmm");
            i++;
        }
    }

    if(pSrcSurface)
    {
        _gcuDestroyBuffer(pContext, pSrcSurface);
    }

    if(pDstSurface)
    {
        _gcuDestroyBuffer(pContext, pDstSurface);
    }

    free((void *)srcVirtAddr);

    gcuDestroyContext(pContext);
    gcuTerminate();
    return 0;
}
