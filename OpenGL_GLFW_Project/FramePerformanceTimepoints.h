

#pragma once

#ifndef FRAME_PERFORMANCE_TIMEPOINTS_H_
#define FRAME_PERFORMANCE_TIMEPOINTS_H_

#include "Timepoint.h"

class FramePerformanceTimepoints final {
public:
    unsigned long long frameNumber;
    Timepoint tStart;
    Timepoint* tBeginRender;
    Timepoint* tFlipBuffers;
    FramePerformanceTimepoints* next;
    FramePerformanceTimepoints(unsigned long long frameNum,
        Timepoint timeStart) : frameNumber(frameNum),
        tStart(timeStart),
        tBeginRender(nullptr),
        tFlipBuffers(nullptr),
        next(nullptr) {
    }
    ~FramePerformanceTimepoints() noexcept {
        if (tBeginRender) {
            delete tBeginRender;
            tBeginRender = nullptr;
        }
        if (tFlipBuffers) {
            delete tFlipBuffers;
            tFlipBuffers = nullptr;
        }
    }
    FramePerformanceTimepoints(FramePerformanceTimepoints&& that) noexcept {
        frameNumber = that.frameNumber;
        tStart = that.tStart;
        tBeginRender = that.tBeginRender;
        that.tBeginRender = nullptr;
        tFlipBuffers = that.tFlipBuffers;
        that.tFlipBuffers = nullptr;
        next = that.next;
        that.next = nullptr;
    }
};


#endif //FRAME_PERFORMANCE_TIMEPOINTS_H_