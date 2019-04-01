#include "soda.h"
#include <vector>

void NameServer::main() {
	printer.print(Printer::Kind::NameServer, (char) States::Start);

	unsigned int assignedMachinesList[numStudents];
	VendingMachine * machineList[numVendingMachines];
	machines = machineList;
	assignedMachines = assignedMachinesList;

	// assign an intial machine to each student
	for (unsigned int i = 0; i < numStudents; i += 1) {
		assignedMachines[i] = i % numVendingMachines;
	}	

	// first allow all vending machines to register
	while (numRegistered < numVendingMachines) _Accept(VMregister);

	for (;;) {
		_Accept(~NameServer) {
			printer.print(Printer::Kind::NameServer, (char) States::Finished); 
			break;
		} or _Accept(getMachine, getMachineList);
	}
}

NameServer::NameServer( Printer & prt, unsigned int numVendingMachines, unsigned int numStudents ) : 
	printer(prt), numVendingMachines(numVendingMachines), numStudents(numStudents), numRegistered(0) {}

void NameServer::VMregister( VendingMachine * vendingmachine ) {
	printer.print(Printer::Kind::NameServer, (char) States::Register, vendingmachine->getId());
	machines[numRegistered] = vendingmachine;
	numRegistered += 1;
}

VendingMachine * NameServer::getMachine( unsigned int id ) {
	VendingMachine * assignedMachine = machines[ assignedMachines[id] ];
	printer.print(Printer::Kind::NameServer, States::NewVending, (int) id, assignedMachine->getId() );
	assignedMachines[id] = (assignedMachines[id] + 1) % numVendingMachines; // advance to next machine
	return assignedMachine;
}

VendingMachine ** NameServer::getMachineList() {
	return machines;
}
