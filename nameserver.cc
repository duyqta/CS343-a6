#include "soda.h"
#include <vector>

void NameServer::main() {
	printer.print(Printer::Kind::NameServer, (char) States::Start);

	// student i should always use machines[ assignedMachines[i] ] next
	unsigned int assignedMachines[numStudents];	
	machines.resize(numVendingMachines);

	// assign an intial machine to each student
	for (unsigned int i = 0; i < numStudents; i += 1) {
		assignedMachines[i] = i % numVendingMachines;
	}	

	// wait for all vending machines to register
	while (numRegistered < numVendingMachines) {
		_Accept(VMregister) {
			printer.print(Printer::Kind::NameServer, (char) States::Register, 
						  machines.at(numRegistered - 1)->getId());
		}
	}

	for (;;) {
		_Accept(~NameServer) {
			printer.print(Printer::Kind::NameServer, (char) States::Finished); 
			break;
		} or _Accept(getMachine) {
			chosenMachine = machines.at(assignedMachines[studentId]); 	// choose next machine for student
			assignedMachines[studentId] = (assignedMachines[studentId] + 1) 
										  % numVendingMachines; 		// advance to next machine
			printer.print(Printer::Kind::NameServer, States::NewVending, 
						  (int) studentId, chosenMachine->getId() );
			assigningCond.signalBlock(); 	// wake up the student calling getMachine
		} or _Accept(getMachineList);
	}
}

NameServer::NameServer( Printer & prt, unsigned int numVendingMachines, unsigned int numStudents ) : 
	printer(prt), numVendingMachines(numVendingMachines), numStudents(numStudents), numRegistered(0) {}

void NameServer::VMregister( VendingMachine * vendingmachine ) {
	machines.at(numRegistered) = vendingmachine;
	numRegistered += 1;
}

VendingMachine * NameServer::getMachine( unsigned int id ) {
	studentId = id;
	assigningCond.wait(); 	// wait for the machine to be assigned
	return chosenMachine;
}

VendingMachine ** NameServer::getMachineList() {
	return &machines[0];
}
