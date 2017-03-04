#include <iostream>
#include <iomanip>
#include <stdio.h>
#include "networkhandler.h"

using std::cout;
using std::endl;

/*
    Aplication takes 4 arguments :
    1. role (initiator or responder)
    2. mode (test, version, algorithm)
            test      - run key negotiation with basic set of supported function
            version   - demonstrate version negotiation (initiator support 1.10, 2.00)
                                                        (responder support 1.10, 1.40)
            algorithm - demonstrate key algorithm negotiation (initiator support DH2k, DH3k, EC25)
                                                              (responder support EC38, DH3k, EC25)

    3. number of tests (only for test mode)
    4. 1 - write out to terminal
       0 - do not write to terminal

    (in version and algoritm mode have count set default to 0)
*/

int main(int argc, char * argv[]){

    // Check role parameters.
    if ((argc < 5) || ((strcmp(argv[1],"initiator") != 0) && (strcmp(argv[1],"responder") != 0))){
        cerr << "Wrong input parameters" << endl;
        return 0;
    }

    // Check mode
    if (((strcmp(argv[2],"version") != 0) && (strcmp(argv[2],"algorithm") != 0)) && (strcmp(argv[2],"test") != 0))      {
        cerr << "Wrong mode option" << endl;
        return 0;
    }

    // 1 - write output to file, 0 do not write.
    if (*(argv[4]) != '0' && *(argv[4]) != '1'){
        cerr << "Wrong file option arguments" << endl;
        return 0;
    }

    QCoreApplication a(argc,argv);

        NetworkHandler* network;

        network = new NetworkHandler(argv);
        network->startZrtpNegotiation();

    return a.exec();

    delete(network);

    return 0;
}







