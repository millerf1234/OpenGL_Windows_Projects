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
#include <iomanip>
#include <set>
#include <ostream>

#include "GLFW_Init.h"

//Basic utility class representing an instance in time. Holds a double 
//member 'timepoint' which represents the number of seconds since GLFWinit()
//was successfully called and a std::string member representing an assignable
//label. A LocalTimepoint will not be logged globally and so is best utilized 
//internal time measurements. For most time events use Timepoint to allow a 
//snapshot of how the Application performs to be recorded.
class LocalTimepoint {
public:
    double timepoint;
    std::string tag;
    LocalTimepoint() : LocalTimepoint("  ~~~~[No Message Assigned]~~~~  ") { ; }
    LocalTimepoint(std::string_view msg) : tag(!(msg.empty())
        ? (msg)
        : ("       ")) {
        if (GLFW_INIT_INTERNAL::GLFW_IS_INIT()) {
            timepoint = glfwGetTime();
            //mMasterTimepointRecord_.insert(*this);
        }
        else
            timepoint = 0.0;
    }
    double operator-(const LocalTimepoint& that) const noexcept {
        return (timepoint - that.timepoint);
    }
};


class Timepoint : public LocalTimepoint {
public:

    //INSERTING A TIMEPOINT WITHOUT A MESSAGE WILL NOT LOG IT GLOBALLY
    Timepoint() : LocalTimepoint() { ; }

    //Constructs a timepoint. WIll log it globally
    Timepoint(std::string_view msg) : LocalTimepoint(msg) {
        if (GLFW_INIT_INTERNAL::GLFW_IS_INIT())
            mMasterTimepointRecord_.insert(*this);
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
        //Print the first value out as a special case
        for (; citer != mMasterTimepointRecord_.cend(); citer++) {
            //auto oldWidth = oss.width(digits);
            oss << "\t" << std::showpoint << std::setprecision(static_cast<std::streamsize>(6)) << std::left
                << floor((citer->timepoint - t0));// * 1'000'000'000.0) / 1'000'000.0;
            //oss.width(oldWidth);
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