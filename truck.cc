#include "soda.h"

void Truck::main() {
	printer.print(Printer::Kind::Truck, (char) States::Start);
	VendingMachine ** machines = nameServer.getMachineList();

	unsigned int cargo[NUMFLAVOURS];	// stores amount of each flavour on the truck

	for (;;) {							// every iteration is a new delivery run
		// clear the cargo
		for ( int flavId = 0; flavId < NUMFLAVOURS; flavId += 1 ) {
			cargo[flavId] = 0;
		}
		yield(mprng(1, 10));

		try {
			_Enable {
				plant.getShipment(cargo);			
			}
		} catch (BottlingPlant::Shutdown &) {	// if the plant is shutdown stop deliveries
			printer.print(Printer::Kind::Truck, (char) States::Finished);
			break;
		}

		// find the total number of bottles cargo
		unsigned int totalBottles = 0;
		for ( int flavId = 0; flavId < NUMFLAVOURS; flavId += 1 ) {
			totalBottles += cargo[flavId];
		}
		printer.print(Printer::Kind::Truck, (char) States::PickUp, (int)totalBottles);
		for (unsigned int i = 0; i < numVendingMachines; i += 1) {
	    	if (totalBottles == 0) break; 	// deliver to each machine until there are no more bottle in cargo
	    	printer.print(Printer::Kind::Truck, (char) States::Delivery, machineId, (int)totalBottles);

			unsigned int * inventory = machines[machineId]->inventory();
			unsigned int notReplenished = 0;	// how many bottles did the machine need but not get
			for ( int flavId = 0; flavId < NUMFLAVOURS; flavId += 1 ) { 
	        	unsigned int numNeeded = maxStockPerFlavour - inventory[flavId];
	        	unsigned int numSupplied;
	        	if (numNeeded > cargo[flavId]) { // machine needs more bottle of flavId than are in cargo
	        		numSupplied = cargo[flavId];
	        		notReplenished += numNeeded - cargo[flavId];
	        	} else {
	        		numSupplied = numNeeded;
	        	}

	        	// now move the bottles from the machine to the truck
	        	inventory[flavId] += numSupplied;
	        	cargo[flavId] -= numSupplied;
	        	totalBottles -= numSupplied;
	    	}

	    	if (notReplenished != 0) { 	// flavId was not filled to capacity in the machine
	    		printer.print(Printer::Kind::Truck, (char) States::Unsuccess, machineId, (int)notReplenished);
	    	} 
	    	machines[machineId]->restocked();	// signal to the truck the transfer is done
	    	printer.print(Printer::Kind::Truck, (char) States::EndDelivery, machineId, (int)totalBottles);
	    	machineId = (machineId + 1) % numVendingMachines; // advance to the next vending machine
		}
	}
}

Truck::Truck( Printer & prt, NameServer & nameServer, BottlingPlant & plant,
	unsigned int numVendingMachines, unsigned int maxStockPerFlavour )
	: printer(prt), nameServer(nameServer), plant(plant), machineId(0), 
	numVendingMachines(numVendingMachines), maxStockPerFlavour(maxStockPerFlavour) {}