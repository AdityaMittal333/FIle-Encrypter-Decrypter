#include "RSA.hpp"
#include "../processes/Task.hpp"
#include <iostream>

int gcd(int a, int b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

int modInverse(int e, int phi) {
    for (int d = 1; d < phi; d++) {
        if ((e * d) % phi == 1) return d;
    }
    return -1;
}

int modPow(int base, int exp, int mod) {
    int result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

int executeRSA(const std::string& taskData) {
    Task task = Task::fromString(taskData);

    int p = 17, q = 11;
    int n = p * q;
    int phi = (p - 1) * (q - 1);
    int e = 7;
    int d = modInverse(e, phi);

    if (d == -1) {
        std::cerr << "Error: Invalid RSA key setup.\n";
        return 1;
    }

    std::string buffer;
    char ch;

    while (task.f_stream.get(ch)) {
        buffer.push_back(ch);
    }

    task.f_stream.clear();
    task.f_stream.seekp(0, std::ios::beg);

    for (char c : buffer) {
        int val = static_cast<unsigned char>(c);
        if (val >= n) {
            std::cerr << "Warning: char '" << c << "' val " << val << " exceeds modulus " << n << ", skipping\n";
            task.f_stream.put(c);
            continue;
        }

        int result;
        if (task.action == Action::ENCRYPT) {
            result = modPow(val, e, n);
        } else {
            result = modPow(val, d, n);
        }
        task.f_stream.put(static_cast<char>(result));
    }

    task.f_stream.close();
    return 0;
}
