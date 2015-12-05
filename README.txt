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
This solution consists of 2 VS projects, director and producer. 
Before we build them, open up their project properties dialog window, and under
Congfiguration Properties -> C/C++ -> General -> Additional Include Directories 
please give the fully qualified path to the ACE_wrappers directory.
Also, under Congfiguration Properties -> Linker -> General -> Additional Library Directories 
please give the fully qualified path to the ACE_wrappers\lib directory, and 
under Congfiguration Properties -> Linker -> Input -> Additional Dependencies 
please add ACEd.lib (for the ACE debug DLL).
Then we can build the two project separately. 

== After Build ==

Before running the programs, be sure to copy the ACEd.dll library to the same
folder as the programs.

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

== Producer module ==
The Producer module is essentially a server module consisting of the following major parts
(1) Console Active Object
(2) Serializaion / Inflation
(3) Message Passing Framework
(4) Client Communication Framework

The producer module heavily uses message passing and notifications to handle the events generated
by the user through the console.

The producer program has two active threads when it is running - the console active object thread
and the ACE_Reactor event loop thread.

The processing of the event loop thread is limited to mainly just network events to limit the amount
of overhead that each of the client handlers incurs to the ACE_Reactor thread to make sure it's
responsive. Each of the handlers sends across very small messages to make sure that it's handle_output
method returns as fast as possible.

=== Network module ===

1: Director program
	(1)MessageHandler
		The message handler is the main handler for interacting with the 
		producer. 
		It receives message from the producer via handle_input and parses 
		the message buffer and then calls the director to react. 
		
	(2)SignalHandler
		This handler handles the Ctrl-C termination protocol. 

2: Producer Program
	(1)ClientHandler
		This class is responsible for processing the messages generated by the console module and relies on
		an internal message queue to send messages serialized messages across to the client.

		Each event handler is notified when the console thread has generated some event based on input from
		the user (via console). The notification is packaged up as a message object which is passed on to the
		respective handler's message queue, and finally a notification to ACE_Reactor schedules the handle_output
		method of the respective handler to be run in the next event reactor loop.

		The instantiation and destruction of this class object is controlled with the ConnectionHandler object 
		(which a typedef for a parameterized Acceptor object). The ClientHandler passes a reference to itself
		to the central DirectorRegister static singleton which manages all of the current client connections

	(2)ISerializer
		Serialization is managed with the ISerializer interface, which defines the methods needed by the
		ClientHandler object to serialize objects while writing them out and inflating them before they
		are read.

		The design of the Serialization is done keeping dependency injection in mind. Because of this it is
		fairly easy to change the codec without effecting the functionality of the application. This is achieved
		with the help of a file called aspect.h, where a template parameter defines the default serializer used
		by the components in the module.

	(3)Comms
		The comms module manages the Acceptor object's lifecycle and initialization. This object then spawns
		ClientHandlers when it detects a director client wishes to connect to the server. This keeps the main
		function code clean.

=== Design details ===

1: Director Program
	 
	(2) Design of class MessageHandler:
		1): In main, this handler is created with the director, which is
			initialized by the script files and number of threads. 
			The director pointer is a data member of the handler, and is 
			the enter point to react upon the signals from the producer. 
			
			Also, when handler is created, we specify the callback function in 
			Script onActEnd to be a function that sends an AVAILABLE signal to the 
			producer. 
			In this case, each time when the script is finished or stopped, the 
			AVAILABLE signal will be sent to the producer. 
		
		2): The function sendFeedBack sends 3 types of messages. 
			REGISTER 0, which is followed by script names. 
			AVAILABLE 1, which is just a signal to notify the producer. 
			NULL_MSG 2, which notifies the producer that the director is quitted 
			or tweminated by Ctrl+c
			
			
		3): The function sendScriptfile is used to send script files to the producer and is 
			called in main once the connection to producer is successful. The scripts 
			are sent in the form of char* buffer and acript names are delimited by "#"
		4): the overloaded handle_input function receives the message from producer and 
			pass it directly to the function processMessage
			
		5): The function processMessage parses the message buffer received from the producer 
			and  assigns the correspondent behaviour to the director. 
			
	(3) Design of class SignalHandler:
		We override the method  int handle_signal(int signum, siginfo_t*,ucontext_t*) , so that once 
		the signal SIGINT is catched, it calls the reactor to end the event loop and the program will end.
		
2: Producer Program 
	(1)Console Active Object / Console Management			
		1. ConsoleUtils
			1. This is the active object that manages the console thread for input from the
				user, and is reponsible for the menu loop
			2. It is a singleton object class, with a private constructor. It is instantiation
				is lazy and is not thread safe, as it is only instantiated from the main thread
			3. It has an internal shutdown variable which prints the menu till it detects that
				it has been set to false.
			4. The doMenu() method does most of the work of printing the menu. It does this by
				getting a copy of the current director-play status through the shared DirectorRegister 
				instance. The DirectorRegister stores the current connection information in the form
				of maps, which means that the order of display might not be consistent, but the display 
				to the console needs to be. This is why doMenu creates the menu based on the order 
				retrieved from the DirectorRegister singleton object and stores it for the time of 
				the menu display. It is then able to map the currently selected option to the map of 
				directorId and playNumber retrieved from the DirectorRegister
			5. It allows the ACE SignalHandler to handle the SIGINT sent by the user by 
				checking the fail bit on the std::cin object which is, in the context of 
				the application, only set by the the SIGINT input to the console
		2. ConsoleLocker
			1. This object is a singleton that manages the exclusive access to the console, by
				making sure all of the console output passes through its execute method.
			2.  The execute method runs any std::function<void(void)> code within its internal
				recurrsive_mutex. This method requires a recurrsive mutex to make sure that the 
				code is never deadlocked as running arbitrary outside client code has the potential
				of causing a deadlock within the same thread if the same function is called within
				the outside code. A recurrsive_mutex prevents this.
			3. It is a class that only allows a singleton object with lazy instantiation, that is 
				thread safe with the use of call_once semantic and a once_flag. This is because the
				console can be accessed by either the ConsoleUtils for displaying the menu as well
				as the ACE_Reactor thread for displaying notifications. This entails the exitance of
				a race condition between the reactor thread and the ConsoleUtils active object for
				access to the console.
			4. ConsoleLocker.h also defines a CL_OUT macro for easy console output.

	(2) Serialization / Inflation
		1. ISerializer
			This is the interface that is used by the ClientHandler objects to serialize / inflate base objects (ClientMessage / ServerMessage).
		2. Serializers
			This is the template class that is a parameterized wrapper facade for accessing an ISerializer interface containing object. It is
			used for the parameterization of the DefaultSerializer object defined in AspectConfig.h
		3. SimpleSerializer
			This is a simple implementation of the ISerialization interface that does the job of parsing a string into a fully inflated object
			and vice versa
		4. ClientMessage (Base / API Object)
			1. This is a simple base structure that represents a client message that is sent to the server.
			2. This class defines the ClientMessageType enum that identifies the type of the message begin sent.
				a. REGISTER: This means that the director client is ready to register and that message contains within it the name of the plays that're	
					avaiable to it in the correct order
				b. AVAILABLE: This means that the director client has finished the rendition of whichever play it was assigned latest
				c. NULL_MSG: This message is never sent by the client, but rather marks the disconnection event of the client from the server
		5. ServerMessage (Base / API Object)
			1. This is a simple base structure that repesents a server message that is generated as a response to the console events and is sent
				to the client (after being serialized)
			2. This class defines the ServerMessageType enum that identifise the type of the message being sent
				a. START: This means that the server wishes to start a play on the director client object
				b. STOP: This means that the server wants the director to stop whatever it's playing
				c. POISON: This means that the server is about to shutdown and wishes to close the director client

	(3) Message Passing Framework / Backbone
		1. MessageQueue
			1. This is the message queue data structure that is included as a member variable in every ClientHandler object.
			2. It uses an internal mutex to lock accesses to an internal queue object which could courrupt the messages inside
		2. DirectorRegister
			1. This is the backbone of the server and manages the message passing and keeps an audit of all the connected clients
			2. This is a class that contains a singleton object that is made available to both the threads in the application
			3. The singleton instantiation is lazy and thread safe to prevent a race condition.
			4. The registration of clients is handled by identifiers that are generated and given to the respective handlers to be stored.
			5. The identifiers are integers that serve as keys to the internal map variables.
			6. The following maps are maintained by the DirectorRegister object
				1. directorIdentifierToHandler: 
					This map stores the ClientHandler pointer against the director identifier
				2. directorToPlayNumberBusy:
					This map stores the play number that the director whose identifier is stored against it, is busy playing, if the
					director is free it contains a -1
				3. directorIdToPlays:
					This map stores the ordered vector of plays that the director whose identifier is stored against it has. Since the vector is
					ordered, plays can be identified by indices
			7. The object also has a member mutex variable that prevents its internal database to be corrupted by access by multiple
				threads (the console thread and the reactor thread)
			8. An exit_initiated variable stores whether shutdown has been initialized, and prevents any further registration of directors
				in the data structure
			9. Whenever a command message such as play, stop or exit is posted to the respective ClientHandler's messageQueue, the
				DirectorRegister object also calls notify on the ACE_Reactor object with a pointer to the respective handler and 
				a WRITE mask to indicate to the Reactor framework that the handler is ready to send data across to the client.
				This makes the framework call the handle_output method on that particular reactor, next time in the reactor event loop.

	(4) Client Communication Framework
		1. ClientHandler
			1. The ClientHandler object is an ACE_Svc_Handler
			2. This object is responsible for the communication with the director client object
			3. It contains a private MessageQueue object for storing the messages that it is supposed to process, which are generated
				by the user (via the console)
			4. The DefaultSerializer is an instance of the parameterized serialization scheme, which it uses internally in getClientMessage()
				function
			5. The handle_output method polls the internal message queue for a message which is sent across to the server, before being checked
				for its type - which if found to be POISON - causes the ClientHandler to shutdown
			5. The open hook of this object is overridden as it is called by the Acceptor object right after accepting the connection,
				in which the default superclass method would register itself with the Reactor for just a READ mask. However, this clientHandler
				is responsible for reading data from and writing data to the client object. Hence, the override registers it with the WRITE 
				mask as well
			6. Since the instantiation of these objects is managed by the Acceptor object inside comms, the only way to pass a reference
				to the DirectorRegister object is in the handle_input method when, a new registration message is sent by the client

	(5) Termination Protocol
		1. ExitSignalHandler
			1. This is an ACE_Event_Handler that is registered with the Reactor to handle SIGINT sent by the user
			2. When this handler detects that a SIGINT has been sent to it, it calls the exit method on the DirectorRegister object to initiate
				shutdown, and unregisters itself from the reactor event loop by returning -1
		2. DirectorRegister's exit handler (exit() method)
			1. This method first checks if there are any connected clients, if not, it calls the end_reactor_event_loop on the shared ACE_Reactor object
				and returns. This is done because if there are no clients connected, then the termination protocol is trivial
			2. It then loops for each active connected director, and posts the POISON message to the respective handler.
			3. The posted message is processed in the processMessage() method of the client handler, which sends the message across and then unregisters
				itself returning a -1
			4. Before returning this however, it has to make a call to DirectorRegister::removeDirector, which now checks if there are any remaining 
				directors that are connected (which happens within a mutex to avoid race conditions), and if not initiates a shutdown of the reactor event loop
			5. After the shutdown of the reactor's event loop, the ACE_Reactor single is closed to make sure there are no dangling objects left
				on the stack after before main returns
			6. Finally, the applicationGuard TGuard object runs it's lambda code which shuts down all of the remaining singleton instances, including ConsoleUtils
				active object. The destructor of the active makes sure that the console thread is done displaying the exit message, by checking if the internalThread
				of the ConsoleUtils active object is joinable and if so waits to join it.

== Insights, Observations and Questions encountered == 

	
== Acknowledgement ==

This project refers to no code from previous assignment by students outside of
this team.
