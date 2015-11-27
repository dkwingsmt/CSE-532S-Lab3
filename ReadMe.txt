CSE 532 Fall 2015 Lab 3

Team Member: 
    Tong Mu (mutong32@gmail.com)
    Jinxuan Zhao (jzhao32@wustl.edu)
    Zeneil Ambekar (ambekar@wustl.edu)

== Usage ==
=== Unpack ===

Unpack the zip file with 7zip.

The extracted folder contains a copy of this ReadMe.txt, a sample_result folder
containing sample output from the executables, and the solution folder
for this assignment.

> lab3\
> |- ReadMe.txt
> |- sample_result\
> |- lab3\
>    |- director\
>    |- producer\
>    |- common\
>    |- configs\
>    |- lab3.sln
>    |- ...

The solution folder consists of the following folders,

- director, for the VS project director
- producer, for the VS project producer
- common, headers and utility functions common across projects.  
- configs, sample configuration files for testing. 

=== Build ===

Open lab3.sln in the solution folder with VS 2012, and build the solution.
This solution consists of 2 VS projects, lab2 and lab2_generator,
and should generate two executable files in Debug or Release folder in the 
solution folder, depends on which profile is choosed.

=== Run ===

Both executables requires command line arguments to run properly; they will 
prompt usage information if no arguments are provided.

The usage for director.exe is

    director.exe <port> <ip_address> <min_threads> <script_file>+

It reads and parses the play files, finds corresponding per-character 
files in the SAME DIRECTORY as the config file, and connects to producer 
at the specified IP address and port.

The usage for producer.exe is

    producer.exe <port>

It listens at the specified port. After receiving connection from some director,
it can manimuplates that.

== Description ==
=== Overview ===

1: General structure

// TODO

=== Director module ===

The director module is the collection of the Director class, Play class and
Player class. It is designed as an extension to the one from lab2, with the 
support of
(1) assigning multiple plays at construction,
(2) unblocked acting,
(3) specifying the play to act at runtime,
(4) stopping a play during its acting, 
(5) starting another play when one play has stopped,
(6) calling a specified callback when a play ends (for any reason).

Almost everything in the Director class from lab2 are moved to a 
separate class called Script, which represents the acting of one play.
A Director instance have zero or one Script member at one time.

The Script class is designed to spawn necessary threads when constructed,
and to stop immediately when destructed. Therefore the Director class can 
translate the start command and stop command into newing and deleting its
Script member.

An atomic boolean Play::_ended is added to the Play class to shut down 
the running child threads. It is added to the condition of every waiting
processes (most importantly, the one in Play::recite()). Every time someone
awakes from the waiting of a condition variable, it first checks if it is 
because of the Play has ended; if so, it returns false and thus terminates
the thread.

In lab2, we left the responsibility of repeatedly calling Director::electDirector()
to the main function. Since there was only one active director at a time, 
the main thread had to wait for one act to end before electing the next, 
therefore it was almost fully blocked during the whole program. In this lab,
in order to allow the cooperation of the director module and the network 
module, we moved this responsibility into another independent thread within
Script class. It is also controlled by Play::_ended.

Finally, we allow Director to receive a function as the callback at the end of 
play. It is stored in Director, and a method of Director that calls this callback
is further passed into Script and is called whenever the play ends for any 
reason.

== Insights, Observations and Questions encountered == 

	
== Acknowledgement ==

This project refers to no code from previous assignment by students outside of
this team.
