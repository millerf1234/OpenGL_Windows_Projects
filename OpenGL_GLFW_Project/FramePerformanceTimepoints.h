

#pragma once

#ifndef FRAME_PERFORMANCE_TIMEPOINTS_H_
#define FRAME_PERFORMANCE_TIMEPOINTS_H_

#include "Timepoint.h"

class FramePerformanceTimepoints final {
public:
    unsigned long long frameNumber;
    Timepoint tStart;
    Timepoint* timepointBeginRender;
    Timepoint* timepointFlipBuffers;
    FramePerformanceTimepoints* next;
    FramePerformanceTimepoints(unsigned long long frameNum,
        Timepoint timeStart) : frameNumber(frameNum),
        tStart(timeStart),
        timepointBeginRender(nullptr),
        timepointFlipBuffers(nullptr),
        next(nullptr) {
    }
    ~FramePerformanceTimepoints() noexcept {
        if (timepointBeginRender) {
            delete timepointBeginRender;
            timepointBeginRender = nullptr;
        }
        if (timepointFlipBuffers) {
            delete timepointFlipBuffers;
            timepointFlipBuffers = nullptr;
        }
    }
    FramePerformanceTimepoints(FramePerformanceTimepoints&& that) noexcept {
        frameNumber = that.frameNumber;
        tStart = that.tStart;
        timepointBeginRender = that.timepointBeginRender;
        that.timepointBeginRender = nullptr;
        timepointFlipBuffers = that.timepointFlipBuffers;
        that.timepointFlipBuffers = nullptr;
        next = that.next;
        that.next = nullptr;
    }
};


#endif //FRAME_PERFORMANCE_TIMEPOINTS_H_