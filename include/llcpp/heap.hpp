#pragma once 
#include <vector>
#include <iostream>

template<class T> 
class Heap {

    std::vector<T> vec; 
    bool isMax;

    static size_t parent(size_t idx) {return (idx-1)/2;}
    static size_t leftChild(size_t idx) {return 2*idx + 1;}
    static size_t rightChild(size_t idx) {return 2*idx + 2;}

    public: 
        Heap(bool isMax_ = false): isMax(isMax_) {};

        void push(const T& input) {

            size_t idx = size();
            vec.push_back(input);

            while (idx > 0 && ((!isMax && vec.at(idx)<vec.at(parent(idx))) or (isMax && vec.at(idx)>vec.at(parent(idx))))) {
                
                std::swap(vec.at(idx), vec.at(parent(idx))); 
                idx = parent(idx); 

            }
        }

        void pop() {

            std::swap(vec.at(0), vec.back()); 
            vec.pop_back(); 

            size_t idx = 0;

            while (size()>0) {

                size_t left_idx = leftChild(idx); 
                size_t right_idx = rightChild(idx); 

                if (left_idx>size()-1) {

                    break;

                } else if (left_idx==size()-1) {

                    if ((!isMax && vec.at(idx)>vec.at(left_idx)) or (isMax && vec.at(idx)<vec.at(left_idx))) {

                        std::swap(vec.at(idx), vec.at(left_idx)); 
                        idx = left_idx;

                    } else {

                        break;
                    }

                } else {

                    size_t choosen_idx = isMax ? vec.at(left_idx)>vec.at(right_idx) ? left_idx : right_idx : vec.at(left_idx)<vec.at(right_idx) ? left_idx : right_idx;

                    if ((!isMax && vec.at(idx)>vec.at(choosen_idx)) or (isMax && vec.at(idx)<vec.at(choosen_idx))) {

                        std::swap(vec.at(idx), vec.at(choosen_idx)); 
                        idx = choosen_idx;

                    } else {

                        break;
                    }
                }

            }

        }

        [[nodiscard]] T top() const {return vec.at(0);} // O(1)

        [[nodiscard]] size_t size() const {return vec.size();}

        void print() const {std::cout << "My heap: " << std::endl; for (const T& t: vec) std::cout << t << std::endl;}

};
