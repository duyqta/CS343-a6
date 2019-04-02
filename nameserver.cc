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
	while (numRegistered < numVendingMachines) {
		_Accept(VMregister) {
			printer.print(Printer::Kind::NameServer, (char) States::Register, 
						  machines[numRegistered - 1]->getId());
		} //Note this does not accept destructor
	}

	for (;;) {
		_Accept(~NameServer) {
			printer.print(Printer::Kind::NameServer, (char) States::Finished); 
			break;
		} or _Accept(getMachine) {
			printer.print(Printer::Kind::NameServer, States::NewVending, 
						  (int) studentId, assignedMachine->getId() );
		} or _Accept(getMachineList);
	}
}

NameServer::NameServer( Printer & prt, unsigned int numVendingMachines, unsigned int numStudents ) : 
	printer(prt), numVendingMachines(numVendingMachines), numStudents(numStudents), numRegistered(0) {}

void NameServer::VMregister( VendingMachine * vendingmachine ) {
	machines[numRegistered] = vendingmachine;
	numRegistered += 1;
}

VendingMachine * NameServer::getMachine( unsigned int id ) {
	assignedMachine = machines[ assignedMachines[id] ];
	studentId = id;
	assignedMachines[id] = (assignedMachines[id] + 1) % numVendingMachines; // advance to next machine
	return assignedMachine;
}

VendingMachine ** NameServer::getMachineList() {
	return machines;
}
