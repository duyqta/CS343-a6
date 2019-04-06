#include "soda.h"

Student::Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff,
			 unsigned int id, unsigned int maxPurchases ): 
             printer( prt ), nameServer( nameServer ), cardOffice( cardOffice ), groupoff( groupoff ),
             id( id ), maxPurchases( maxPurchases ), watCardAvail( false ), giftCardAvail( false ) {}

void Student::main() {
    numOfPurchases = mprng( 1, maxPurchases );			// Choose number of purchases
    favouriteFlavour = mprng( NUMFLAVOURS - 1 );		// Choose favourite flavour
    printer.print( Printer::Student, id, ( char ) Student::Start, 
        favouriteFlavour, numOfPurchases );

    fwatcard = cardOffice.create( id, 5 ); 				// Create watcard
    fgiftCard = groupoff.giftCard();					// Create giftcard (only once)
    currentMachine = nameServer.getMachine( id );		// Get vending machine
    printer.print( Printer::Student, id, ( char ) Student::SelectVending, 
        ( int ) currentMachine->getId() );

    WATCard * card = nullptr;								// Current card to use 
	WATCard * watcard = nullptr, * giftCard = nullptr;
    States cardType = Student::WatCard;			// Giftcard or watcard

	// Buy soda numOfPuchases times
    for ( int i = 0; i < numOfPurchases; i++ ) {
        yield( mprng( 1, 10 ) );				// Yield before buying

        for ( ;; ) {							// Loop until buy successfully
            try {
                _When( !giftCardAvail ) _Select ( fgiftCard ) {		// Priority to giftcard
                    giftCardAvail = true;
                    giftCard = fgiftCard;

                    cardType = Student::GiftCard;
                    fgiftCard.reset();
                } or _When( !watCardAvail ) _Select( fwatcard ) {	// Wait if watcard not available
                    watCardAvail = true;
                    watcard = fwatcard;

                    cardType = Student::WatCard;
                    fwatcard.reset();
                } // Do not block if either available
				_When( watCardAvail || ( giftCardAvail && ( giftCard->getBalance() > 0 ))) _Else {
                    if ( giftCardAvail && ( giftCard->getBalance() > 0 ) ) {
                        card = giftCard;	// Priority to giftcard
                        cardType = Student::GiftCard;
                    } else {
                        card = watcard;
                        cardType = Student::WatCard;
                    }
                }

                card->getBalance();
                currentMachine->buy( ( VendingMachine::Flavours ) favouriteFlavour, *card );		// Buy from vending machine

            } catch ( WATCardOffice::Lost & ) {						// Lost card
                printer.print( Printer::Student, id, ( char ) Student::Lost );
                fwatcard = cardOffice.create( id, 5 );				// Create a new card
                watCardAvail = false;
                watcard = nullptr;
                continue;
            } catch ( VendingMachine::Funds & ) {					// Insufficient funds
                int amount = 5 + currentMachine->cost();							// Current soda cost and 5$
                fwatcard = cardOffice.transfer( id, amount, card );	// Transfer extra money 
                watCardAvail = false;
                watcard = nullptr;
                continue;
            } catch ( VendingMachine::Stock & ) {					// Out of stock
                currentMachine = nameServer.getMachine( id );		// Get new vending from nameserver
                printer.print( Printer::Student, id, ( char ) Student::SelectVending, 
                    ( int ) currentMachine->getId() );
                continue;
            } catch ( VendingMachine::Free & ) {
                if ( cardType == Student::WatCard ) 
                    cardType = Student::WatCardFree;
                else 
                    cardType = Student::GiftCardFree;

                yield( 4 );
            }
            break;
        }

        // Student bought the soda
            printer.print( Printer::Student, id, ( char ) cardType, 
                favouriteFlavour, card->getBalance());
    }
    // Delete giftcard if student received it. Else, not the student's responsibility.
    if ( giftCard != nullptr ) delete giftCard;

    // Wait for couriers to return watcard, since student has a responsibility to delete their watcard
    try { 
        _When( !watCardAvail ) _Select( fwatcard ) {
            watcard = fwatcard;
        }
    } catch ( WATCardOffice::Lost & ) {}
    delete watcard;
    
    printer.print( Printer::Student, id, ( char ) Student::Finished );
}