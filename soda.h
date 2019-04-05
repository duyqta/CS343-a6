#include <vector>
#include <uFuture.h>
#include <queue>
#include "MPRNG.h"

using namespace std;

extern MPRNG mprng;

// Class for info needed to be output by each voter
struct Info {
  	bool flushed = true;
	bool value1Avail, value2Avail;
  	char state;
  	int value1, value2;
  	unsigned int lid;
	void clear() { flushed = true; value1Avail = false; value2Avail = false; }
};

_Monitor Printer {    // chose one of the two kinds of type constructor
	int numStudents, numVendingMachines, numCouriers, numOfInput;
	const int outputSize;
  	std::vector< Info* > vOutput;
  	void flush(); // Call this function to output current line
  public:
	enum Kind { Parent = 0, Groupoff = 1, WATCardOffice = 2, NameServer = 3, Truck = 4, 
			BottlingPlant = 5, Student = 6, Vending, Courier };
	Printer( unsigned int numStudents, unsigned int numVendingMachines, unsigned int numCouriers );
	~Printer();
	void print( Kind kind, char state );
	void print( Kind kind, char state, int value1 );
	void print( Kind kind, char state, int value1, int value2 );
	void print( Kind kind, unsigned int lid, char state );
	void print( Kind kind, unsigned int lid, char state, int value1 );
	void print( Kind kind, unsigned int lid, char state, int value1, int value2 );
  private:
	int getIndex( Kind kind, unsigned int lid );
};

_Monitor Bank {
	std::vector<unsigned int> accounts;
  public:
	Bank( unsigned int numStudents );
	void deposit( unsigned int id, unsigned int amount );
	void withdraw( unsigned int id, unsigned int amount );
};

_Task Parent {
	Printer & printer;
	Bank & bank;
	unsigned int numStudents, parentalDelay;
	void main();
	enum States { Start = 'S', Deposit = 'D', Finished = 'F' };
  public:
	Parent( Printer & prt, Bank & bank, unsigned int numStudents, unsigned int parentalDelay );
};

class WATCard {
	unsigned int balance;
	WATCard( const WATCard & ) = delete;	// prevent copying
	WATCard & operator=( const WATCard & ) = delete;
  public:
	typedef Future_ISM<WATCard *> FWATCard;	// future watcard pointer
	WATCard();
	void deposit( unsigned int amount );
	void withdraw( unsigned int amount );
	unsigned int getBalance();
};

_Task WATCardOffice {
	Printer & printer;
	Bank & bank;
	unsigned int numCouriers;

	struct Args {
		unsigned int sid;
		unsigned int amount;
		WATCard * card;
		enum Jobs { Create, Transfer, Destroy } job;
	};
	struct Job {							// marshalled arguments and return future
		Args args;							// call arguments (YOU DEFINE "Args")
		WATCard::FWATCard result;			// return future
		Job( Args args ) : args( args ) {}
	};
	std::queue<Job*> jobQueue;

	_Task Courier { 
		Printer & printer;
		Bank & bank;
		WATCardOffice * cardOffice;
		unsigned int id;
		enum States { Start = 'S', StartTransfer = 't', Lost = 'L', 
			CompleteTransfer = 'T', Finished = 'F' };
		void main(); 
	  public: 
		Courier( Printer & prt, Bank & bank, WATCardOffice * cardOffice, unsigned id );
	};					// communicates with bank
	vector<Courier*> couriers;

	uCondition requestWait;
	bool jobRequested;
	unsigned int lastId, lastAmount;
	void main();
	enum States { Start = 'S', RequestWork = 'W', CreateCall = 'C', TransferCall = 'T', Finished = 'F' };
  public:
	_Event Lost {};							// lost WATCard
	WATCardOffice( Printer & prt, Bank & bank, unsigned int numCouriers );
	WATCard::FWATCard create( unsigned int sid, unsigned int amount );
	WATCard::FWATCard transfer( unsigned int sid, unsigned int amount, WATCard * card );
	Job * requestWork();
};

_Task Groupoff {
	vector<WATCard::FWATCard> emptyCards;
	Printer & printer;
	WATCard::FWATCard newCard;
	unsigned int numStudents;
	unsigned int sodaCost;
	unsigned int groupoffDelay;
	unsigned int cardCount;
	uCondition cardCond;
	void main();
	enum States { Start = 'S', Deposit = 'D', Finished = 'F' };
  public:
	Groupoff( Printer & prt, unsigned int numStudents, unsigned int sodaCost, unsigned int groupoffDelay );
	WATCard::FWATCard giftCard();
};

_Task VendingMachine;
_Task NameServer {
	Printer & printer;
	unsigned int numVendingMachines;
	unsigned int numStudents;
	unsigned int numRegistered;
	unsigned int studentId;
	VendingMachine ** machines;
	unsigned int * assignedMachines;  // student i should use machines[ assignedMachines[i] ] next
	VendingMachine * assignedMachine;
	void main();
	enum States { Start = 'S', Register = 'R', NewVending = 'N', Finished = 'F' };
  public:
	NameServer( Printer & prt, unsigned int numVendingMachines, unsigned int numStudents );
	void VMregister( VendingMachine * vendingmachine );
	VendingMachine * getMachine( unsigned int id );
	VendingMachine ** getMachineList();
};

_Task VendingMachine {
	unsigned int currFlavour;
	WATCard * currCard;
	uCondition buyCond;
	Printer & printer;
	NameServer & nameServer;
	unsigned int id;
	unsigned int sodaCost;
	unsigned int inv[NUMFLAVOURS];	
	enum ExceptType { FreeEx, FundsEx, StockEx, NoEx };
	ExceptType exceptToThrow;
	void main();
	enum States { Start = 'S', Reloading = 'r', CompleteReloading = 'R', Bought = 'B', Finished = 'F' };
  public:
	enum Flavours { BlackCherry, CreamSoda, RootBeer, Lime }; 	// flavours of soda (YOU DEFINE)
	_Event Free {};												// free, advertisement
	_Event Funds {};											// insufficient funds
	_Event Stock {};											// out of stock for particular flavour
	VendingMachine( Printer & prt, NameServer & nameServer, unsigned int id, unsigned int sodaCost );
	void buy( Flavours flavour, WATCard & card );
	unsigned int * inventory();
	void restocked();
	_Nomutex unsigned int cost() const;
	_Nomutex unsigned int getId() const;
};

_Task Truck;
_Task BottlingPlant {
	Printer & printer;
	NameServer & nameServer;
	Truck * truck;
	unsigned int numVendingMachines, maxShippedPerFlavour, maxStockPerFlavour, timeBetweenShipments;
	bool shutdown;
	unsigned int flavourStock[NUMFLAVOURS];
	void main();
	enum States { Start = 'S', Generate = 'G', PickedUp = 'P', Finished = 'F' };
  public:
	_Event Shutdown {};					// shutdown plant
	BottlingPlant( Printer & prt, NameServer & nameServer, unsigned int numVendingMachines,
				 unsigned int maxShippedPerFlavour, unsigned int maxStockPerFlavour,
				 unsigned int timeBetweenShipments );
	void getShipment( unsigned int cargo[] );
};

_Task Truck {
	Printer & printer;
	NameServer & nameServer;
	BottlingPlant & plant;
	unsigned int machineId;
	unsigned int numVendingMachines;
	unsigned int maxStockPerFlavour;
	void main();
	enum States { Start = 'S', PickUp = 'P', Delivery = 'd', Unsuccess = 'U',
			EndDelivery = 'D', Finished = 'F' };
  public:
	Truck( Printer & prt, NameServer & nameServer, BottlingPlant & plant,
		   unsigned int numVendingMachines, unsigned int maxStockPerFlavour );
};

_Task Student {
	Printer & printer;
	NameServer & nameServer;
	WATCardOffice & cardOffice;
	Groupoff & groupoff;
	unsigned int id, maxPurchases;
	WATCard::FWATCard fwatcard;
	WATCard::FWATCard fgiftCard;
	int numOfPurchases, favouriteFlavour;
	VendingMachine * currentMachine;
	bool watCardAvail, giftCardAvail;
	void main();
	enum States { Start = 'S', SelectVending = 'V', GiftCard = 'G', GiftCardFree = 'a', 
			WatCard = 'B', WatCardFree = 'A', Lost = 'L', Finished = 'F' };
  public:
	Student( Printer & prt, NameServer & nameServer, WATCardOffice & cardOffice, Groupoff & groupoff,
			 unsigned int id, unsigned int maxPurchases );
};
