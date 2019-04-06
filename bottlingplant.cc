#include <utility>
#include "soda.h"

BottlingPlant::BottlingPlant( Printer & prt, NameServer & nameServer, unsigned int numVendingMachines,
				 unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
				 unsigned int timeBetweenShipments ): 
        printer( prt ), nameServer( nameServer ), numVendingMachines( numVendingMachines ), 
        maxShippedPerFlavour( maxShippedPerFlavour ), maxStockPerFlavour( maxStockPerFlavour ),
        timeBetweenShipments( timeBetweenShipments ), shutdown(false) {}

void BottlingPlant::main() {
	// Bottling Plant starts 
	printer.print( Printer::BottlingPlant, BottlingPlant::Start );
    // Create a truck
    Truck truck ( printer, nameServer, *this, numVendingMachines, maxStockPerFlavour );

	// Initialize the stocks
	for ( int i = 0; i < NUMFLAVOURS; i++ ) flavourStock[i] = 0;

    for ( ;; ) {
		unsigned int generated = 0;

		// Random production of each flavour
        for ( int i = 0; i < NUMFLAVOURS; i++ ) {
            flavourStock[i] += mprng( maxShippedPerFlavour );
			generated += flavourStock[i];
        }
		printer.print( Printer::BottlingPlant, BottlingPlant::Generate, ( int ) generated );


        _Accept( ~BottlingPlant ) {					// Plant shutdown
            shutdown = true;						// From this point on, the plant is shutdown
			try {
            	_Accept( getShipment );				// Wait for last shipment call
			} catch ( uMutexFailure::RendezvousFailure & ) {}
            break;
        } or _Accept( getShipment ) {				// Shipment picked up
			printer.print( Printer::BottlingPlant, BottlingPlant::PickedUp );
		}
    }
	printer.print( Printer::BottlingPlant, BottlingPlant::Finished );
}

void BottlingPlant::getShipment( unsigned int cargo[] ) {
    if ( shutdown ) _Throw Shutdown();				// Plant shutdown, throw event at truck
    for ( int i = 0; i < NUMFLAVOURS; i++ ) {
        std::swap( flavourStock[i], cargo[i] );		// cargo assumed to be empty, swap
    }
}