

BACKGROUND:
This is an idea I had while working on a different part of the project.
I have thus not yet had a chance to get around to implementing it, but
my plan is to try to get this fully ready for use in the near future.



Description Of This Tool: 
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


/////////  Here is a function I wrote that can be used when implementing this tool:
/*   BONUS FUNCTION                    [I am not sure yet if/how this should be
                                          included with the rest of this class. It's 
                                          more of a convenient debug feature than 
                                          something that is necessary for run time]

        Here is the implementation for a function I wrote that checks to see if
     there are any real texture coordinates in the '.obj' file. It looks like 
     if an '.obj' file is exported from Blender with texture coordinates included
     but the model never had a UV-map generated for it, it will contain texture
     coordinates for each vertex but all of the values will be '0.000000'
     

     AssetLoadingInternal::AsciiAsset tst(R"(obj\DemoSceneInsideABox00.obj)");
    std::vector<int> tstVec; tstVec.reserve(tst.getNumberOfLines());
    tst.getLinesThatBeginWithCharacter('v', tstVec);
    for (int i : tstVec) {
        std::string s = tst.getLine(i);
        if (s.at(1) != 't')
            continue;
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
                printf("\nFound a tn on line %d that meets the criteria!\n"
                    "Line is: %s\n", i, s.c_str()); //FALLTHROUGH
            case '\n':
                continue;
            }
        }
    }
*/