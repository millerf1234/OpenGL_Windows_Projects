//This is just a quick and   that 
//wasn't intended to be part of 
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
        //Need to have a valid node already existing at 'head' when we enter
        //the render loop so we create a valid but meaningless placeholder node and
        //assign it as the head. This allows us to assume the list will never be empty
        //and allows for simpler logic once program is in a renderloop 
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
    const FramePerformanceTimepoints* allocateFramePerfTimepointsAndAssignToCurrent(const uint64_t frameNumber,
                                                                                    std::string_view         sv);
};


#endif //FRAME_PERFORMANCE_TIMEPOINTS_LIST_H_

