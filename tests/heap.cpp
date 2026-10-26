#include <llcpp/heap.hpp>
#include <cassert>

int main() {

    Heap<int> minHeap; 
    Heap<int> maxHeap(true); 

    minHeap.push(5); 
    maxHeap.push(5); 

    assert(minHeap.top() == 5); 
    assert(minHeap.size() == 1); 
    assert(maxHeap.top() == 5); 
    assert(maxHeap.size() == 1); 

    minHeap.push(10);
    maxHeap.push(10); 

    assert(minHeap.top() == 5);
    assert(minHeap.size() == 2);
    assert(maxHeap.top() == 10); 
    assert(maxHeap.size() == 2); 

    minHeap.push(2);
    maxHeap.push(2); 

    assert(minHeap.top() == 2);
    assert(minHeap.size() == 3);
    assert(maxHeap.top() == 10); 
    assert(maxHeap.size() == 3); 

    minHeap.push(15);
    maxHeap.push(15); 

    assert(minHeap.top() == 2);
    assert(minHeap.size() == 4);
    assert(maxHeap.top() == 15); 
    assert(maxHeap.size() == 4); 

    minHeap.print(); 
    minHeap.push(5);
    minHeap.print(); 
    minHeap.pop(); 
    minHeap.print(); 


    minHeap.pop(); 
    maxHeap.pop(); 

    assert(minHeap.top() == 5);
    assert(minHeap.size() == 3);
    assert(maxHeap.top() == 10); 
    assert(maxHeap.size() == 3); 

    minHeap.pop(); 
    maxHeap.pop(); 

    assert(minHeap.top() == 10);
    assert(minHeap.size() == 2);
    assert(maxHeap.top() == 5); 
    assert(maxHeap.size() == 2); 
    
}