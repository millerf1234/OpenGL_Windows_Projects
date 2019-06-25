
//I have decided to go a different route than the throw-exception-for-improper-usage
//route since improper usage probably won't be surrounded in try-catch blocks anyways,
//so this exception is not ever thrown. 

#pragma once

#ifdef RESTRICTED_OPERATION_VIOLATION_H_
#define RESTRICTED_OPERATION_VIOLATION_H_

#include <stdexcept>

//This exception will be thrown in response to any thread other than the main 
//rendering thread attempting to construct, use, or interact with the 
//ScreenCaptureAssistant object in any way.
class RestrictedOperationViolation : public std::exception {
public:
    RestrictedOperationViolation(std::string_view msg) : std::exception(
        (!(msg.empty())
            ? msg.data()
            : "This Operation Is Not Permitted At This Time")) {

    }
};


#endif //RESTRICTED_OPERATION_VIOLATION_H_