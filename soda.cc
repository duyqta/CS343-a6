#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "config.h"
#include "soda.h"
#include "MPRNG.h"
using namespace std;				// direct access to std
#include <cstdlib>					// exit

MPRNG mprng;

bool convert( int & val, char * buffer ) {		// convert C string to integer
    std::stringstream ss( buffer );			// connect stream and buffer
    string temp;
    ss >> dec >> val;					// convert integer from buffer
    return ! ss.fail() &&				// conversion successful ?
	! ( ss >> temp );				// characters after conversion all blank ?
} // convert

int main( int argc, char * argv[] ) {
    // MUST BE INT (NOT UNSIGNED) TO CORRECTLY TEST FOR NEGATIVE VALUES
    int seed = getpid();					// default value
    char defaultFile[] = "soda.config";

    try {
	switch ( argc ) {
	  case 3:
		if ( ! convert( seed, argv[2] ) || seed <= 0 ) throw 1; // invalid ?
	    // FALL THROUGH
	  case 2:
	    // FALL THROUGH
	  case 1:					// defaults
	    break;
	  default:					// wrong number of options
	    throw 1;
	} // switch
    } catch( ... ) {
	cerr << "Usage: " << argv[0]
	     << " [ config-file [ random-seed (> 0) ] ]" << endl;
	exit( EXIT_FAILURE );				// TERMINATE
    } // try

	mprng.set_seed(seed);

	ConfigParms cparms;
	if (argc >= 2) {
		processConfigFile(argv[1], cparms);
	} else {
		processConfigFile(defaultFile, cparms);
	}

	cout << "seed: " << seed << " numStudents: " << cparms.numStudents << endl;

	// Printer printer(cparms.numStudents, cparms.numVendingMachines, cparms.numCouriers);
	// Bank bank(cparms.numStudents);
	// Parent parent(printer, bank, cparms.numStudents, cparms.parentalDelay);
	// WATCardOffice office(printer, bank, cparms.numCouriers);
	// Groupoff groupoff(printer, cparms.numStudents, cparms.sodaCost, cparms.groupoffDelay);
	// NameServer nameServer(printer, cparms.numVendingMachines, cparms.numStudents);

	// VendingMachine * machines[cparms.numVendingMachines];
	// for (unsigned int i = 0; i < cparms.numVendingMachines; i += 1) {
	// 	machines[i] = new VendingMachine(printer, nameServer, i, cparms.sodaCost);
	// }


	// BottlingPlant plant(printer, nameServer, cparms.numVendingMachines,
	// 					cparms.maxShippedPerFlavour, cparms.maxStockPerFlavour,
	// 					cparms.timeBetweenShipments);

	// Student * students[cparms.numStudents];
	// for (unsigned int i = 0; i < cparms.numStudents; i += 1) {
	// 	students[i] = new Student(printer, nameServer, office, groupoff, i, cparms.maxPurchases);
	// }

	// for (unsigned int i = 0; i < cparms.numStudents; i += 1) {
	// 	delete students[i];
	// }
	// plant.~BottlingPlant();
	// for (unsigned int i = 0; i < cparms.numVendingMachines; i += 1) {
	// 	delete machines[i];
	// }

} // main
