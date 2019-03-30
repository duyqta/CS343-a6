#include "soda.h"

Parent::Parent( Printer & prt, Bank & bank, unsigned int numStudents, unsigned int parentalDelay ):
    printer( prt ), bank( bank ), numStudents( numStudents ), parentalDelay( parentalDelay ) {}

void Parent::main() {
    for ( ;; ) {
        _Accept ( ~Parent ) {
            printer::print( Printer::Parent, Finished );
            break;
        } _Else {
            yield( parentalDelay );
            int id = mprng( numStudents - 1 );
            int amount = mprng( 2 ) + 1;
            bank.deposit( id, amount );
            printer.print( Printer::Parent, Deposit, id, amount);
        }
    }
}