#include <utility>
#include "soda.h"

BottlingPlant::BottlingPlant( Printer & prt, NameServer & nameServer, unsigned int numVendingMachines,
				 unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
				 unsigned int timeBetweenShipments ): 
        printer( prt ), nameServer( nameServer ), numVendingMachines( numVendingMachines ), 
        maxShippedPerFlavour( maxShippedPerFlavour ), maxStockPerFlavour( maxStockPerFlavour ),
        timeBetweenShipments( timeBetweenShipments ), shutdown(false) {
    truck = new Truck( printer, nameServer, *this, numVendingMachines, maxStockPerFlavour );
}

void BottlingPlant::main() {
	printer.print( Printer::BottlingPlant, BottlingPlant::Start );
	for ( int i = 0; i < NUMFLAVOURS; i++ ) flavourStock[i] = 0;
    for ( ;; ) {
		unsigned int generated = 0;
        for ( int i = 0; i < NUMFLAVOURS; i++ ) {
            flavourStock[i] += mprng( maxShippedPerFlavour );
			generated += flavourStock[i];
        }
		printer.print( Printer::BottlingPlant, BottlingPlant::Generate, ( int ) generated );

        _Accept( ~BottlingPlant ) {
            shutdown = true;
			try {
            	_Accept( getShipment );
			} catch ( uMutexFailure::RendezvousFailure & ) {}
            delete truck;
            break;
        } or _Accept( getShipment ) {
			printer.print( Printer::BottlingPlant, BottlingPlant::PickedUp );
		}
    }
	printer.print( Printer::BottlingPlant, BottlingPlant::Finished );
}

void BottlingPlant::getShipment( unsigned int cargo[] ) {
    if ( shutdown ) _Throw Shutdown();
    for ( int i = 0; i < NUMFLAVOURS; i++ ) {
        std::swap( flavourStock[i], cargo[i] );
    }
}