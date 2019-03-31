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
    for ( ;; ) {
        for ( int i = 0; i < NUMFLAVOURS; i++ ) {
            flavourStock[i] += mprng( maxShippedPerFlavour );
        }

        _Accept( ~BottlingPlant ) {
            shutdown = true;
            _Accept( getShipment );
            delete truck;
            break;
        } or _Accept( getShipment );
    }
}

void BottlingPlant::getShipment( unsigned int cargo[] ) {
    if ( shutdown ) _Throw Shutdown();
    for ( int i = 0; i < NUMFLAVOURS; i++ ) {
        std::swap( flavourStock[i], cargo[i] );
    }
}