/* RenderDemo:              Flying Camera Demo                    */
/* Stage:                     VERTEX SHADER                       */
/*                                                                */
#version 460 core   
// [+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+]
// [|---------------------------  Header  ---------------------------|]
// [+~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+]
// [|  File:    FlyingCameraDemo.vert                                |]
// [|  Title:   Primary Vertex Shader                                |]
// [|  Desc:    [todo]                                               |]
// [|                                                                |]
// [|                                                                |]
// [|  Author:  Forrest S Miller                                     |]
// [|  Date:    June 15, 2019                                        |]
// [+----------------------------------------------------------------+]


 //The following macro is used to simplify the syntax for declaring inputs
#ifndef IN
 #define IN(x) layout(location = x) in
#endif 




/*                                      SECTION 1                                              \  
/----------------------------------------------------------------------------------------------\  
|                                                                                              |  
|       //---------------------------------------------------------------------//              |  
|       //          /+---------------------------------------------+\          //              |  
|       //         /++---------------------------------------------++\         //              |
|       //        |||                STAGE INTERFACE                |||        //              |
|       //         \++---------------------------------------------++/         //              |
|       //          \+---------------------------------------------+/          //              |
|       //---------------------------------------------------------------------//              |
|                                                                                              |
|          ____                                                           ____                 |
|         <    >                                                         <    >                |
|          \__/                                                           \__/                 |
|            \_                     /~~~~~~~~~~~~~~~\                     _/                   |
|              +--------------------+    INPUTS!    +--------------------+                     |
|            _/                     \~~~~~~~~~~~~~~~/                     \_                   |
|          _/_                                                             _\_                 |
|        /'~~~'\                                                         /'~~~'\               |
|                                                                                              |
|    +------------------------>   EXPECTED PER-VERTEX FORMAT   <------------------------+      |
|    |  Description:                                                                    |      |
|    |    The primary input for a vertex shader is a stream of vertices which are to be |      |
|    |      processed. An instance (thread) of the vertex shader is run for each vertex |      |
|    |      in this stream. The following defines the expected per-vertex data          |      |
|    |      components and layout for each vertex which is to be processed by this      |      |
|    |      shader.                                                                     |      |
|    |                                                                                  |      |
|    |    vertex {                                                                      |      |
|    |*/     IN(0)     vec4 WorldPosition; /*{x,y,z,1.0}                                |      |  //layout(location = 0) in vec4 ModelPosition;  // {X,Y,Z,1.0}
|    |*/     IN(1)     vec2 TextureCoord;  /*{s,t}                                      |      |  //layout(location = 1) in vec2 ModelTexCoord;  // {s, t}
|    |*/     IN(2)     vec3 VertNormal;    /*{xN,yN,zN}                                 |      |  //layout(location = 2) in vec3 ModelNormal;    // {Xn, Yn, Zn}
|    |    }                                                                             |      |
|    +----------------------------------------------------------------------------------+      |
|                                                                                              |
|                                                                                              |
|    +------------------------>    PER-Draw-Call Parameters    <------------------------+      |
|    | [i.e. UNIFORMS]                                                                  |      |
|    |  Description: For each draw call there typically is a collection of parameters   |      |    
|    |    which remain constant for every vertex being processed from the stream. These |      |
|    |    parameters are values like for example an offset into world-space shared by   |      |  
|    |    all vertices (a position) or perhaps the current time in a simulation for     |      |
|    |    when the draw call gets made. Values such as these are specified here as      |      |    
|    |    a special type of variable known in GLSL as 'Uniforms'.                       |      |
|    |                                                                                  |      |    
|    |*/     uniform  float  zoom;      /*                                              |      |
|    |*/     uniform  float  time;      /*                                              |      |
|    |*/     uniform   mat4  rotation;  /*                                              |      |
|    |*/     uniform   mat4  MVP;       /* Model View Projection Matrix                 |      |
|    |                                                                                  |      |       
|    +----------------------------------------------------------------------------------+      |
|                                                                                              |    
|                               [WORK STILL IN PROGRESS...]                                    |    
|                                                                                              |    
|                                                                                              |   
|          ____                                                            ____                |
|         <    >                                                          <    >               |
|          \__/                                                            \__/                |
|            \_                     /~~~~~~~~~~~~~~~~\                     _/                  |
|              +--------------------+    OUTPUTS!    +--------------------+                    |
|            _/                     \~~~~~~~~~~~~~~~~/                     \_                  |
|           /                                                                \                 |
|                                                                                              | 
|             OUTPUT IS HANDLED USING A SHADER INTERFACE BLOCK. BASICALLY IT IS A FANCY        |
|                GLSL STRUCT.                                                                  |
|                                                                                              |
|            */ out SHADED_VERTEX_DATA {  /*                                                   |
|            */     vec4 position;        /*                                                   |
|            */     vec3 pretransform_normal;          /*                                                   |
|            */     vec3 normal;          /*                                                   |
|            */     vec2 textureCoord;    /*                                                   |
|            */     float instanceID;     /*                                                   |
|            */     float vertexID;       /*                                                   |
|            */ } shaded_vertex;          /*                                                   |
|                                                                                              |
|                                                                                              |
|      +-----------------------------------------------+                                       |
|      |  END            Shader Interface Description  |                                       |
|      +-----------------------------------------------+                                       |
\----------------------------------------------------------------------------------------------/
\                                                END OF SECTION 1                              /    */





//Function to call for fun and profit
float cosinePartialSummation(float x, int intervalsToCompute, float intervalSpacing) {
    float summation = cos(x);
    for (int i = 1; i <= max(1, intervalsToCompute); i++) {
        summation += (1.0 / pow(intervalSpacing, i)) * sin( intervalSpacing * x);
    }
    return summation;
}


//  BEGIN       Shader Logic Implementation Description

void main() {
    
    shaded_vertex.pretransform_normal = VertNormal;

    vec4 positionInWorldMesh = WorldPosition;
    

    vec4 positionInScreenSpace = MVP * positionInWorldMesh;

    gl_Position = positionInScreenSpace + 
                                          0.05*vec4(float((gl_InstanceID / 18))*((0.001 + (0.0019*float(gl_InstanceID % 5)) + 0.19*float(gl_InstanceID % 25))*float(gl_InstanceID)*cos(time + gl_InstanceID)),
                                               float((gl_InstanceID / 18))*0.05*float(gl_InstanceID)*sin(time + gl_InstanceID),
                                               float((gl_InstanceID / 18))*300.*(clamp(TextureCoord.s - 0.5, -0.5, 0.5) - clamp(TextureCoord.t, 0.3, 1.0)),
                                               17.0*zoom) + vec4(0.0, 0.0, 0.0, 17.0*zoom);

    //shaded_vertex.position = positionInScreenSpace + vec4(0.0, 0.0, 0.0, zoom);
    shaded_vertex.position = positionInWorldMesh;
    shaded_vertex.normal = mat3(rotation) * VertNormal;
    shaded_vertex.textureCoord = TextureCoord;
    shaded_vertex.instanceID = float(gl_InstanceID + 1);
    shaded_vertex.vertexID = float(gl_VertexID);

    gl_PointSize = 1.0 + 0.325*(float((gl_InstanceID + gl_VertexID) % 47));
}

//  END         Shader Logic Implementation Description