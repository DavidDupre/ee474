#ifndef _UDOOLS_
#define _UDOOLS_

// template <class T>
// T norm(T input, T min_in, T max_in, T min_out, T max_out);
template <class T>
T norm(T input, T min_in, T max_in, T min_out, T max_out) {
    T output = (max_out - min_out)*(input - min_in)/(max_in - min_in);
    return output;
}

void vehicleComms(void *vehicleCommsData);

#endif  /* _UDOOLS_ */