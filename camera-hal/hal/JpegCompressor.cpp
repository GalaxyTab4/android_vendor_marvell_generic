/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Contains implementation of a class YUYVJpegCompressor that encapsulates a
 * converter between YUYV, and JPEG formats.
 */

// #define LOG_NDEBUG 0
#define LOG_TAG "EmulatedCamera_JPEG"
#include <cutils/log.h>
#include "JpegCompressor.h"

namespace android {

YUYVJpegCompressor::YUYVJpegCompressor()
    : Yuv422IToJpegEncoder(mStrides)
{
}

YUYVJpegCompressor::~YUYVJpegCompressor()
{
}

/****************************************************************************
 * Public API
 ***************************************************************************/

status_t YUYVJpegCompressor::compressRawImage(const void* image,
                                              int width,
                                              int height,
                                              int quality)
{
    ALOGV("%s: %p[%dx%d]", __FUNCTION__, image, width, height);
    void* pY = const_cast<void*>(image);
    int offsets[1];
    offsets[0] = 0;
    mStrides[0] = width * 2;
    if (encode(&mStream, pY, width, height, offsets, quality)) {
        ALOGV("%s: Compressed JPEG: %d[%dx%d] -> %d bytes",
             __FUNCTION__, (width * height * 12) / 8, width, height, mStream.getOffset());
        return NO_ERROR;
    } else {
        ALOGE("%s: JPEG compression failed", __FUNCTION__);
        return errno ? errno : EINVAL;
    }
}

}; /* namespace android */
