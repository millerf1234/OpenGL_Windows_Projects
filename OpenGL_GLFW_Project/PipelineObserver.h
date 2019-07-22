
//INCOMPLETE!

#pragma once

#ifndef PIPELINE_OBSERVER_H_
#define PIPELINE_OBERSVER_H_

#include "GlobalIncludes.h"
#include "LoggingMessageTargets.h"

#include <vector>

static constexpr const int NUM_PIPELINE_QUERY_TYPES = 11;

constexpr const size_t VERTICES_SUBMITTED_QUERY = 0U;
constexpr const size_t PRIMITIVES_SUBMITTED_QUERY = 1U;
constexpr const size_t VERTEX_SHADER_INVOCATIONS_QUERY = 2u;
constexpr const size_t TESS_CONTROL_SHADER_PATCHES_QUERY = 3u;
constexpr const size_t TESS_EVAL_SHADER_INVOCATIONS_QUERY = 4u;
constexpr const size_t GEOMETRY_SHADER_INVOCATIONS_QUERY = 5u;
constexpr const size_t GEOMETRY_SHADER_PRIMITIVES_EMITTED_QUERY = 6u;
constexpr const size_t FRAGMENT_SHADER_INVOCATIONS = 7u;
constexpr const size_t COMPUTE_SHADER_INVOCATIONS_QUERY = 8u;
constexpr const size_t CLIPPING_INPUT_PRIMITIVES_QUERY = 9u;
constexpr const size_t CLIPPING_OUTPUT_PRIMITIVES_QUERY = 10u;


typedef struct PipelineQuery {
    size_t queryType;
    bool enabled;
} PQuery;


//Construct an instance of this class before placing 
//draw calls, then call its 'retrieve()' method 
//after all draw calls to observe have been made
class PipelineObserver {
public:

    static PQuery verticesSubmitted;
    static PQuery primitivesSubmitted;
    static PQuery vertexShaderInvocations;
    static PQuery tessControlShaderPatches;
    static PQuery tessEvalInvocations;
    static PQuery geomInvocations;
    static PQuery geomPrimitivesEmitted;
    static PQuery fragInvocations;
    static PQuery computeInvocations;
    static PQuery clippingInputPrimitives;
    static PQuery clippingInputPrimitives;


    PipelineObserver() : mInvalid_(false) {
        
        /*
        glGenQueries(NUM_PIPELINE_QUERY_TYPES, &(mQueries_[0]));
        //It is good practice to make sure that each query is not 0
        for (int i = 0; i < NUM_PIPELINE_QUERY_TYPES; i++) {
            if (mQueries_[i] == 0u) {
                fprintf(MSGLOG, "WARNING! Failed To Create A Pipeline Query!\n");
                glDeleteQueries(10, &(mQueries_[0]));
                mInvalid_ = true;
                return;
            }
        }
        */
    }


    ~PipelineObserver() {
        //if (mQueries_[0] != 0u)
        //    glDeleteQueries(NUM_PIPELINE_QUERY_TYPES, &(mQueries_[0]));
       //Reseting Array Members is not necessary in destructor
       // for (int i = 0; i < NUM_PIPELINE_QUERY_TYPES; i++)
       //     mQueries_[i] = 0u;
    }

private:
    bool mInvalid_;
   // GLuint mQueries_[NUM_PIPELINE_QUERY_TYPES];

    typedef struct ActivePipelineQuery {
        GLenum type;
        int positionInQueryArray;
        bool valid;
        GLuint queryResult;
    } ActiveQuery;


};

PQuery PipelineObserver::verticesSubmitted = { VERTICES_SUBMITTED_QUERY, true };
PQuery PipelineObserver::primitivesSubmitted = { PRIMITIVES_SUBMITTED_QUERY, true };
PQuery PipelineObserver::vertexShaderInvocations = { VERTEX_SHADER_INVOCATIONS_QUERY, true };
PQuery PipelineObserver::tessControlShaderPatches = { TESS_CONTROL_SHADER_PATCHES_QUERY, false };
PQuery PipelineObserver::tessEvalInvocations = { TESS_EVAL_SHADER_INVOCATIONS_QUERY, false };
PQuery PipelineObserver::geomInvocations = { GEOMETRY_SHADER_INVOCATIONS_QUERY, false };
PQuery PipelineObserver::geomPrimitivesEmitted = { GEOMETRY_SHADER_PRIMITIVES_EMITTED_QUERY, false };
PQuery PipelineObserver::fragInvocations = { FRAGMENT_SHADER_INVOCATIONS, true };
PQuery PipelineObserver::computeInvocations = { COMPUTE_SHADER_INVOCATIONS_QUERY, false };
PQuery PipelineObserver::clippingInputPrimitives = { CLIPPING_INPUT_PRIMITIVES_QUERY, true };
PQuery PipelineObserver::clippingInputPrimitives = { CLIPPING_OUTPUT_PRIMITIVES_QUERY, true };





#if 0
//enum class PipelineObserverState { Idle, Observing};

class PipelineObserver {
public:
    PipelineObserver();

    //Returns the current state of this object
    //PipelineObserverState state() const noexcept { return mState_; }

private:
    //PipelineObserverState mState_;
    
};
#endif 


#endif //PIPELINE_OBSERVER_H_
