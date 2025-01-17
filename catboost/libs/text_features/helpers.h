#pragma once

#include <catboost/libs/helpers/exception.h>
#include <util/generic/array_ref.h>
#include <util/generic/ymath.h>
#include <util/stream/length.h>

inline double Norm(TConstArrayRef<float> vec) {
    double norm = 0;
    for (auto val : vec) {
        norm += val * val;
    }
    norm = sqrt(norm);
    return norm;
}

template <class T1, class T2>
inline double CosDistance(
    TConstArrayRef<T1> left,
    TConstArrayRef<T2> right) {
    CB_ENSURE(left.size() == right.size(), "Embedding dim should be equal");
    double normLeft = 1e-10;
    double normRight = 1e-10;
    double dotProduct = 1e-10;
    for (ui64 i = 0; i < left.size(); ++i) {
        dotProduct += left[i] * right[i];
        normLeft += left[i] * left[i];
        normRight += right[i] * right[i];
    }
    return dotProduct / sqrt(normLeft * normRight);
}


inline void Softmax(TArrayRef<double> vals) {
    double maxValue = vals[0];
    for (auto val : vals) {
        maxValue = Max<double>(val, maxValue);
    }
    double total = 0;
    for (auto& val : vals) {
        val -= maxValue;
        val = exp(val);
        total += val;
    }
    for (auto& val : vals) {
        val /= total;
    }
}

inline void AddPadding(TCountingOutput* const output, const ui32 alignment) {
    if (output->Counter() % alignment == 0) {
        return;
    }

    const auto bytesToWrite = alignment - output->Counter() % alignment;
    for (ui64 i = 0; i < bytesToWrite; ++i) {
        output->Write('\0');
    }
}

inline void SkipPadding(TCountingInput* const input, const ui32 alignment) {
    if (input->Counter() % alignment == 0) {
        return;
    }

    const auto bytesToSkip = alignment - input->Counter() % alignment;
    const auto bytesSkipped = input->Skip(bytesToSkip);
    CB_ENSURE(bytesToSkip == bytesSkipped);
}
