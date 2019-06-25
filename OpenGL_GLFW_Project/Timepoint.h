//
//
//  Represents a Timepoint captured from GLFWs clock
//  Time is measured in seconds and represented as a double
//  
//  This class has been set up so that whenever a 
//  Dependent upon GLFW to operate
//
//


#pragma once

#ifndef TIMEPOINT_H_
#define TIMEPOINT_H_

#include <string>
#include <string_view>
#include <set>
#include <ostream>

#include "GLFW_Init.h"


class Timepoint {
public:
    double timepoint;
    std::string tag;
    Timepoint() : Timepoint("  ~~~~[No Message Assigned]~~~~  ") { ; }
    Timepoint(std::string_view msg) : tag( !(msg.empty()) 
                                        ? (msg) 
                                          : ("       ") ) {
        if (GLFW_INIT_INTERNAL::GLFW_IS_INIT()) {
            timepoint = glfwGetTime();
            mMasterTimepointRecord_.insert(*this);
        }
        else
            timepoint = 0.0;
    }

    Timepoint(const Timepoint& other) {
        tag = other.tag;
        timepoint = other.timepoint;
        //mMasterTimepointRecord_.insert(*this);
    };
    Timepoint(Timepoint&& other) = default;
    Timepoint& operator=(const Timepoint& that) = default;
    Timepoint& operator=(Timepoint&& that) = default;

    bool operator<(const Timepoint& other) const noexcept {
        return timepoint < other.timepoint;
    }
    bool operator==(const Timepoint& other) const noexcept {
        return timepoint == other.timepoint;
    }

    bool operator>(const Timepoint& other) const noexcept {
        return timepoint > other.timepoint;
    }

    static std::string getAllTimepoints() noexcept {
        if (mMasterTimepointRecord_.empty()) 
            return std::string("\n\n        [No Timepoints Have Yet Been Created!]\n\n"); 
        //Get the smallest timepoint
        auto citer = mMasterTimepointRecord_.cbegin();
        double t0 = citer->timepoint;
        //Get the largest timepoint   
        double tMax = mMasterTimepointRecord_.crbegin()->timepoint - t0;
        //Figure out how many digits it will take to display tMax. That will be
        //the width used for printing the time column
        int digits = 1;
        while (tMax >= 10) {
            tMax = tMax / 10;
            digits++;
        }
        std::ostringstream oss;
        oss << "\n\n     Time (ms)                Message \n   ";
        for (; citer != mMasterTimepointRecord_.cend(); citer++) {
            auto oldWidth = oss.width(digits);
            oss << "\t" << floor((citer->timepoint - t0) * 1'000'000'000) / 1'000'000;
            oss.width(oldWidth);
            oss << "   |   " << citer->tag;
            //Only print new line is tag doesn't already
            auto newlineCheck = citer->tag.crbegin();
            for (; newlineCheck != citer->tag.crend(); newlineCheck++) {
                if (*newlineCheck == '\n')
                    continue;
            }
            oss << "\n";
        }
        oss << "\n\n\n";
        return oss.str();
    }

    static const std::multiset<Timepoint, std::less<Timepoint>>& getMasterTimepointMultiset() noexcept {
        return mMasterTimepointRecord_;
    }

private:
    static std::multiset<Timepoint, std::less<Timepoint>> mMasterTimepointRecord_;
};






#endif //TIMEPOINT_H_