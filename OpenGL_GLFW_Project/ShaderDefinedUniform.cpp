
//The idea: let shaders be able to declare their own uniforms
//in the form of a comment or macro. For example
//
//     |  File: 'SampleVertexShader.vert'                                                                     |
//     |                                                                                                      |
//     |     #version 460 core                                                                                
//     |                                                                                                      |
//     |     //-------------------------------------                                                          |
//     |     //  Declare Ammo to be tracked by Application                                                    |
//     |     //-------------------------------------                                                          |
//     |     //!~DeclareUniform 'Heat' type:'float' init:'185.0' count:'-0.1' rate:'5' scope:'local'          |
//     |                                                                                                      |
//     |                                                                                                      |
//     |      ~~~~                                                                                            |
//     |      ~~~~                                                                                            |
//     |      ~~~~                                                                                            |
//     |                                                                                                      |
//     |                                                                                                      |
//     |                                                                                                      |
//     |  File: 'SampleFragShader.frag'                                                                       |
//     |                                                                                                      |
//     |     //-------------------------------------                                                          |
//     |     //                                                                                               |
//     |     //-------------------------------------                                                          |
//     |     //!~AdoptUniform 'Ammo'                                                                          |
//     |                                                                                                      |
//     |                                                                                                      |
//     |                                                                                                      |
//     |                                                                                                      |
//     |                                                                                                      |
//
//  This is taking too long. 
//  
//  The idea is to invent a basic bare-bones scripting language for shaders to allow them
//  some flexibility to declare uniforms.
//
//  ---------------------
//   HERE IS THE IDEA
//  ---------------------
//          Basically there are 2 available statements that will be searched for and parsed:
//             
//   (1)  The DECLARATION statement. This let's a shader tell the Application to provide it a variable to
//        manage. The declaration statement does not automatically mean the shader its in will be passed the 
//       declared value, instead the variable will only be passed in if it is declared within the scope of 
//       the local shader code file. i.e.  something like 
// |      
// |        (In shader file DroneMothership.frag)
// |  //!~DeclareUniform 'DroneNetworkOverSaturation' type:'float' init:'0.0' count:'==(0.75*DCP'DroneTransponderIndiciationLight.frag') rate: '0' scope: 'global'   |
// |  // [No Declaration of the uniform DroneNetworkSaturation in this file]  
// |
//
// |        (In shader file DroneTransponserIndicationLight.frag)
// |   uniform float DroneNetworkOverSaturation;
// |   main() {
// |       //Compute frag-color based off the value of the drone network's saturation...
//        In the above example statement, the fragment shader for the drone mother ship carrier would like to be able to send out a variable to every
//        one of its drones it has out in combat [or mining]. The more drones are out away from the mother ship in flight, the increasingly saturated
//        their transponder network will become. To indicate network overload, each drone has an indicator light on it providing a visual clue of how stressed
//        their network is. This value will be calculated based off the number of transponders where draw in the previous frame. This means the Application will
//        keep track of the previous frames draw calls and use that for the logic to modify this value. [Might want to give it some inertia and accelartion?]
//        
//          The fact that the variable is global means each of the transponders can declare it as their own uniform and it will be available to them
//
//  ----- I got a little sidetracked. Part of why I got sidetracked though is because I can just imagine this being done on a massive scale, with 
//         taking care of the large drone swarms on the GPU. The CPU probably would still have to track them?
//   
//