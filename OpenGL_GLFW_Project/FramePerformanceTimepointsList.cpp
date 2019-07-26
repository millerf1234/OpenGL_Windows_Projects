//
// File:                     FramePerformanceTimepointsList
//
// Implementation:          For fun I decided to code this old-school using manual memory management.
//                          I have tested it enough to be fairly confident there are no memory leaks but
//                          with that being said, please use ample care if making any modifications to
//                          this file

#include "FramePerformanceTimepointsList.h"

//            VERY IMPORTANT -- PLEASE READ 
//   It is very important to understand the lifetimes and usage-pattern of the
//'Head' and 'Current' pointers. When this object is created, both pointers
//will be nullptr. No memory will be allocated until the Application is ready
//to enter the render loop.
//   Immediately before the first iteration of the render loop is when 
//the 'Head' and 'Current' pointers are created. 
//   To keep things simple, all memory deallocations are to be performed 
//only through the 'Head' pointer, and all memory allocations are performed
//using the 'Current' pointer. This ensures that all memory is accounted
//for thus greatly reducing the possibility of a leak.


FramePerformanceTimepointsList::FramePerformanceTimepointsList() : framePerformanceListSize(0ULL),
                                                                   framePerformanceListHead(nullptr),
                                                                   framePerformanceListCurrent(nullptr) {

}

FramePerformanceTimepointsList::~FramePerformanceTimepointsList() noexcept {
    if (framePerformanceListSize > 0ULL) {
        while (framePerformanceListHead != nullptr) {
            while (framePerformanceListHead->next != nullptr) {
                while (framePerformanceListHead->next != framePerformanceListCurrent) {
                    auto next = framePerformanceListHead->next;
                    delete framePerformanceListHead;
                    framePerformanceListHead = next;
                }
                //Delete all the elements up until the last one so that headPtr == currentPtr
                delete framePerformanceListHead;
                framePerformanceListHead = framePerformanceListCurrent;
            }
            //The head pointer and current pointer are the same
            delete framePerformanceListHead;
            framePerformanceListHead = nullptr;
            framePerformanceListCurrent = nullptr;
            //If next is nullptr then we have reached the last element in the list
        }
        //Else if head is nullptr we are done
    }
}


const FramePerformanceTimepoints* FramePerformanceTimepointsList::allocateFramePerfTimepointsAndAssignToCurrent(const unsigned long long frameNumber, std::string_view sv) noexcept {
    try {
        if (framePerformanceListCurrent) {
            framePerformanceListCurrent->next = new FramePerformanceTimepoints(frameNumber, Timepoint(sv.empty() ? "NO_CONTENT" : sv));
            framePerformanceListCurrent = framePerformanceListCurrent->next;
        }
        else {
            framePerformanceListCurrent = new FramePerformanceTimepoints(frameNumber, Timepoint(sv.empty() ? "NO_CONTENT" : sv));
        }
        framePerformanceListSize++;
        return framePerformanceListCurrent;
    }
    catch (const std::bad_alloc& e) { //'new' is unsuccessful
        fprintf(ERRLOG, "\nDetected a Bad Allocation!\n[MSG: %s]\n", e.what());
        fprintf(ERRLOG, "Of Course This Means The Application Must Now Crash!\n");
        std::exit(EXIT_FAILURE);
    }
    catch (const std::exception& e) { //Something went wrong
        static const std::string localCopy(sv);
        fprintf(ERRLOG, "\nTEST ABC    svLocalCopy is %s\n", localCopy.data());
        fprintf(ERRLOG, "\n\n\n\n\t\t\t\tException Was Thrown!\n"
            "While Allocating Memory in FramePerforanceTimepointList\'s Member Function\n"
            "\'allocateFramePerfTimepointAndAssignToCurrent(frameNumber, //unsigned long long\n"
            "                                                 sv )\'      //string_view\n");
        fprintf(ERRLOG, "  \t\t(which was called with the parameters:\n"
            "\t\t        Object Pointer: 0x%p \n"
            "\t\t        frameNumber:    %d \n"
            "\t\t        sv:             %s     )\n\n"
            "The Exception Provides The Following Description Message: \n\t \'%s\' \n",
            static_cast<void*>(&(*this)),
            static_cast<int>(frameNumber),
            localCopy.c_str(),
            e.what()
        );
        fprintf(ERRLOG, "\nThe Application Was Not Expecting This Exception.\n"
            "It will thus now crash!\n");

        fprintf(ERRLOG, "\nFYI FUN FACT: EXITING WITH \"CODE 1\" MEANS THE EXECUTABLE\n"
            "ENDED ITS EXISTENCE WITH THE OPERATING SYSTEM BY REPORTING\n"
            "ITS REASON FOR TERMINATION AS \"EXIT_FAILURE\".    [i.e. The Program Crashed]");
        std::exit(EXIT_FAILURE);
    }
    catch (...) { //Gotta catch 'em all
        fprintf(ERRLOG, "\nCaught an unidentified exception while attempting\n"
            "to allocate memory for a new \'FramePerformanceTimepoints\'\n"
            "object");
        fprintf(ERRLOG, "\nNothing left to do at this point other then crash...\n");
        std::exit(EXIT_FAILURE);
    }
}