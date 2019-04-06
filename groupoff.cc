#include "soda.h"
#include <vector>

// delete the watcard in any future that is still available
void Groupoff::deleteUnusedCards(WATCard::FWATCard cards[], unsigned int cardCount) {
	for (unsigned int i = 0; i < cardCount; i += 1) {
		if (cards[i].available()) {
			delete (cards[i])();
		}
	}
}

void Groupoff::main() {
	printer.print(Printer::Kind::Groupoff, (char) States::Start);
	emptyCards.resize(numStudents);
	WATCard::FWATCard fullCards[numStudents];
	unsigned int fullCardCount = 0;

	// first give all students a empty future
	for (unsigned int i = 0; i < numStudents; i += 1) {
		if (cardCond.empty()) { 
			_Accept(giftCard);
		}
		emptyCards.at(emptyCardCount);
		emptyCardCount += 1;
		cardCond.signalBlock();
	}

	for (unsigned int i = 0; i < numStudents; i += 1) {
		_Accept(~Groupoff) {
			deleteUnusedCards(fullCards, fullCardCount);
			break;
		} _Else {
			yield(groupoffDelay);
			unsigned int chosenCard = mprng(emptyCardCount - 1);	// chose a random gift card to be filled
			WATCard * giftCard = new WATCard();
			giftCard->deposit(sodaCost);
			emptyCards.at(chosenCard).delivery(giftCard);			// put the filled card in the future
			printer.print(Printer::Kind::Groupoff, (char) States::Deposit, (int) sodaCost);
 
			fullCards[fullCardCount] = emptyCards.at(chosenCard); // add the delivered future to fullCards
			fullCardCount += 1;

			// shift all cards in the array that are after the chosen card, towards the front
			for (unsigned int i = chosenCard + 1; i < numStudents; i += 1) {
				emptyCards.at(i - 1) = emptyCards.at(i);
			}
			emptyCardCount -= 1;
		}
	}
	printer.print(Printer::Kind::Groupoff, (char) States::Finished);

	_Accept(~Groupoff) {
		deleteUnusedCards(fullCards, fullCardCount);
	}
}

Groupoff::Groupoff( Printer & prt, unsigned int numStudents, 
	unsigned int sodaCost, unsigned int groupoffDelay ) 
	: printer(prt), numStudents(numStudents),  sodaCost(sodaCost), 
	groupoffDelay(groupoffDelay), emptyCardCount(0) {

}


WATCard::FWATCard Groupoff::giftCard() {
	cardCond.wait();
	return emptyCards.at(emptyCardCount - 1);
}
