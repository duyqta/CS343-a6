#include "soda.h"

WATCardOffice::WATCardOffice( Printer & prt, Bank & bank, unsigned int numCouriers ):
    printer( prt ), bank( bank ), numCouriers( numCouriers ), jobRequested( false ) {
    printer.print( Printer::WATCardOffice, ( char ) WATCardOffice::Start );
    for ( unsigned int i = 0; i < numCouriers; i++ ) 
        couriers.push_back( new Courier( printer, bank, this, i ) );	// Create couriers
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

WATCardOffice::Job * WATCardOffice::requestWork() {
    // _When ( jobQueue.empty() ) _Accept( create, transfer ) {}   // Block if no job available
    // or _When ( jobQueue.empty() ) _Accept( ~WATCardOffice )	{
    //     Args args = { 0, 0, NULL, Args::Destroy };
    //     Job * job = new Job( args );
    //     jobQueue.push( job );
    //     terminate = true;
    //     terminateCond.wait();
    // }	
    if ( jobQueue.empty() ) requestWait.wait();

	// Pop available job from queue
    Job * job = jobQueue.front();
    jobQueue.pop();
    jobRequested = true;

    return job;
}

void WATCardOffice::main() {
    for ( ;; ) {
        _Accept( ~WATCardOffice ){
            for ( unsigned int i = 0; i < numCouriers; i++ ) {
                Args args = { 0, 0, NULL, Args::Destroy };
                Job * job = new Job( args );
                jobQueue.push( job );
            }
            while ( !requestWait.empty() ) requestWait.signalBlock();
            for ( unsigned int i = 0; i < numCouriers; i++ ) delete couriers[i];
            break;
        } or _Accept( requestWork ) {
            if ( jobRequested ) {
                printer.print( Printer::WATCardOffice, ( char ) WATCardOffice::RequestWork );
                jobRequested = false;
            }
        } or _Accept( create ) {
            printer.print( Printer::WATCardOffice, ( char ) WATCardOffice::CreateCall, lastId, lastAmount);
            if ( !requestWait.empty() ) {
                requestWait.signalBlock();
                printer.print( Printer::WATCardOffice, ( char ) WATCardOffice::RequestWork );
                jobRequested = false;
            }
        } or _Accept( transfer ) {
            printer.print( Printer::WATCardOffice, ( char ) WATCardOffice::TransferCall, lastId, lastAmount);
            if ( !requestWait.empty() ) {
                requestWait.signalBlock();
                printer.print( Printer::WATCardOffice, ( char ) WATCardOffice::RequestWork );
                jobRequested = false;
            }
        }
    }
    printer.print( Printer::WATCardOffice, ( char ) WATCardOffice::Finished );
}

WATCardOffice::Courier::Courier( Printer & prt, Bank & bank, WATCardOffice * cardOffice, unsigned int id ): 
    printer( prt ), bank( bank ), cardOffice( cardOffice ), id( id ) {}

void WATCardOffice::Courier::main() {
    printer.print( Printer::Courier, id, ( char ) Courier::Start );
    for ( ;; ) {
		// Request new job
        Job * job = cardOffice->requestWork();
        Args args = job->args;
        WATCard * watcard = nullptr; 
        if ( args.job == Args::Destroy ) {
            delete job;
            break;
        }

        printer.print( Printer::Courier, id, ( char ) Courier::StartTransfer );

		// Create new WatCard or use preexisting WatCard
        if ( args.job == Args::Create ) watcard = new WATCard();
        else if ( args.job == Args::Transfer ) watcard = args.card;

		// Transfer money from bank to WatCard
        bank.withdraw( args.sid, args.amount );
        watcard->deposit( args.amount );

		// 1 in 6 chance of losing the card
        if ( mprng( 1, 6 ) == 1 ) {
            job->result.exception( new WATCardOffice::Lost() );
            printer.print( Printer::Courier, id, ( char ) Courier::Lost );
            delete watcard;
        } else {
            job->result.delivery( watcard );
            printer.print( Printer::Courier, id, ( char ) Courier::CompleteTransfer, 
                           ( int ) args.sid, ( int ) args.amount );
        }
        delete job;
    }
    printer.print( Printer::Courier, id, ( char ) Courier::Finished );
}