#include "soda.h"

Student::Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff,
			 unsigned int id, unsigned int maxPurchases ): 
             printer( prt ), nameServer( nameServer ), cardOffice( cardOffice ), groupoff( groupoff ),
             id( id ), maxPurchases( maxPurchases ), watCardAvail( false ), giftCardAvail( false ) {
    numOfPurchases = mprng( 1, maxPurchases );			// Choose number of purchases
    favouriteFlavour = mprng( NUMFLAVOURS - 1 );		// Choose favourite flavour
    fwatcard = cardOffice.create( id, 5 ); 				// Create watcard
    fgiftCard = groupoff.giftcard();					// Create giftcard (only once)
    currentMachine = nameServer.getMachine( id );		// Get vending machine
}

void Student::main() {
    printer.print( Printer::Student, Student::Start, id, 
        favouriteFlavour, numOfPurchases );
    printer.print( Printer::Student, Student::SelectVending, 
        id, ( int ) currentMachine->getId() );
    WATCard * card;								// Current card to use 
	WATCard * watcard, giftCard;
    States cardType;							// Giftcard or watcard

	// Buy soda numOfPuchases times
    for ( int i = 0; i < numOfPurchases; i++ ) {
        yield( mprng( 1, 10 ) );				// Yield before buying

        for ( ;; ) {							// Loop until buy successfully
            try {
                When( !giftCardAvail ) _Select ( fgiftCard ) {		// Priority to giftcard
                    giftCardAvail = true;
                    giftCard = fgiftCard;

                    cardType = Student::GiftCard;
                    fgiftCard.reset();
                } or When( !watCardAvail ) _Select( fwatcard ) {	// Wait if watcard not available
                    watCardAvail = true;
                    watcard = fwatcard;

                    cardType = Student::WatCard;
                    fwatcard.reset();
                } // Do not block if either available
				When( watCardAvail || ( giftCardAvail && ( giftCard.getBalance() > 0 ))) _Else {}

				if ( giftCardAvail && ( giftCard.getBalance() > 0 ) ) card = giftCard;	// Priority to giftcard
				else card = watcard;

                card.getBalance();
                currentMachine->buy( favouriteFlavour, card );		// Buy from vending machine

            } catch ( WATCardOffice::Lost & ) {						// Lost card
                printer.print( Printer::Student, Student::Lost, id );
                fwatcard = cardOffice.create( id, 5 );				// Create a new card
                continue;
            } catch ( VendingMachine::Funds & ) {					// Insufficient funds
                int amount = 5 + sodaCost;							// Current soda cost and 5$
                fwatcard = cardOffice.transfer( id, amount, &card );	// Transfer extra money 
                continue;
            } catch ( VendingMachine::Stock & ) {					// Out of stock
                currentMachine = nameServer.getMachine( id );		// Get new vending from nameserver
                continue;
            } catch ( VendingMachine::Free & ) {
                if ( cardType == Student::WatCard ) 
                    cardType = Student::WatCardFree;
                else 
                    cardType = Student::GiftCardFree;

                yield( 4 );
            }

			// Student bought the soda
            printer.print( Printer::Student, cardType, id, favouriteFlavour, card.getBalance());
        }
    }
}