#ifndef DATAVISUALIZER_MY_ALGORITHM_H
#define DATAVISUALIZER_MY_ALGORITHM_H

#include <algorithm>

template <typename InputIt, typename T>
void fillRangeWithStep(InputIt first, InputIt last, T value, T step)
{
    std::for_each(first, last,
        [&value, step](double &currValue)
        {
            currValue = value;
            value += step;
        }
    );
}

#endif //DATAVISUALIZER_MY_ALGORITHM_H
