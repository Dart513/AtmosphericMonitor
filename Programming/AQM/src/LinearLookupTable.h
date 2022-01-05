#ifndef LIN_LOOKUP_TABLE_H
#define LIN_LOOKUP_TABLE_H

#include <vector>
#include <algorithm>


template <
    typename Tx,
    typename Ty
>
class LookupTable{
    private:

    std::vector<std::pair<Tx, Ty>> table;

    static int compare(const std::pair<Tx, Ty> & a, const std::pair<Tx, Ty> & b) {
        if (a.first < b.first) {
            return -1;
        } else if (a.first > b.first) {
            return 1;
        } else {
            return 0;
        }
    }

    //execute binary search for x0 in x, then return the index of the
    //closest x-val.
    int search(Tx val) {
        int low = 0;
        int high = table.size() - 1;
        int mid;
        while (low < high) {
            mid = (low + high) / 2;
            if (table[mid].first < val) {
                low = mid + 1;
            } else if (table[mid].first > val) {
                high = mid - 1;
            } else {
                return mid;
            }
            yield();
        }
        return low;
    }


    Ty interpolate(int index0, int index1, Tx xVal) {
        Tx x0 = table[index0].first;
        Tx x1 = table[index1].first;
        Ty y0 = table[index0].second;
        Ty y1 = table[index1].second;

        return y0 + (y1 - y0) * (xVal - x0) / (x1 - x0);
    }

    public:
    LookupTable() {
        static_assert(std::is_arithmetic<Tx>::value, "Not an arithmetic type");
    };

    LookupTable(std::vector<Tx> x0, std::vector<Ty> y0) {
        static_assert(std::is_arithmetic<Tx>::value, "Not an arithmetic type");
        for (int i = 0; i < x0.size(); i++) {
            table.push_back(std::pair<Tx, Ty>(x0[i], y0[i]));
            Serial.printf("So the stuff is: %f, %f\n", x0[i], y0[i]);
        }

        //sort the table
    
        std::sort(table.begin(), table.begin(), compare);
    }

    LookupTable(Tx * x0, Ty * y0, int arrLen)
    {
        static_assert(std::is_arithmetic<Tx>::value, "Not an arithmetic type");
        for (int i = 0; i < arrLen; i++) {
            table.push_back(std::pair<Tx, Ty>(x0[i], y0[i]));
        }

        //sort the table
        std::stable_sort(table.begin(), table.end(), compare); //yeah so the implementation of std::sort is broken on esp8266. 
        //Please don't make the same mistakes I did.
    }

    //Assume that table[0] < x0 < table[table.size() - 1]
    Ty getValue(Tx x0) {
        int lowIndex = 0;
        int highIndex = 0;
        int approxIndex = search(x0);

        if (table[approxIndex].first == x0) {
            return table[approxIndex].second;
        }
        else if (approxIndex == 0) {
            lowIndex = approxIndex;
            highIndex = approxIndex + 1; 
        }
        else if (approxIndex == table.size() - 1) {
            lowIndex = approxIndex - 1;
            highIndex = approxIndex;
        }
        else if (table[approxIndex].first < x0) {
            lowIndex = approxIndex;
            highIndex = approxIndex + 1;
        }
        else {
            lowIndex = approxIndex - 1;
            highIndex = approxIndex;
        }

        return(interpolate(lowIndex, highIndex, x0));
    }

    Tx getMinX() {
        return table[0].first;
    }

    Tx getMaxX() {
        return table[table.size() - 1].first;
    }

    Ty getMinY() {
        return table[0].second;
    }

    Ty getMaxY() {
        return table[table.size() - 1].second;
    }


};


#endif