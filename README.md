# OpenGL_Windows_Projects
Playground for OpenGL and Windows

[Project Description] 
A MSVC C++ project of a 3D rendering engine built targeting OpenGL 4.5 and later. Project compiles under C++17 and (probably) could be
made to compile under C++14 with only a few changes. 



[Project Background / 'Why I Started']
This project is my attempt at creating a 3D renderer in modern C++ (i.e. C++17) targeting OpenGL 4.5 (and now 4.6) built on
top of the Open-Source library 'GLFW' [version 3.3]. This project itself began when I first wrote the 3 lines of code constituting
the function main() in early July 2018; however, the groundwork in place for my experience with computer graphics had begun
several years earlier. I consider myself lucky enough to have spent several of my early undergraduate years at Edmonds Community
College, which provided me the oppertunity to experience and explore many of the different STEM fields. It was during this time 
I discovered a common theme throughout STEM: there was always a heavy reliance upon mathematics, and yet many of my fellow students 
seemed to be always wanting to simply memorize the mathematics in order to pass but without understanding. It occured to me at that
time if I could get a firm grasp on the underlying mathematics, I would be well on my way to understanding any future field of STEM
I chose to persue. Thus I elected to persue a Mathematics degree, which I completed in June 2017 from the University of Washington,
Bothell. In my upper level mathematics courses, I did not shy away from enrolling in some of the most challanging available
electives, many of which were in the subfield of Nurmerical/Computational methods. It was during my time in undergrad that I had my
first exposure to OpenGL/GLSL through incredibly accessible websites such as 'ShaderToy' ( https://www.shadertoy.com/about ) and
'Book of Shaders' ( https://thebookofshaders.com ). 

After graduating, I elected to enroll into Seattle University's "Computer Science Fundamentals Cirtificate". It was during my time
here I became re-exposed to C++, which to me took on a whole new level of raw potential due to my mathematical background. In late
2017 I did some small projects for fun that were just simple console applications (see my other GitHub projects), which helped me 
solidify and apply many of the basic ideas while also helping me increase my confidence. In very late 2017 I had begun work on a 
full 2D-spaceship game built entirely in the console when during the Winter holiday (Christmas) my cousing suggested I take a look 
at the open-source project Glitter ( https://github.com/Polytonic/Glitter ). Glitter provides a basic example of all the boilerplate
required to have a functional OpenGL application for which I can not give enough credit to for providing me an entry-point into the
world of OpenGL Application programing.

My first few experiements in OpenGL were done during January 2018. At this time I was using my Apple Macbook Pro as my primary
development environment, which provided a familiar (if limited) envirment to learn the ropes. I elected to try to make something
similar to the 2D console-based space game, except instead of using ncurses I would code it targeting OpenGL 4.1. (Version 4.1 was
the last version of OpenGL supported by Apple, as I would soon find out. Rather unfortunatly, Apple's support of OpenGL had
been declining in recent years such that OpenGL 4.5 [released 2014] was at that time the latest version of the standard but yet
Apple had not been releasing support for OpenGL beyond 4.1 (which had released in 2010). This would later come back to haunt me).
The 'Space-Game' I produced at this time should still be available on Github, but I would recommend avoiding looking at any of the
code I wrote for that project for more than a few seconds at a time to avoid serious injury. Joking aside, the 'Space-Game' project
would wind up being my proving ground for what does and does not work when writing C++, with a lot of my ideas on how the code should
work falling into the latter category. I had a working (if unexciting) version of the 'Space-Game' around the end of March 2018, at 
which point I was running into so many issues with my design I decided cease further developement, debug what I had as much as I could
bear, and look to move on to a new project. 

During my early work on the 'Space-Game' project, I was following various tutorials on texturing, yet I kept running into this issue
with having my textures display improperly. The issue was that after every few rows of pixels would be a row which ended just 
a few rgb components short of the necessary row length. This would then fill in the missing pixels with the start of the next row, 
which shifted forward all of the image components in all the rows after it, which had the affect of giving each texture image a skewed
apearance. Eventually I discovered what GL_UNPACK_ALIGNMENT was, but before I knew about this I built into in my Texture class a
function for manually performing a computation that could fill in the missing components on certain texture images provided their
dimensions matched precisely a few pre-specified dimensions. This got them to display properly (with a few minor artifacts, but they
were minor enough that you wouldn't notice them until told they were there). Anyways, as part of this effort to get texures to display
properly, I purchased several computer graphics textbooks, one of which was Pawel Lapinski's 'Vulkan Cookbook', which provided a 
small section mentioning how to load a texture using the same library as the 'Space-Game' used (I thought the image-loader library
was the source of my texture issue at that time), plus as a bonus it included an introduction to Vulkan. I decided in early April
to begin working my way through this book.

I was still in school when I began this work on Vulkan, often finishing my homework quickly to try to get to learning more Vulkan. 
While I was at that time not fully prepared for the level of complexity and detail the Vulkan API asks of its programmers, I also
found myself learning quite a bit about how modern GPU hardware actually works. This also helped me become acutely aware of the 
numerous abuses my 'Space-Game' must put any OpenGL driver through just to get things to work. Luckily right around the time when 
I was getting to the end of the Vulkan book and contemplating returning to fix the 'Space-Game', Apple announced on June 5, 2018 
that they were deprecating OpenGL in favor of their new Metal 2.0 API. 

I must say in retrospect Apple really did me a favor there, since they provided me a great excuse to cleanly abandon the mess of a
project that had occupied a large part of the year thus far. I was thinking around this time that my next step would be to start
again from scratch like I had with 'Space-Game', except by using Vulkan.

I decided however at first to not completly abandon the 'Space-Game' project since it was after all a 'working' piece of software
which could provide a useful framework to try out new ideas. Thus I ended my development of 'Space-Game' by spending a week or so
learning the OpenAL API. OpenAL allowed me to add sound to the game. Luckily this OpenAL wrapper code I wrote could exist as almost
entirely seperate from the rest of the 'Space-Game' project, and while not entirly conforming to the style of code I would write today,
the code I wrote here was some of the first to introduce my current style. Perhaps more importantly, this code also holds the honorable 
distinction of being the first code I had written that wouldn't cause me to cringe while looking at it upon returning after
several weeks.

Needless to say, I discovered pretty quickly that I was not going to be able to just pick up and run with Vulkan the same way I 
did OpenGL, especially since I still had large parts of OpenGL to learn. To top it off, there was a chance that writing everything
in Vulkan could still result in a slower Application if I failed to write things such that they were unable to out-perform the
OpenGL driver. I looked  breifly at DirectX 12, but the Component-Object-Model gave me vauge flashbacks to several of my poorly
thought through and even more poorly implemented 'Space-Game' systems (if you are feeling brave, see 'Space-Game' files 
'WeaponTracker.h', 'GameEntityManager.h' and 'KineticWeaponManager.h', but don't say I didn't warn you). Perhaps there is a
chance Microsoft has discovered a way past all of the flaws and issues I was encountering when there are classes designed to 
initialize, distribute and manage many pointers/handles representing opaque 'tracker' classes which are then given out to any other
class that asks for one and has to manage incoming requests based off previously distributed pointers/handles. Perhaps if I were
1,000 programmers rather than a single individual and if I had a massive development budget combined with several decades to work out
the kinks rather than a recent college-grad with just my free-time to work, I could produce a system which operates effectivly on 
that type of design.

But alas, my time is limited so I soon decided to return to OpenGL. The difference this time however would be I would be using
a desktop PC running Windows 10, the Application would target OpenGL 4.5 (OpenGL 4.6 released several weeks after I started), and 
I would have a clean start to do thing right from day 1. This time I would also be using smart pointer as much as possible, and 
taking advantage of features added in modern C++ updates. Since I had already done several iterations of CG projects with low-level
APIs, I knew this time what I wanted my code to do and I had had enough experience writing bad code I knew what to avoid. 
Thus this brings us to when I wrote those first few lines of main(). However, I almost immediatly broke my rule and imported 
a lot of my 'Space-Game's GLFW_Init class (while rewritting the most egregious parts) to get GLFW up and running quickly. I am still
in the process currently of figuring out how to replace this old foundational code (see Graphics Language Framework version 1
('Space-Game' import version) and version 2 (still in progress rewrite).

[Current Portablility Issues]
Forrest Miller -- January 1, 2019
I just today tried to open and run this project on my friends computer and realized the project here has several deficiencies.
First off, attempting to clone the project straight into visual studio will result in all of the files apearing in a visual
studio project like normal, but the project will fail to build (and might blame the issue on the GLFW shared library being
missing). After rebuilding the GLFW dll on this other computer and still encountering failures to launch, I discovered the 
issue to be from when the files are cloned from GitHub into Visual Studio, Visual Studio opens them all up as a directory rather 
than open the project solution. Finding the project solution in the directory, right-clicking it and selecting 'Open' should 
open the project properly. But wait, there are still the following issues:
      -None of the '.obj' model files are on GitHub.
      -Project is still hardcoded to be run on a 4k monitor (flaw in the class GLFW_Init, which is set to soon be replaced)
      
