#include "soda.h"

WATCardOffice::WATCardOffice( Printer & prt, Bank & bank, unsigned int numCouriers ):
    printer( prt ), bank( bank ), numCouriers( numCouriers ) {
    for ( int i = 0; i < ( int ) numCouriers; i++ ) 
        couriers.push_back( new Courier( printer, bank, this ) );    
}

WATCardOffice::~WATCardOffice() {
    for ( int i = 0; i < ( int ) numCouriers; i++ ) 
        delete couriers[i];   
}

WATCard::FWATCard WATCardOffice::create( unsigned int sid, unsigned int amount ) {
    lastId = sid;
    lastAmount = amount;
    Args args = { sid, amount, NULL, Args::Create };
    Job * job = new Job( args );
    jobQueue.push( job );
    return job->result;
}

WATCard::FWATCard WATCardOffice::transfer( unsigned int sid, unsigned int amount, WATCard * card ) {
    lastId = sid;
    lastAmount = amount;
    Args args = { sid, amount, card, Args::Transfer };
    Job * job = new Job( args );
    jobQueue.push( job );
    return job->result;
}

Job * WATCardOffice::requestWork() {
    _When ( jobQueue.empty() ) _Accept( create, transfer );
    Job * job = jobQueue.front();
    jobQueue.pop();
    return job;
}

WATCard::FWATCard WATCardOffice::main() {
    for ( ;; ) {
        _Accept( ~WATCardOffice ) { break; }
        or _Accept( create ) {
            printer.print( Printer::WATCardOffice, WATCardOffice::CreateCall, lastId, lastAmount);
        } or _Accept( transfer ) {
            printer.print( Printer::WATCardOffice, WATCardOffice::TransferCall, lastId, lastAmount);
        } or _Accept( requestWork ) {
            printer.print( Printer::WATCardOffice, WATCardOffice::RequestWork );
        }
    }
}

WATCardOffice::Courier::Courier( Printer & prt, Bank & bank, WATCardOffice * cardOffice ): 
    printer( prt ), bank( bank ), cardOffice( cardOffice ) {}

WATCardOffice::Courier::main() {
    for ( ;; ) {
        Job * job = cardOffice->requestWork();
        Args args = *job->args;
        Watcard * watcard; 

        if ( args.job == Args::Create ) watcard = new Watcard();
        else if ( args.job == Args::Transfer ) watcard = args.card;

        bank.withdraw( args.sid, args.amount );
        watcard->deposit( args.amount );

        if ( mprng( 1, 6 ) == 1 ) result.exception( new Lost() );
        else result.delivery( watcard );
    }
}