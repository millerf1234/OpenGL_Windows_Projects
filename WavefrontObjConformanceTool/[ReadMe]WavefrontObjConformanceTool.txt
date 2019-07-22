
     +-------------------------------------------------------------+
     |    Current Status         INCOMPLETE -- WORK IN PROGRESS    |
     +-------------------------------------------------------------+
x-------x
| TL;DR |
x-------x
    To improve Application loading performance while still relying on the
usage of the '.obj' file format, it should prove beneficial to restrict 
the scope of legal '.obj' syntax from every possibility allowed by the
standard to instead a reduced subset of the standard. This will allow for
the omittance of otherwise-necessary edge-case testing for unusual/uncommon
syntax, thus reducing the complexity of the parse logic the Application
must perform while loading.
    The idea is to require any '.obj' file to first undergo conformance 
validation (with potentially modification) by this tool before it will
be able to be loaded by the Application.



 --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 

x------------x
| BACKGROUND |
x------------x
This is an idea I had while working on a different part of the project.
I have thus not yet had a chance to get around to implementing it, but
my plan is to try to get this fully ready for use in the near future.



 --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 

x------------------x
| FULL DESCRIPTION | 
x------------------x
In my short time since having first written [for the
main project in this solution] an '.obj' parser, I have had a reoccurring 
issue of encountering '.obj' files which fail to parse correctly due to
them having unusual contents. There are also some '.obj' files that have 
weird extensions or unusual geometry or have very large polygon faces 
(my parser only likes triangles and quads). On top of this, there are some
valid '.obj' files that contain superfluous information within them which
could very easily be left out, providing both a smaller required memory footprint
plus a faster parse time. A common example of this would be when an '.obj' is 
exported from Blender with uv-texture coordinates requested as part of the 
export but for a mesh that has no uv-map. In this case, each vertex will be
assigned the texture coordinates <0.000, 0.000>. This data could easily be 
omitted from the '.obj' without affecting the end result. 

Thus enters this tool. The idea is to have it accept a wider range of 
'.obj' file syntax, which it will then convert any unusualness into 
something the main Application can handle. 

It will be possible to tell if an '.obj' has gone through this utility because
this utility will add an additional comment line at the top stating that 
the file has been validated and is ready for use in the Application. To ensure
the integrity of the Application, this line will also include a secretly-generated
'magic number' which will be calculated based off the contents of the entire file.
That way if a file is validated but then someone goes in an manually edits the file,
it will need to be revalidated by this tool before the main Application will accept
it. If the Application tries to load an '.obj' and it computes a different magic number
than the one listed in the file, then it will automatically reject the file. 



 --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  -- 

x---------------------x
| NOTES / EXTRA STUFF |
x---------------------x

I haven't decided yet whether to give processed files a new file extension or to 
keep their extension as '.obj'.


//  Here is a function I wrote that checks an '.obj' file to see be used when implementing this tool:
/*   BONUS FUNCTION                    [I am not sure yet if/how this should be
                                          included with the rest of this class. It's 
                                          more of a convenient debug feature than 
                                          something that is necessary for run time]




   //   Here is the implementation for a function I wrote that checks to see if
   //there are any real texture coordinates in the '.obj' file. It looks like 
   //if an '.obj' file is exported from Blender with texture coordinates included
   //but the model never had a UV-map generated for it, it will contain texture
   //coordinates for each vertex but all of the coordinates will be <0.000000, 
   //0.0000000>. This is basically the same thing as not having texture coordinates,
   //so if we can determine that this is indeed the case, we can simply remove all 
   //of these lines of empty texture coordinate data to reduce the amount of work we 
   //must do when launching the Application.
     
     //#include "AsciiAsset.h"
bool determineIfHasLegitimateTextureCoordinates(std::string filepath) {
    AssetLoadingInternal::AsciiAsset objFile(filepath);
    std::vector<int> linesWithPossibleTecCoordData;
    linesWithPossibleTecCoordData.reserve(objFile.getNumberOfLines());
    objFile.getLinesThatBeginWithCharacter('v', linesWithPossibleTecCoordData);
    for (int lineNum : linesWithPossibleTecCoordData) {
        std::string s = objFile.getLine(lineNum);
        if (s.at(1) != 't') //Line must start with "vt" to be a texture line
            continue; //Line is not a texture line so we can skip it
        auto ss = s.substr(2);
        for (char c : ss) {
            switch (c) {
            default:
            case ' ':
            case '.':
            case '0':
                break;
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                printf("\nFound a vt on line %d that meets the criteria!\n"
                    "Line is: %s\n", i, s.c_str()); //FALLTHROUGH
                return true;
            case '\n':
                continue;
            }
        }
        return false;
    }
*/