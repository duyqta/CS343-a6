#include "soda.h"

void Truck::main() {
	printer.print(Printer::Kind::Truck, (char) States::Start);
	VendingMachine ** machines = nameServer.getMachineList();

	unsigned int cargo[NUMFLAVOURS];

	for (;;) {
		yield(mprng(1, 10));
		try {
			_Enable {
				plant.getShipment(cargo);			
			}
		} catch (BottlingPlant::Shutdown &) {
			printer.print(Printer::Kind::Truck, (char) States::Finished);
			break;
		}

		int totalBottles = 0;
		for ( int flavId = 0; flavId < NUMFLAVOURS; flavId++ ) {
			totalBottles += cargo[flavId];
		}
		printer.print(Printer::Kind::Truck, (char) States::PickUp, totalBottles);
		for (unsigned int i = 0; i < numVendingMachines; i += 1) {
	    	if (totalBottles == 0) break;
	    	printer.print(Printer::Kind::Truck, (char) States::Delivery, machineId, totalBottles);

			unsigned int * inventory = machines[machineId]->inventory();
			int notReplenished = 0;
			for ( int flavId = 0; flavId < NUMFLAVOURS; flavId++ ) {
	        	unsigned int numNeeded = maxStockPerFlavour - inventory[flavId];
	        	unsigned int numSupplied;
	        	if (numNeeded > cargo[flavId]) {
	        		numSupplied = cargo[flavId];
	        		notReplenished = numNeeded - cargo[flavId];
	        	} else {
	        		numSupplied = numNeeded;
	        	}
	        	cargo[flavId] -= numSupplied;
	        	totalBottles -= numSupplied;
	    	}

	    	if (notReplenished != 0) {
	    		printer.print(Printer::Kind::Truck, (char) States::Unsuccess, machineId, notReplenished);
	    	} 
	    	printer.print(Printer::Kind::Truck, (char) States::EndDelivery, machineId, totalBottles);
	    	machineId = (machineId + 1) % numVendingMachines;
		}
	}
}

Truck::Truck( Printer & prt, NameServer & nameServer, BottlingPlant & plant,
	unsigned int numVendingMachines, unsigned int maxStockPerFlavour )
	: printer(prt), nameServer(nameServer), plant(plant), machineId(0), 
	numVendingMachines(numVendingMachines), maxStockPerFlavour(maxStockPerFlavour) {}