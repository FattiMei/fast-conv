#ifndef __AUTOMATA_HPP__
#define __AUTOMATA_HPP__


#include <cstdint>


// `compute_new_line_simd` relies on the Cell type to be an unsigned, 8-bit integer
using Cell = uint8_t;


void compute_new_line_reference        (const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_branchless       (const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_corner_cases     (const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_reuse            (const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_full_reuse       (const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_reuse_lower_bound(const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_packed           (const int n, const Cell above[], Cell below[], const int rule);


// ADVICE: should I include conditionally the prototype of the function?
void compute_new_line_simd             (const int n, const Cell above[], Cell below[], const int rule);


template <typename Integer>
void compute_new_line_packed           (const int n, const Cell above[], Cell below[], const int rule);


#endif
