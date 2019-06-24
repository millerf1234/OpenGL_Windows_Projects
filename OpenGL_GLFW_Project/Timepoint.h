//
//
//  Represents a Timepoint captured from GLFWs clock
//
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
        //if (GLFW_INIT_INTERNAL::GLFW_IS_INIT) {
            timepoint = glfwGetTime();
            mMasterTimepointRecord_.insert(*this);
        //}
        //else
        //    timepoint = 0.0;
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
        if (mMasterTimepointRecord_.empty()) { return "\n\n        [No Timepoints Have Yet Been Created!]\n\n"; }
        //Get the smallest timepoint
        auto citer = mMasterTimepointRecord_.cbegin();
        double t0 = citer->timepoint;
        //Get the largest timepoint
        double tMax = (--mMasterTimepointRecord_.crend())->timepoint - t0;
        //Figure out how many digits it will take to display tMax
        int digits = 1;
        while (tMax >= 10) {
            tMax = tMax / 10;
            digits++;
        }
        std::ostringstream oss;
        oss << "\n\n     Time                 Message \n   ";
        for (; citer != mMasterTimepointRecord_.cend(); citer++) {
            auto oldWidth = oss.width(digits);
            oss << (citer->timepoint - t0);
            oss.width(oldWidth);
            oss << "   |   " << citer->tag << "\n   ";
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