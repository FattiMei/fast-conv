#ifndef __AUTOMATA_HPP__
#define __AUTOMATA_HPP__


using Cell = int8_t;


void compute_new_line_reference(const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_branchless(const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_corner_cases(const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_reuse(const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_full_reuse(const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_reuse_lower_bound(const int n, const Cell above[], Cell below[], const int rule);
void compute_new_line_packed(const int n, const Cell above[], Cell below[], const int rule);

template <int UNROLL_FACTOR>
void compute_new_line_simd(const int n, const Cell above[], Cell below[], const int rule);


#endif
