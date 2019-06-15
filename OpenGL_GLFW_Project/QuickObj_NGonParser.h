
//WARNING! 
//This feature is incomplete and (Probably) not used anywhere within the QuickObj implementation.

//Experimental code for parsing ngon's (i.e. faces of more than 4 vertices). 


//Example of an ngon face:
// "f 964//546 1122//546 1120//546 1118//546 1116//546 1114//546 1112//546 1110//546 1108//546 1106//546 1104//546 1102//546 1100//546 1098//546 1096//546 1094//546 1092//546 1090//546 1088//546 1086//546 1084//546 1082//546 1080//546 1078//546 1076//546 1074//546 1072//546 1070//546 1068//546 1066//546 1064//546 1062//546 1060//546 1058//546 1056//546 1054//546 1052//546 1050//546 1048//546 1046//546 1044//546 1042//546 1040//546 1038//546 1036//546 1034//546 1032//546 1030//546 1028//546 1026//546 1024//546 1022//546 1020//546 1018//546 1016//546 1014//546 1012//546 1010//546 1008//546 1006//546 1004//546 1002//546 1000//546 998//546 996//546 994//546 992//546 990//546 988//546 986//546 984//546 982//546 980//546 978//546 976//546 974//546 972//546 970//546 968//546 966//546\n"


//Note:
//From what I can tell, Blender will never export an '.obj' file with more than 4 vertices a face. I have
//come across some '.obj' files online though that would do something like the above example.

#pragma once

#ifndef QUICK_OBJ_NGON_PARSER_H_
#define QUICK_OBJ_NGON_PARSER_H_

#include "Line.h"           //Used internally by class
#include "Face.h"           //Used internally by class
#include "AsciiAsset.h"     //Used internally by class
#include "Vertex.h"         //Used to store data


//Helper func
namespace ngonParsing {

    std::vector<int> findNGons(const AssetLoadingInternal::AsciiAsset& file);


    inline void eatWhitespace(const char ** c) {
        while (**c == ' ') 
            (*c)++;
    }
} //namespace ngonParsing



#endif  //QUICK_OBJ_NGON_PARSER_H_
