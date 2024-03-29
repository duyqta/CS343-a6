#include <iostream>
#include "soda.h"
using namespace std;

Printer::Printer( unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers ):
        numStudents( numStudents ), numVendingMachines( numVendingMachines ), 
        numCouriers( numCouriers ), numOfInput( 0 ),
        outputSize( 6 + numStudents + numVendingMachines + numCouriers ) {
    
    // Print the first row
    cout << "Parent\t" << "Gropoff\t" << "WATOff\t" << "Names\t"
         << "Truck\t" << "Plant";    
    for ( int i = 0; i < ( int ) numStudents; i++ ) cout << "\tStud" << i;
    for ( int i = 0; i < ( int ) numVendingMachines; i++ ) cout << "\tMach" << i;
    for ( int i = 0; i < ( int ) numCouriers; i++ ) cout << "\tCour" << i;
    cout << endl;

    // Print the dividing
    for (int i = 0; i < outputSize; i++){
	    cout << "*******";
	    if ( i < outputSize - 1 ) cout << "\t";
	    vOutput.push_back( new Info() );
    }
    cout << endl;
}

Printer::~Printer(){ 
    flush();	    // Print remaining output still waiting to be printed
    for ( int i = 0; i < outputSize; i++ ) delete vOutput[i];
    cout << "***********************" << endl; 
}

int Printer::getIndex( Kind kind, unsigned int lid = 0 ) {
    int index;
    if ( kind == Vending ) index = Student + numStudents + ( int ) lid;
    else if ( kind == Courier ) index = Student + numStudents + numVendingMachines + ( int ) lid;
    else index = ( int ) kind + ( int ) lid;
    return index;
}
void Printer::print( Kind kind, char state ) {
    int index = getIndex( kind );
    if ( !vOutput[index]->flushed ) flush();

    numOfInput++;
    vOutput[index]->state = state;
    vOutput[index]->flushed = false;
}
void Printer::print( Kind kind, char state, int value1 ) {
    int index = getIndex( kind );
    print( kind, state );
    vOutput[index]->value1 = value1;
    vOutput[index]->value1Avail = true;
}

void Printer::print( Kind kind, char state, int value1, int value2 ) {
    int index = getIndex( kind );
    print( kind, state, value1 );
    vOutput[index]->value2 = value2;
    vOutput[index]->value2Avail = true;
}

void Printer::print( Kind kind, unsigned int lid, char state ) {
    int index = getIndex( kind, lid );
    if ( !vOutput[index]->flushed ) flush();

    numOfInput++;
    vOutput[index]->state = state;
    vOutput[index]->flushed = false;
}
void Printer::print( Kind kind, unsigned int lid, char state, int value1 ) {
    int index = getIndex( kind, lid );
    print( kind, lid, state );
    vOutput[index]->value1 = value1;
    vOutput[index]->value1Avail = true;
}

void Printer::print( Kind kind, unsigned int lid, char state, int value1, int value2 ) {
    int index = getIndex( kind, lid );
    print( kind, lid, state, value1 );
    vOutput[index]->value2 = value2;
    vOutput[index]->value2Avail = true;
}

void Printer::flush() {
    int count = numOfInput;
    for ( auto it : vOutput ) {
	    if ( it->flushed ) {
	        cout << "\t"; // Nothing to output
	        continue;
	    }

	    cout << it->state;	// Print out the state

	    // The following prints out accompanying info with the states

	    if ( it->value1Avail ) cout << it->value1;
        if ( it->value2Avail ) cout << "," << it->value2;

	    // Finished printing for current subject, clear info
	    it->clear();
	
	    if ( --count == 0 ) break;			    // No more columns to print
	
	    cout << "\t";
    }
    numOfInput = 0;
    cout << endl;
}
