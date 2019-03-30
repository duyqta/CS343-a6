#include "soda.h"
#include <vector>

using namespace std;

Bank::Bank( unsigned int numStudents ) {
	accounts =  vector<unsigned int>(numStudents, 0);
}

void Bank::deposit( unsigned int id, unsigned int amount ) {
	accounts[id] += amount;
}

void Bank::withdraw( unsigned int id, unsigned int amount ) {
	while (accounts[id] < amount) _Accept(deposit);
	accounts[id] -= amount;
}