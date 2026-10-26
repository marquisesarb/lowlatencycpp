#include <llcpp/orderbook.hpp>
#include <cassert>
#include <random>
#include <chrono>

void testPriceQuantityMap() {

    PriceQuantityMap bid{true};

    bid.push({100,15}); 
    bid.push({101,15}); 
    bid.push({102,15}); 
    bid.push({102,15}); 

    std::vector<PriceQuantityPair> pairs = bid.top(3); 

    assert(pairs[0].price == 102); 
    assert(pairs[1].price == 101); 
    assert(pairs[2].price == 100); 
    assert(pairs[0].quantity == 30); 
    assert(pairs[1].quantity == 15); 
    assert(pairs[2].quantity == 15); 
    assert(bid.depth()==3);
    assert(bid.bestPrice()==102); 
    assert(bid.bestQuantity()==30);

    PriceQuantityMap ask{false};

    ask.push({100,15}); 
    ask.push({101,15}); 
    ask.push({102,15}); 
    ask.push({102,15}); 

    std::vector<PriceQuantityPair> pairs2 = ask.top(3); 

    assert(pairs2[0].price == 100); 
    assert(pairs2[1].price == 101); 
    assert(pairs2[2].price == 102); 
    assert(pairs2[0].quantity == 15); 
    assert(pairs2[1].quantity == 15); 
    assert(pairs2[2].quantity == 30); 
    assert(ask.depth()==3);
    assert(ask.bestPrice()==100); 
    assert(ask.bestQuantity()==15);

    ask.erase(102); 

    assert(ask.depth()==2);
    assert(ask.bestPrice()==100); 
    assert(ask.bestQuantity()==15);

    std::vector<PriceQuantityPair> pairs3 = ask.top(3); 

    assert(pairs3.size()==2); 
}

void testSystemInOrder() {

    OrderBook book(0,5);

    book.onUpdate(Update{1, {'B', '\0'}, 101, 10});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 1);
    assert(book.bestBid() == 101); 
    assert(book.bestAsk() == -1); 
    assert(book.depthBid() == 1); 
    assert(book.depthAsk() == 0);  

    book.onUpdate(Update{2, {'B', '\0'}, 100, 10});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 2);
    assert(book.bestBid() == 101); 
    assert(book.bestAsk() == -1); 
    assert(book.depthBid() == 2); 
    assert(book.depthAsk() == 0); 

    book.onUpdate(Update{3, {'A', '\0'}, 102, 10});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 3);
    assert(book.bestAsk() == 102); 
    assert(book.bestBid() == 101); 
    assert(book.depthAsk() == 1); 
    assert(book.depthBid() == 2); 

    book.onUpdate(Update{4, {'A', '\0'}, 102, 10});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 4);
    assert(book.bestAsk() == 102); 
    assert(book.bestBid() == 101); 
    assert(book.depthAsk() == 1); 
    assert(book.depthBid() == 2); 

    book.onUpdate(Update{5, {'A', '\0'}, 102, 0});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 5);
    assert(book.bestAsk() == -1); 
    assert(book.bestBid() == 101); 
    assert(book.depthAsk() == 0); 
    assert(book.depthBid() == 2); 

    book.onUpdate(Update{6, {'A', '\0'}, 102, 10});
    book.onUpdate(Update{7, {'A', '\0'}, 103, 12});
    book.onUpdate(Update{8, {'A', '\0'}, 104, 5});
    book.onUpdate(Update{9, {'A', '\0'}, 105, 25});
    book.onUpdate(Update{10, {'B', '\0'}, 99, 10});
    book.onUpdate(Update{11, {'B', '\0'}, 95, 12});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 11);
    assert(book.bestAsk() == 102); 
    assert(book.bestBid() == 101); 
    assert(book.depthAsk() == 4); 
    assert(book.depthBid() == 4); 

    book.onUpdate(Update{12, {'A', '\0'}, 103, 0});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 12);
    assert(book.bestAsk() == 102); 
    assert(book.bestBid() == 101); 
    assert(book.depthAsk() == 3); 
    assert(book.depthBid() == 4); 

    book.onUpdate(Update{13, {'A', '\0'}, 104, -5});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 13);
    assert(book.bestAsk() == 102); 
    assert(book.bestBid() == 101); 
    assert(book.depthAsk() == 2); 
    assert(book.depthBid() == 4); 

}

void testSystemNotInOrder() {

    OrderBook book(0,5);

    book.onUpdate(Update{1, {'B', '\0'}, 101, 10});
    book.onUpdate(Update{2, {'B', '\0'}, 100, 10});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 2);
    assert(book.bestBid() == 101); 
    assert(book.bestAsk() == -1); 
    assert(book.depthBid() == 2); 
    assert(book.depthAsk() == 0); 

    book.onUpdate(Update{6, {'A', '\0'}, 102, 10});
    book.onUpdate(Update{7, {'A', '\0'}, 103, 12});
    assert(book.numberInQueue()==2);
    assert(book.sequence() == 2);
    assert(book.bestBid() == 101); 
    assert(book.bestAsk() == -1); 
    assert(book.depthBid() == 2); 
    assert(book.depthAsk() == 0); 

    book.onUpdate(Update{4, {'A', '\0'}, 102, 10});
    book.onUpdate(Update{5, {'A', '\0'}, 102, 0});
    assert(book.numberInQueue()==4);
    assert(book.sequence() == 2);
    assert(book.bestBid() == 101); 
    assert(book.bestAsk() == -1); 
    assert(book.depthBid() == 2); 
    assert(book.depthAsk() == 0); 

    book.onUpdate(Update{8, {'A', '\0'}, 104, 5});

    try {

        book.onUpdate(Update{9, {'A', '\0'}, 105, 25});
        assert(false);

    } catch (const std::runtime_error& e) {

        assert(true);
    }

    book.onUpdate(Update{3, {'A', '\0'}, 102, 10});
    assert(book.numberInQueue()==0);

    book.onUpdate(Update{9, {'A', '\0'}, 105, 25});
    book.onUpdate(Update{10, {'B', '\0'}, 99, 10});
    book.onUpdate(Update{11, {'B', '\0'}, 95, 12});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 11);
    assert(book.bestAsk() == 102); 
    assert(book.bestBid() == 101); 
    assert(book.depthAsk() == 4); 
    assert(book.depthBid() == 4); 


    book.onUpdate(Update{13, {'A', '\0'}, 104, -5});
    assert(book.numberInQueue()==1);
    book.onUpdate(Update{12, {'A', '\0'}, 103, 0});
    assert(book.numberInQueue()==0);
    assert(book.sequence() == 13);
    assert(book.bestAsk() == 102); 
    assert(book.bestBid() == 101); 
    assert(book.depthAsk() == 2); 
    assert(book.depthBid() == 4); 

}

std::vector<Update> generateUpdates(size_t N, int K, uint64_t startSequence = 0) {

    std::vector<Update> updates;
    updates.reserve(N);

    std::mt19937_64 rng(std::random_device{}());

    std::uniform_int_distribution<int> sideDist(0, 1);
    std::uniform_int_distribution<int> priceDist(1, 109);
    std::uniform_int_distribution<int> quantityDist(1, 109);
    std::bernoulli_distribution deleteDist(0.8);

    for (uint64_t i = startSequence; i < N; ++i) {
        Update u;

        u.sequence = i+1;

        u.side[0] = sideDist(rng) == 0 ? 'A' : 'B';
        u.side[1] = '\0';

        u.price = priceDist(rng);

        u.quantity = deleteDist(rng)
            ? 0
            : quantityDist(rng);

        updates.push_back(u);
    }

    // Shuffle each block independently.
    for (size_t begin = 0; begin < N; begin += K) {
        size_t end = std::min(begin + K, N);
        std::shuffle(
            updates.begin() + begin,
            updates.begin() + end,
            rng
        );
    }

    return updates;
}

void testSpeedNaive() {

    int K = 1000; 
    int N = 1000000; 

    std::vector<Update> inputVector = generateUpdates(N, K);
    
    OrderBook book(0,K); 

    auto start = std::chrono::steady_clock::now();
    book.onUpdateVectorized(inputVector); 
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start
    ); 

    std::cout << "Time: " << duration.count() << " milliseconds\n";

    book.print(); 

    inputVector = generateUpdates(N, K, book.sequence());

    book.onUpdateVectorized(inputVector); 

    book.print();

}

int main() {

    testPriceQuantityMap();
    testSystemInOrder();
    testSystemNotInOrder();
    testSpeedNaive();
    return 0;
}