#include "soda.h"
#include <vector>
#include <iostream>

void VendingMachine::main() {
	printer.print(Printer::Kind::Vending, id, (char) States::Start, (int) sodaCost);
	nameServer.VMregister(this);
	for (unsigned int i = 0; i < NUMFLAVOURS; i += 1) {
		inv[i] = 0; // set inital inventory to 0 for all flavours
	}
	for (;;) {
		_Accept(~VendingMachine) {
			printer.print(Printer::Kind::Vending, id, (char) States::Finished);
			break;
		} or _Accept(buy) { // priority goes to buy over delivery
			if (currCard->getBalance() < sodaCost) { 
				exceptToThrow = FundsEx;
			} else if (inv[currFlavour] == 0) { // the requested flavour is not available
				exceptToThrow = StockEx;
			} else if (mprng(4) == 0) { 		// does the machine show an advertisment		
				exceptToThrow = FreeEx;
			} else {							// soda is bought
				exceptToThrow = NoEx;
				currCard->withdraw(sodaCost);
				inv[currFlavour] -= 1;
				printer.print(Printer::Kind::Vending, id, (char) States::Bought, (int) currFlavour, inv[currFlavour]);
			}
			buyCond.signalBlock();
		} or _Accept(inventory) {
			printer.print(Printer::Kind::Vending, id, (char) States::Reloading);
			_Accept(restocked, ~VendingMachine);
			printer.print(Printer::Kind::Vending, id, (char) States::CompleteReloading);
			//cout << inv[0] << " " << inv[1] << " " << inv[2] <<" "<<  inv[3] << endl;
		}
	}
}

VendingMachine::VendingMachine( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int sodaCost ) :
printer(prt), nameServer(nameServer), id(id), sodaCost(sodaCost) {}

void VendingMachine::buy( Flavours flavour, WATCard & card ) {
	currFlavour = flavour;
	currCard = &card;
	buyCond.wait();
	switch(exceptToThrow) { // check if an exception needs thrown and if it does then throw it
		case FreeEx:
			_Throw Free();
		case FundsEx:
			_Throw Funds();
		case StockEx:
			_Throw Stock();
	}
}

unsigned int * VendingMachine::inventory() {
	return inv;
}

void VendingMachine::restocked() {}

_Nomutex unsigned int VendingMachine::cost() const {
	return sodaCost;
}

_Nomutex unsigned int VendingMachine::getId() const {
	return id;
}