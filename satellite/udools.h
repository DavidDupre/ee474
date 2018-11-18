#ifndef _UDOOLS_
#define _UDOOLS_

// template <class T>
// T norm(T input, T min_in, T max_in, T min_out, T max_out);
template <class T>
T norm(T input, T min_in, T max_in, T min_out, T max_out) {
    T output = (max_out - min_out)*(input - min_in)/(max_in - min_in) + min_out;
    return output;
}

template <class T>
void addToBuffer(T newValue, volatile T *buffer, int bufferLength) {
    for (int i = bufferLength - 1; i > 0; i--) {
        buffer[i] = buffer[i - 1];
    }
    buffer[0] = newValue;
}

void vehicleComms(void *vehicleCommsData);

#endif  /* _UDOOLS_ */