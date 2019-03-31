#include "soda.h"
#include <vector>

void NameServer::main() {
	// first allow all vending machines to register
	while (machines.size() < numVendingMachines) _Accept(VMregister);

	for (;;) {
		_Accept(~NameServer) {
			printer.print(Printer::Kind::NameServer, (char) States::Finished); 
			break;
		} or _Accept(getMachine, getMachineList);
	}
}

NameServer::NameServer( Printer & prt, unsigned int numVendingMachines, unsigned int numStudents ) : 
printer(prt), numVendingMachines(numVendingMachines) {
	printer.print(Printer::Kind::NameServer, (char) States::Start);
	machines = vector<VendingMachine *>();
	assignedMachines = vector<unsigned int>(numStudents);
	for (unsigned int i = 0; i < numStudents; i += 1) {
		assignedMachines[i] = i % numVendingMachines;
	}
}

void NameServer::VMregister( VendingMachine * vendingmachine ) {
	printer.print(Printer::Kind::NameServer, (char) States::Register, machines.size() );
	machines.push_back(vendingmachine);
}

VendingMachine * NameServer::getMachine( unsigned int id ) {
	int machineId = assignedMachines[id];
	VendingMachine * assignedMachine = machines[machineId];
	printer.print(Printer::Kind::NameServer, States::NewVending, (int) id, machineId );
	assignedMachines[id] = (assignedMachines[id] + 1) % numVendingMachines; // advance to next machine
	return assignedMachine;
}

VendingMachine ** NameServer::getMachineList() {
	return &machines[0];
}
