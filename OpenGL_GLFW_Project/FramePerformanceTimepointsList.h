
#pragma once

#ifndef FRAME_PERFORMANCE_TIMEPOINTS_LIST_H_
#define FRAME_PERFORMANCE_TIMEPOINTS_LIST_H_

#include <string>
#include <string_view>

#include "FramePerformanceTimepoints.h"


class FramePerformanceTimepointsList {
public:
    size_t framePerformanceListSize;
    FramePerformanceTimepoints* framePerformanceListHead; //pointer to the head of the list 
    FramePerformanceTimepoints* framePerformanceListCurrent; //pointer to the most recent element in the list
    FramePerformanceTimepointsList();
    ~FramePerformanceTimepointsList() noexcept;


    void prepareToEnterRenderLoop() {

        constexpr const char decoyMsg[] = "Frame 0 Decoy";
        allocateFramePerfTimepointsAndAssignToCurrent(0ull, decoyMsg);
        assert(framePerformanceListCurrent);
        framePerformanceListCurrent->timepointBeginRender = new Timepoint(decoyMsg);
        framePerformanceListCurrent->timepointFlipBuffers = new Timepoint(decoyMsg);
        framePerformanceListHead = framePerformanceListCurrent;
    }

    //This function MUST be called every loop iteration since it will create
    //the internal object which stores each Timepoint for the entire frame
    void recordLoopStartTimepoint(const unsigned long long& frameNumber) {
        allocateFramePerfTimepointsAndAssignToCurrent(frameNumber, "Frame " + std::to_string(frameNumber) + " Start!");
    }

    //Records the Timepoint of the place in the loop where input and logic
    //have completed processing and draw-calls are beginning to be issued
    //[This includes uniform updates]
    void recordBeginDrawCommandsTimepoint() {
        assert(nullptr == framePerformanceListCurrent->timepointBeginRender);
        framePerformanceListCurrent->timepointBeginRender = 
            new Timepoint("Input and Logic Completed. Issuing Draw Calls");
    }

    void recordReadyToFlipBuffersTimepoint() {
        assert(nullptr == framePerformanceListCurrent->timepointFlipBuffers);
        framePerformanceListCurrent->timepointFlipBuffers =
            new Timepoint("All Draw Calls Issued! Preparing to Flip Buffers!");
    }

   

private: 
    const FramePerformanceTimepoints* allocateFramePerfTimepointsAndAssignToCurrent(const unsigned long long frameNumber, std::string_view sv) noexcept;
};


#endif //FRAME_PERFORMANCE_TIMEPOINTS_LIST_H_

