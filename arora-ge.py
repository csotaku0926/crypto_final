import numpy as np
from sympy import symbols, Poly, groebner, GF

def solve_lwe_sympy(A, b, q, E, S=None):
    m = len(A)
    n = len(A[0])

    # Create symbolic variables s0, s1, ..., sn-1
    s_vars = symbols(f's0:{n}')

    # Build polynomial constraints from LWE equation: 
    # (b_i - <a_i, s> - e) = 0 for some e in E → ∏_e (b_i - <a_i, s> - e) = 0
    polys = []
    for i in range(m):
        inner = sum(A[i][j] * s_vars[j] for j in range(n))
        poly_expr = 1
        for e in E:
            poly_expr *= (b[i] - inner - e) #% q
        polys.append(Poly(poly_expr, *s_vars, domain=GF(q)))

    # Add constraints s_j in S (if given)
    if S is not None:
        for j in range(n):
            constraint = 1
            for s_val in S:
                constraint *= (s_vars[j] - s_val)
            polys.append(Poly(constraint, *s_vars, domain=GF(q)))

    # Compute Gröbner basis
    G = groebner(polys, *s_vars, order='lex', domain=GF(q))

    # Extract linear univariate polynomials and solve
    solution = [None] * n
    for i, var in enumerate(s_vars):
        # Try to find a univariate polynomial in var
        for g in G:
            if g.total_degree() == 1 and g.gens == (var,):
                coeffs = g.all_coeffs()
                if len(coeffs) == 2:  # linear
                    a, b_const = coeffs
                    try:
                        inv_a = pow(int(a), -1, q)
                    except ValueError:
                        continue  # skip if not invertible
                    root = (-int(b_const) * inv_a) % q
                    solution[i] = int(root)
                    break

    # Check if we have all variables
    if any(s is None for s in solution):
        raise ValueError("Incomplete solution from Gröbner basis.")

    return solution


if __name__ == '__main__':
    A = np.array([[7],[10],[12],[9],[16],[12],[13], [5], [5],[10],[4],[0],[15],[16]])
    s_true = [6]
    e = [0,-1,-1,-1,1,0,1,-1,0,-1,-1,0,0,1]
    q = 17
    b = (A @ s_true + e) % q
    E = [-1,0, 1]
    S = None  

    s_guess = solve_lwe_sympy(A ,b, q, E, S)
    print("Recovered secret:", s_guess)