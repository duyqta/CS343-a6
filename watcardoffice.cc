#include "soda.h"

WATCardOffice::WATCardOffice( Printer & prt, Bank & bank, unsigned int numCouriers ):
    printer( prt ), bank( bank ), numCouriers( numCouriers ) {
    for ( int i = 0; i < ( int ) numCouriers; i++ ) 
        couriers.push_back( new Courier( printer, bank, this ) );	// Create couriers
}

WATCardOffice::~WATCardOffice() {
    for ( int i = 0; i < ( int ) numCouriers; i++ ) 
        delete couriers[i];   										// Terminate couriers
}

WATCard::FWATCard WATCardOffice::create( unsigned int sid, unsigned int amount ) {
	// Save last student info
    lastId = sid;
    lastAmount = amount;

	// Push new Create job to the queue
    Args args = { sid, amount, NULL, Args::Create };
    Job * job = new Job( args );
    jobQueue.push( job );

	// Return the future WatCard
    return job->result;
}

WATCard::FWATCard WATCardOffice::transfer( unsigned int sid, unsigned int amount, WATCard * card ) {
	// Save last student info
    lastId = sid;
    lastAmount = amount;

	// Push new Transfer job to the queue
    Args args = { sid, amount, card, Args::Transfer };
    Job * job = new Job( args );
    jobQueue.push( job );

	// Return the future WatCard
    return job->result;
}

Job * WATCardOffice::requestWork() {
    _When ( jobQueue.empty() ) _Accept( create, transfer );		// Block if no job available

	// Pop available job from queue
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

void WATCardOffice::Courier::main() {
    for ( ;; ) {
		// Request new job
        Job * job = cardOffice->requestWork();
        Args args = *job->args;
        Watcard * watcard; 

		// Create new WatCard or use preexisting WatCard
        if ( args.job == Args::Create ) watcard = new Watcard();
        else if ( args.job == Args::Transfer ) watcard = args.card;

		// Transfer money from bank to WatCard
        bank.withdraw( args.sid, args.amount );
        watcard->deposit( args.amount );

		// 1 in 6 chance of losing the card
        if ( mprng( 1, 6 ) == 1 ) result.exception( new Lost() );
        else result.delivery( watcard );
    }
}