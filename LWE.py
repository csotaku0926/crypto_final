import numpy as np

class LWE:
    """
    This class implements the Learning with Errors (LWE) cryptosystem functionality
    used to define parameters for LWE encryption and decryption.
    """
    def __init__(self, N, Q):
        """
        Initialize the LWE parameters.

        :param N: Number of bits in the secret key.
        :param Q: Modulus for the LWE scheme.
        """
        self.N = N
        self.Q = Q
        b = self._calc_err_bound()
        self.B = (-b, b+1)
        self.rng = np.random.default_rng()

    def _calc_err_bound(self):
        """
        Caclculate the error bound for the LWE scheme

        :return B: Error bound
        """
        return np.floor(np.sqrt(self.Q / (4 * self.N)))
    
    def _get_from_distribution(self, sz):
        """
        Get integers from a discrete normal between -B and B+1
        """
        return self.rng.binomial(self.B[1] - self.B[0], 0.5, sz) + self.B[0]