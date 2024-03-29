#include "soda.h"

Parent::Parent( Printer & prt, Bank & bank, unsigned int numStudents, unsigned int parentalDelay ):
    printer( prt ), bank( bank ), numStudents( numStudents ), parentalDelay( parentalDelay ) {}

void Parent::main() {
    printer.print( Printer::Parent, ( char ) Parent::Start );
    for ( ;; ) {
        _Accept ( ~Parent ) {
            printer.print( Printer::Parent, ( char ) Parent::Finished );
            break;
        } _Else {
            yield( parentalDelay );
            unsigned int amount = mprng( 1, 3 );
            unsigned int id = mprng( numStudents - 1 );
            
            bank.deposit( id, amount );
            printer.print( Printer::Parent, ( char ) Parent::Deposit, (int) id, (int) amount);
        }
    }
}