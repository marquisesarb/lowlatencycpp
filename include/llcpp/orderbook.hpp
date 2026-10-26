#pragma once 
#include <llcpp/heap.hpp>
#include <set>

struct PriceQuantityPair {

    int price;
    int quantity; 

    bool operator<(const PriceQuantityPair& other) const {return price<other.price;}
}; 

class PriceQuantityMap {

    std::set<PriceQuantityPair> pairs_; 
    bool _isBid;

    std::set<PriceQuantityPair>::iterator lowerBound(int price) const {

        return pairs_.lower_bound({price,0}); // O(Log N)
    }

    public: 
        PriceQuantityMap(bool isBid_): _isBid(isBid_) {}

        void push(const PriceQuantityPair& pair) {

            std::set<PriceQuantityPair>::iterator it = lowerBound(pair.price); // O(Log N)

            int quantity = pair.quantity;

            if (it != pairs_.end() && pair.price == it->price) {

                quantity += it->quantity; 
                it = pairs_.erase(it);
            } 

            if (quantity != 0) {
                
                pairs_.insert(it, {pair.price,quantity}); // Amortized O(1)
            }
        }

        void erase(int price) {

            pairs_.erase({price,0}); // O(Log N)
        }

        int bestPrice() const {return (pairs_.size()==0) ? -1 : _isBid ? pairs_.rbegin()->price : pairs_.begin()->price;} // O(1)

        int bestQuantity() const {return (pairs_.size()==0) ? -1 : _isBid ? pairs_.rbegin()->quantity : pairs_.begin()->quantity;} // O(1)

        std::vector<PriceQuantityPair> top(size_t n) const {
            
            std::vector<PriceQuantityPair> output; 
            size_t idx = 0;

            if (!isBid()) {

                for (auto it = pairs_.begin(); it != pairs_.end(); ++it) {

                    output.push_back(*it);
                    idx += 1; 
                    if (idx==n or idx==depth()) break;
                }

            } else {

                for (auto it = pairs_.rbegin(); it != pairs_.rend(); ++it) {

                    output.push_back(*it);
                    idx += 1; 
                    if (idx==n or idx==depth()) break;
                }
            }
            
            return output;  // O(N)
        }

        bool isBid() const {return _isBid;}

        size_t depth() const {return pairs_.size();}

};

struct Update {

    uint64_t sequence; 
    char side[2]; // "A" or "B"
    int price; 
    int quantity; // 0 = delete 

    bool isValid() const {

        if ((std::memcmp(side, "A", 2) == 0) or (std::memcmp(side, "B", 2) == 0)) {

            if ((quantity >= 0 or quantity <= 109) && (price >= 1 or price <= 109)) {

                if (sequence<10e7) {

                    return true;   
                }
            }
        }
        return false;
    }

    PriceQuantityPair pair() const {return {price,quantity};}

    bool operator<(const Update& other) {return sequence<other.sequence;}
    bool operator>(const Update& other) {return sequence>other.sequence;}
};

class OrderBook {

    uint64_t sequence_; 
    Heap<Update> updateInQueue; 
    PriceQuantityMap bidSide; 
    PriceQuantityMap askSide; 
    int maximumInQueue;

    public: 
        OrderBook(uint64_t initialSequence = 0, int K = 100): updateInQueue(false), askSide(false), bidSide(true) {

            sequence_ = initialSequence; 
            maximumInQueue = K;      
        }

        int bestBid() const {return bidSide.bestPrice();} // O(1)
        int bestAsk() const {return askSide.bestPrice();} // O(1)
        int bestBidSize() const {return bidSide.bestQuantity();} // O(1)
        int bestAskSize() const {return askSide.bestQuantity();} // O(1)
        size_t depthAsk() const {return askSide.depth();} //O(1)
        size_t depthBid() const {return bidSide.depth();} //O(1)
        size_t numberInQueue() const {return updateInQueue.size();}
        uint64_t sequence() const {return sequence_;}

        void onUpdate(const Update& update) {

            if (update.sequence == sequence_+1) {

                sequence_ = update.sequence; 

                if (update.isValid()) {

                    if ((std::memcmp(update.side, "A", 2) == 0)) {

                        if ((depthBid()==0) or (update.price>=bestBid())) {

                            if (update.quantity==0) {

                                askSide.erase(update.price);

                            } else {

                                askSide.push(update.pair());
                            }
                        } 

                    } else if ((std::memcmp(update.side, "B", 2) == 0)) {

                        if ((depthAsk()==0) or (update.price<=bestAsk())) {

                            if (update.quantity==0) {

                                bidSide.erase(update.price);

                            } else {

                                bidSide.push(update.pair());
                            }

                        } 
                    }
                }

                if (updateInQueue.size()>0){

                    Update topUpdate = updateInQueue.top();
                    updateInQueue.pop();
                    onUpdate(topUpdate);
                    
                }
            } else {

                if (updateInQueue.size()==maximumInQueue) {
                    
                    throw std::runtime_error("Too many updates in queue!");

                } else {

                    updateInQueue.push(update);
                }
            }
        }

        void print() const {

            std::vector<PriceQuantityPair> bids = bidSide.top(bidSide.depth()); 
            std::vector<PriceQuantityPair> asks = askSide.top(askSide.depth()); 

            std::cout << "Sequence: " << sequence_ << std::endl; 
            std::cout << "Bids: " << std::endl;
            for (size_t i = bidSide.depth(); i>0; i--) {

                std::cout << "Price: " << bids[i-1].price << ", Quantity: " << bids[i-1].quantity << std::endl;
            }

            std::cout << "Asks: " << std::endl;
            for (size_t i = 0; i<askSide.depth(); i++) {

                std::cout << "Price: " << asks[i].price << ", Quantity: " << asks[i].quantity  << std::endl;
            }
        }

        void onUpdateVectorized(const std::vector<Update>& updates) {

            for (size_t i = 0; i<updates.size(); i++) {


                onUpdate(updates[i]);

            }
        }

        std::unique_ptr<Update> nextInQueue() const {return (numberInQueue()==0) ? nullptr : std::make_unique<Update>(updateInQueue.top());}


};
