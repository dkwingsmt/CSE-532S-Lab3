#include "common.h"
#include "Director.h"
#include <string>

using namespace std;

#define ARGID_PORT    1
#define ARGID_IPADDR  2
#define ARGID_MINTHRD 3
#define ARGID_SCRIPT  4

const char *USAGE = " <port> <ip_address> <min_threads> <script_file>+";

void print_usage(const char *argv0) {
    cout << "Usage: " << argv0 << USAGE << endl;
}

int program(int argc, char **argv) {
    if (argc < (ARGID_SCRIPT+1)) {
        cout << "Need script file name.\n";
        print_usage(argv[0]);
        return ARGUMENT_ERROR;
    }

    int numberOfThreads = 0;
    try{
        numberOfThreads = argc > (ARGID_MINTHRD+1) ? stoi(argv[ARGID_MINTHRD]) : 0;
    }
    catch (...) {
        cerr << "3rd parameter expected as a lower bound (integer) on the number of threads" << endl;
        print_usage(argv[0]);
        return ARGUMENT_ERROR;
    }

    //TODO: Parse port id and ip address
    cout << "Client to listen on " << argv[ARGID_IPADDR] << ":" << argv[ARGID_PORT] << endl;

    vector<string> scripts_filename;
    for (int i = ARGID_SCRIPT; i < argc; i++) {
        scripts_filename.push_back(argv[i]);
        cout << "[New script] " << scripts_filename.back() << endl;
    }
    {
        Director director(scripts_filename, numberOfThreads);
        director.selectScript(1);
        getchar();
    }
    cout << "End of main." << endl;
    return 0;
}

int main(int argc, char **argv) {
    int return_code = 0;
    try {
        return_code = program(argc, argv);
    } catch (std::exception &e) {
        cerr << e.what() << endl;
        return_code = EXCEPTION;
    } catch (...) {
        cerr << "Something went wrong" << endl;
        return_code = EXCEPTION;
    }
    return return_code;
}
