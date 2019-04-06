#include "soda.h"
#include <vector>

using namespace std;

Bank::Bank( unsigned int numStudents ) {
	// the inital ballance of all accounts is set to 0
	accounts =  vector<unsigned int>(numStudents, 0);
}

void Bank::deposit( unsigned int id, unsigned int amount ) {
	accounts.at(id) += amount;
}

void Bank::withdraw( unsigned int id, unsigned int amount ) {
	// if there is not enough money wait until enough has been deposited
	while (accounts.at(id) < amount) _Accept(deposit);
	accounts.at(id) -= amount;
}