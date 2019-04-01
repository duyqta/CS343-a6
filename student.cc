#include "soda.h"

Student::Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff,
			 unsigned int id, unsigned int maxPurchases ): 
             printer( prt ), nameServer( nameServer ), cardOffice( cardOffice ), groupoff( groupoff ),
             id( id ), maxPurchases( maxPurchases ), watCardAvail( false ), giftCardAvail( false ) {
    numOfPurchases = mprng( maxPurchases - 1 ) + 1;
    favouriteFlavour = mprng( NUMFLAVOURS - 1 );
    fwatcard = cardOffice.create( id, 5 ); 
    fgiftCard = groupoff.giftcard();
    currentMachine = nameServer.getMachine( id );
}

void Student::main() {
    printer.print( Printer::Student, Student::Start, id, 
        favouriteFlavour, numOfPurchases );
    printer.print( Printer::Student, Student::SelectVending, 
        id, ( int ) currentMachine->getId() );
    WATCard * card;
    States cardType;
    for ( int i = 0; i < numOfPurchases; i++ ) {
        yield( mprng( 1, 10 ) );

        for ( ;; ) {
            try {
                When( !giftCardAvail ) _Select ( fgiftCard ) {
                    giftCardAvail = true;
                    card = fgiftCard;
                    cardType = Student::GiftCard;
                    fgiftCard.reset();
                } or When( !watCardAvail ) _Select( fwatcard ) {
                    watCardAvail = true;
                    card = fwatcard;
                    cardType = Student::WatCard;
                    fwatcard.reset();
                } When( watCardAvail ) _Else {}

                card.getBalance();
                currentMachine->buy( favouriteFlavour, card );
                
            } catch ( WATCardOffice::Lost & ) {
                printer.print( Printer::Student, Student::Lost, id );
                fwatcard = cardOffice.create( id, 5 );
                continue;
            } catch ( VendingMachine::Funds & ) {
                int amount = 5 + sodaCost;
                fwatcard = cardOffice.transfer( id, amount, &card );
                continue;
            } catch ( VendingMachine::Stock & ) {
                currentMachine = nameServer.getMachine( id );
                continue;
            } catch ( VendingMachine::Free & ) {
                if ( cardType == Student::WatCard ) 
                    cardType = Student::WatCardFree;
                else 
                    cardType = Student::GiftCardFree;

                yield( 4 );
            }
            printer.print( Printer::Student, cardType, id, favouriteFlavour, card.getBalance());
        }
    }
}