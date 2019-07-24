
//INCOMPLETE!

//REQUIRES OPENGL 4.6 TO WORK PROPERLY. PREVIOUS VERSIONS OF 
//OPENGL REQUIRE THE EXTENSION GL_ARB_pipeline_statistics to 
//be available on the platform.

#pragma once

#ifndef PIPELINE_OBSERVER_H_
#define PIPELINE_OBERSVER_H_

#include "GlobalIncludes.h"
#include "LoggingMessageTargets.h"

#include <string>
#include <vector>

static constexpr const int NUM_PIPELINE_QUERY_TYPES = 12;

constexpr const size_t PRIMITIVES_GENERATED_QUERY = 0u;
constexpr const size_t VERTICES_SUBMITTED_QUERY = 1U;
constexpr const size_t PRIMITIVES_SUBMITTED_QUERY = 2U;
constexpr const size_t VERTEX_SHADER_INVOCATIONS_QUERY = 3u;
constexpr const size_t TESS_CONTROL_SHADER_PATCHES_QUERY = 4u;
constexpr const size_t TESS_EVAL_SHADER_INVOCATIONS_QUERY = 5u;
constexpr const size_t GEOMETRY_SHADER_INVOCATIONS_QUERY = 6u;
constexpr const size_t GEOMETRY_SHADER_PRIMITIVES_EMITTED_QUERY = 7u;
constexpr const size_t FRAGMENT_SHADER_INVOCATIONS = 8u;
constexpr const size_t COMPUTE_SHADER_INVOCATIONS_QUERY = 9u;
constexpr const size_t CLIPPING_INPUT_PRIMITIVES_QUERY = 10u;
constexpr const size_t CLIPPING_OUTPUT_PRIMITIVES_QUERY = 11u;


typedef struct PipelineQuery {
    size_t queryType;
    bool enabled;
} PQuery;


//Construct an instance of this class before placing 
//draw calls, then call its 'retrieve()' method 
//after all draw calls to observe have been made
class PipelineObserver {
public:

    //These following 12 public static structs determine 
    //which pipeline queries will be placed whenever
    //the next instance of this class is constructed.

    static PQuery primitivesGenerated;
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
    static PQuery clippingOutputPrimitives;


    PipelineObserver() : mInvalid_(false),
                         mQueries_{ 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u } {
        
       

        if (primitivesGenerated.enabled) {
            mActiveQueries_.push_back({ GL_PRIMITIVES_GENERATED, false, 0u });
        }
        if (verticesSubmitted.enabled) {
            mActiveQueries_.push_back({GL_VERTICES_SUBMITTED, false, 0u});
        }
        if (primitivesSubmitted.enabled) {
            mActiveQueries_.push_back({ GL_PRIMITIVES_SUBMITTED, false, 0u });
        }
        if (vertexShaderInvocations.enabled) {
            mActiveQueries_.push_back({ GL_VERTEX_SHADER_INVOCATIONS, false, 0u });
        }
        if (tessControlShaderPatches.enabled) {
            //todo
        }
        if (geomInvocations.enabled) {
            //todo
        }
        if (geomPrimitivesEmitted.enabled) {
            //todo
        }
        if (fragInvocations.enabled) {
            mActiveQueries_.push_back({ GL_FRAGMENT_SHADER_INVOCATIONS, false, 0u });
        }
        if (computeInvocations.enabled) {
            //todo
        }
        if (clippingInputPrimitives.enabled) {
            mActiveQueries_.push_back({ GL_CLIPPING_INPUT_PRIMITIVES, false, 0u });
        }
        if (clippingOutputPrimitives.enabled) {
            mActiveQueries_.push_back({ GL_CLIPPING_OUTPUT_PRIMITIVES, false, 0u });
        }
        
        fprintf(MSGLOG, "\n\n\t[DEBUG   Generating %u GPU pipeline queries]\n", mActiveQueries_.size());

        glGenQueries(static_cast<GLsizei>(mActiveQueries_.size()), &(mQueries_[0]));
        //Make sure none of the generated query objects failed to create properly
        for (size_t i = 0U; i < mActiveQueries_.size(); i++) {
            if (mQueries_[i] == 0u) {
                mInvalid_ = true;
                glDeleteQueries(static_cast<GLsizei>(mActiveQueries_.size()), &(mQueries_[0]));
                for (int i = 0; i < NUM_PIPELINE_QUERY_TYPES; i++)
                    mQueries_[i] = 0u;
                return;
            }
        }


        int nextQueryIndex = 0;

        //This implementation works as long as the order in which these 'if' statements 
        //occur never change. (The other option would be to iterate through the ActiveQuery
        //vector with a switch statement to enable each query. The other option is better 
        //because it doesn't rely on the order of these 'if' statements occurring exactly the same
        //multiple times in multiple places.
        if (primitivesGenerated.enabled) {
            glBeginQuery(GL_PRIMITIVES_GENERATED, mQueries_[nextQueryIndex++]);
        }
        if (verticesSubmitted.enabled) {
            glBeginQuery(GL_VERTICES_SUBMITTED, mQueries_[nextQueryIndex++]);
        }
        if (primitivesSubmitted.enabled) {
            glBeginQuery(GL_PRIMITIVES_SUBMITTED, mQueries_[nextQueryIndex++]);
        }
        if (vertexShaderInvocations.enabled) {
            glBeginQuery(GL_VERTEX_SHADER_INVOCATIONS, mQueries_[nextQueryIndex++]);
        }
        if (tessControlShaderPatches.enabled) {
            glBeginQuery(GL_TESS_CONTROL_SHADER_PATCHES, mQueries_[nextQueryIndex++]);
        }
        if (geomInvocations.enabled) {
            glBeginQuery(GL_GEOMETRY_SHADER_INVOCATIONS, mQueries_[nextQueryIndex++]);
        }
        if (geomPrimitivesEmitted.enabled) {
            glBeginQuery(GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED, mQueries_[nextQueryIndex++]);
        }
        if (fragInvocations.enabled) {
            glBeginQuery(GL_FRAGMENT_SHADER_INVOCATIONS, mQueries_[nextQueryIndex++]);
        }
        if (computeInvocations.enabled) {
            glBeginQuery(GL_COMPUTE_SHADER_INVOCATIONS, mQueries_[nextQueryIndex++]);
        }
        if (clippingInputPrimitives.enabled) {
            glBeginQuery(GL_CLIPPING_INPUT_PRIMITIVES, mQueries_[nextQueryIndex++]);
        }
        if (clippingOutputPrimitives.enabled) {
            glBeginQuery(GL_CLIPPING_OUTPUT_PRIMITIVES, mQueries_[nextQueryIndex++]);
        }

        assert(static_cast<size_t>(nextQueryIndex) == mActiveQueries_.size());


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
        if (mQueries_[0] != 0u)
            glDeleteQueries(static_cast<GLsizei>(mActiveQueries_.size()), &(mQueries_[0]));
       //Reseting Array Members is not necessary in destructor
       // for (int i = 0; i < NUM_PIPELINE_QUERY_TYPES; i++)
       //     mQueries_[i] = 0u;
    }


    std::string getPipelineReport() {

        int nextQueryIndex = 0;


        if (primitivesGenerated.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (verticesSubmitted.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (primitivesSubmitted.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (vertexShaderInvocations.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (tessControlShaderPatches.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (geomInvocations.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (geomPrimitivesEmitted.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (fragInvocations.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (computeInvocations.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (clippingInputPrimitives.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }
        if (clippingOutputPrimitives.enabled) {
            glEndQuery(mQueries_[nextQueryIndex++]);
        }

        return "";

    }



private:
    bool mInvalid_;
    //C-Array used for creating and destroying the queries
    GLuint mQueries_[NUM_PIPELINE_QUERY_TYPES];  

    typedef struct ActivePipelineQuery {
        GLenum type;
        bool valid;
        GLuint queryResult;
    } ActiveQuery;

    std::vector<ActiveQuery> mActiveQueries_;
};

PQuery PipelineObserver::primitivesGenerated = { PRIMITIVES_GENERATED_QUERY, true };
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
PQuery PipelineObserver::clippingOutputPrimitives = { CLIPPING_OUTPUT_PRIMITIVES_QUERY, true };




/* //For future use  [just in case]
        if (primitivesGenerated.enabled) {
            
        }
        if (verticesSubmitted.enabled) {
            
        }
        if (primitivesSubmitted.enabled) {
            
        }
        if (vertexShaderInvocations.enabled) {

        }
        if (tessControlShaderPatches.enabled) {

        }
        if (geomInvocations.enabled) {

        }
        if (geomPrimitivesEmitted.enabled) {

        }
        if (fragInvocations.enabled) {

        }
        if (computeInvocations.enabled) {

        }
        if (clippingInputPrimitives.enabled) {

        }
        if (clippingOutputPrimitives.enabled) {

        }
*/





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
