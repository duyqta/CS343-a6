#include "soda.h"

void Truck::main() {
	printer.print(Printer::Kind::Truck, (char) States::Start);
	VendingMachine ** machines = nameServer.getMachineList();

	unsigned int cargo[NUMFLAVOURS];


	for (;;) {
		// clear the cargo
		for ( int flavId = 0; flavId < NUMFLAVOURS; flavId += 1 ) {
			cargo[flavId] = 0;
		}
		yield(mprng(1, 10));
		try {
			_Enable {
				plant.getShipment(cargo);			
			}
		} catch (BottlingPlant::Shutdown &) {
			printer.print(Printer::Kind::Truck, (char) States::Finished);
			break;
		}

		unsigned int totalBottles = 0;
		for ( int flavId = 0; flavId < NUMFLAVOURS; flavId += 1 ) {
			totalBottles += cargo[flavId];
		}
		printer.print(Printer::Kind::Truck, (char) States::PickUp, (int)totalBottles);
		for (unsigned int i = 0; i < numVendingMachines; i += 1) {
	    	if (totalBottles == 0) break;
	    	printer.print(Printer::Kind::Truck, (char) States::Delivery, machineId, (int)totalBottles);

			unsigned int * inventory = machines[machineId]->inventory();
			unsigned int notReplenished = 0;
			for ( int flavId = 0; flavId < NUMFLAVOURS; flavId += 1 ) {
	        	unsigned int numNeeded = maxStockPerFlavour - inventory[flavId];
	        	unsigned int numSupplied;
	        	if (numNeeded > cargo[flavId]) {
	        		numSupplied = cargo[flavId];
	        		notReplenished += numNeeded - cargo[flavId];
	        	} else {
	        		numSupplied = numNeeded;
	        	}

	        	inventory[flavId] += numSupplied;
	        	cargo[flavId] -= numSupplied;
	        	totalBottles -= numSupplied;
	    	}

	    	if (notReplenished != 0) {
	    		printer.print(Printer::Kind::Truck, (char) States::Unsuccess, machineId, (int)notReplenished);
	    	} 
	    	machines[machineId]->restocked();
	    	printer.print(Printer::Kind::Truck, (char) States::EndDelivery, machineId, (int)totalBottles);
	    	machineId = (machineId + 1) % numVendingMachines;
		}
	}
}

Truck::Truck( Printer & prt, NameServer & nameServer, BottlingPlant & plant,
	unsigned int numVendingMachines, unsigned int maxStockPerFlavour )
	: printer(prt), nameServer(nameServer), plant(plant), machineId(0), 
	numVendingMachines(numVendingMachines), maxStockPerFlavour(maxStockPerFlavour) {}