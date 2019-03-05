
#include "QuickObj_NGonParser.h"


//Example of an ngon face:
// "f 964//546 1122//546 1120//546 1118//546 1116//546 1114//546 1112//546 1110//546 1108//546 1106//546 1104//546 1102//546 1100//546 1098//546 1096//546 1094//546 1092//546 1090//546 1088//546 1086//546 1084//546 1082//546 1080//546 1078//546 1076//546 1074//546 1072//546 1070//546 1068//546 1066//546 1064//546 1062//546 1060//546 1058//546 1056//546 1054//546 1052//546 1050//546 1048//546 1046//546 1044//546 1042//546 1040//546 1038//546 1036//546 1034//546 1032//546 1030//546 1028//546 1026//546 1024//546 1022//546 1020//546 1018//546 1016//546 1014//546 1012//546 1010//546 1008//546 1006//546 1004//546 1002//546 1000//546 998//546 996//546 994//546 992//546 990//546 988//546 986//546 984//546 982//546 980//546 978//546 976//546 974//546 972//546 970//546 968//546 966//546\n"

bool isNum(const char* c) {
    if ( (*c >= '0') && (*c <= '9') ) {
        return true;
    }
    return false;
}

//It is assumed that file is guaranteed to be valid
std::vector<int> findLinesThatAreNGons(const AssetLoadingInternal::AsciiAsset& file) {
  
    std::vector<int> ngonLines = { };
    size_t fileSize = file.getNumberOfLines();

    for (size_t i = 0; i < fileSize; i++) {
        std::string line = file.getLine(i);
        const char * lineIter = line.c_str();

        if (*lineIter != 'f')
            continue;

        lineIter++; //move past the 'v'
        if (*lineIter != ' ')
            continue;

        
        ngonParsing::eatWhitespace(&lineIter);
        if (!isNum(lineIter))
            continue;

        int positionValuesReadSoFarOnCurrentLine = 0; //counter, if this goes above 4, then line is an ngon
        
        lineIter++;
        while (isNum(lineIter))
            lineIter++;

        bool hasComponentsOtherThanPositions = false;
        if (*lineIter == ' ')
            positionValuesReadSoFarOnCurrentLine++;
        else if (*lineIter == '/')
            hasComponentsOtherThanPositions = true;
        else {
            fprintf(ERRLOG, "\nError parsing line: %s\n", file.getLine(i).c_str());
            continue;
        }

        bool determinedFaceIsAnNgon = false;
        bool recordedLineAsNGon = false;

        lineIter++;
        //Here the logic splits down 2 separate paths. There is a chance that no '/' characters appear on this line.
        //If this is true, then we simply need to count the number of times lineIter alternates between spaces and numbers.
        if (!hasComponentsOtherThanPositions) {
            ngonParsing::eatWhitespace(&lineIter); //just in case
            determinedFaceIsAnNgon = false; 
            //while ( (*lineIter != '\n') && (!determinedFaceIsAnNgon) ) {
            while (*lineIter != '\n') {
                while (isNum(lineIter))
                    lineIter++;

                if (*lineIter == ' ') {
                    positionValuesReadSoFarOnCurrentLine++;
                    ngonParsing::eatWhitespace(&lineIter);
                }

                if (positionValuesReadSoFarOnCurrentLine >= 4) {
                    if (!recordedLineAsNGon) {
                        ngonLines.push_back(i);
                        determinedFaceIsAnNgon = true;
                        fprintf(MSGLOG, "NGON FOUND AT LINE %d:\n\t%s\n", i, file.getLine(i).c_str());
                        recordedLineAsNGon = true;
                    }
                }
            }
        }
        //Otherwise it has been determined that '\' delimiting characters appear on the line. Thus we must perform
        //a more complicated parsing procedure for counting vertices
        else {
            while (*lineIter != '/')
                lineIter++;

            while (isNum(lineIter))
                lineIter++;

            determinedFaceIsAnNgon = false;
            //while ((*lineIter != '\n') && (!determinedFaceIsAnNgon)) {
            while (*lineIter != '\n') {
                while (isNum(lineIter) || (*lineIter == '/'))
                    lineIter++;

                if (*lineIter == ' ') {
                    positionValuesReadSoFarOnCurrentLine++;
                    ngonParsing::eatWhitespace(&lineIter);
                }

                if (positionValuesReadSoFarOnCurrentLine >= 4) {
                    if (!recordedLineAsNGon) {
                        ngonLines.push_back(i);
                        determinedFaceIsAnNgon = true;
                        fprintf(MSGLOG, "NGON FOUND AT LINE %d:\n\n%s\n\n", i, file.getLine(i).c_str());
                        recordedLineAsNGon = true;
                    }
                }
            }
        }

        if (recordedLineAsNGon) {
            fprintf(MSGLOG, "\nThe NGon found at line %d has %d sides! Wow!\n", i, positionValuesReadSoFarOnCurrentLine);
        }
    }
    return ngonLines;
}
