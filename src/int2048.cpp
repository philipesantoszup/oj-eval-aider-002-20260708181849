#include "int2048.h"
#include <iomanip>

namespace sjtu {

int2048::int2048() : neg(false) {
    digits.push_back(0);
}

int2048::int2048(long long v) {
    if (v == 0) {
        neg = false;
        digits.push_back(0);
        return;
    }
    neg = v < 0;
    if (v < 0) v = -v;
    while (v > 0) {
        digits.push_back(v % BASE);
        v /= BASE;
    }
}

int2048::int2048(const std::string &s) {
    read(s);
}

int2048::int2048(const int2048 &other) : digits(other.digits), neg(other.neg) {}

void int2048::trim() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    if (digits.empty()) {
        digits.push_back(0);
        neg = false;
    }
    if (digits.size() == 1 && digits[0] == 0) {
        neg = false;
    }
}

void int2048::read(const std::string &s) {
    digits.clear();
    if (s.empty()) {
        digits.push_back(0);
        neg = false;
        return;
    }
    int start = 0;
    neg = false;
    if (s[0] == '-') {
        neg = true;
        start = 1;
    } else if (s[0] == '+') {
        start = 1;
    }

    for (int i = (int)s.size() - 1; i >= start; i -= 9) {
        int val = 0;
        int power = 1;
        for (int j = i; j >= std::max(start, i - 8); --j) {
            val += (s[j] - '0') * power;
            power *= 10;
        }
        digits.push_back(val);
    }
    trim();
}

void int2048::print() {
    if (neg && !(digits.size() == 1 && digits[0] == 0)) {
        std::cout << '-';
    }
    std::cout << digits.back();
    for (int i = (int)digits.size() - 2; i >= 0; --i) {
        std::cout << std::setfill('0') << std::setw(9) << digits[i];
    }
}

int int2048::compare_abs(const int2048 &other) const {
    if (digits.size() != other.digits.size()) {
        return digits.size() < other.digits.size() ? -1 : 1;
    }
    for (int i = (int)digits.size() - 1; i >= 0; --i) {
        if (digits[i] != other.digits[i]) {
            return digits[i] < other.digits[i] ? -1 : 1;
        }
    }
    return 0;
}

int2048 int2048::abs_add(const int2048 &other) const {
    int2048 res;
    res.digits.clear();
    int carry = 0;
    size_t n = digits.size();
    size_t m = other.digits.size();
    size_t max_len = std::max(n, m);
    for (size_t i = 0; i < max_len || carry; ++i) {
        long long sum = carry + (i < n ? digits[i] : 0) + (i < m ? other.digits[i] : 0);
        res.digits.push_back(sum % BASE);
        carry = sum / BASE;
    }
    res.trim();
    return res;
}

int2048 int2048::abs_sub(const int2048 &other) const {
    int2048 res;
    res.digits.clear();
    int borrow = 0;
    for (size_t i = 0; i < digits.size(); ++i) {
        long long sub = (long long)digits[i] - borrow - (i < other.digits.size() ? other.digits[i] : 0);
        if (sub < 0) {
            sub += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }
        res.digits.push_back(sub);
    }
    res.trim();
    return res;
}

int2048 &int2048::add(const int2048 &other) {
    if (neg == other.neg) {
        int2048 res = abs_add(other);
        res.neg = neg;
        *this = res;
    } else {
        int cmp = compare_abs(other);
        if (cmp >= 0) {
            int2048 res = abs_sub(other);
            res.neg = neg;
            *this = res;
        } else {
            int2048 res = other.abs_sub(*this);
            res.neg = other.neg;
            *this = res;
        }
    }
    return *this;
}

int2048 add(int2048 a, const int2048 &b) {
    return a.add(b);
}

int2048 &int2048::minus(const int2048 &other) {
    int2048 temp = other;
    temp.neg = !temp.neg;
    return add(temp);
}

int2048 minus(int2048 a, const int2048 &b) {
    return a.minus(b);
}

int2048 int2048::operator+() const { return *this; }
int2048 int2048::operator-() const {
    int2048 res = *this;
    if (!(res.digits.size() == 1 && res.digits[0] == 0)) {
        res.neg = !res.neg;
    }
    return res;
}

int2048 &int2048::operator=(const int2048 &other) {
    if (this != &other) {
        digits = other.digits;
        neg = other.neg;
    }
    return *this;
}

int2048 &int2048::operator+=(const int2048 &other) {
    return add(other);
}

int2048 operator+(int2048 a, const int2048 &b) {
    return a += b;
}

int2048 &int2048::operator-=(const int2048 &other) {
    return minus(other);
}

int2048 operator-(int2048 a, const int2048 &b) {
    return a -= b;
}

int2048 &int2048::operator*=(const int2048 &other) {
    if ((digits.size() == 1 && digits[0] == 0) || (other.digits.size() == 1 && other.digits[0] == 0)) {
        digits = {0};
        neg = false;
        return *this;
    }
    
    size_t n = digits.size();
    size_t m = other.digits.size();
    std::vector<long long> res_digits(n + m, 0);
    for (size_t i = 0; i < n; ++i) {
        long long d_i = digits[i];
        if (d_i == 0) continue;
        for (size_t j = 0; j < m; ++j) {
            res_digits[i + j] += d_i * other.digits[j];
            if (res_digits[i + j] >= 8e18) {
                res_digits[i + j + 1] += res_digits[i + j] / BASE;
                res_digits[i + j] %= BASE;
            }
        }
    }
    
    long long carry = 0;
    digits.clear();
    for (size_t i = 0; i < res_digits.size() || carry; ++i) {
        long long cur = carry + (i < res_digits.size() ? res_digits[i] : 0);
        digits.push_back(cur % BASE);
        carry = cur / BASE;
    }
    
    neg = neg != other.neg;
    trim();
    return *this;
}

int2048 operator*(int2048 a, const int2048 &b) {
    return a *= b;
}

int2048 int2048::div_abs(const int2048 &other, int2048 &rem) const {
    int2048 q, r;
    r.digits.clear();
    r.neg = false;
    q.digits.clear();

    for (int i = (int)digits.size() - 1; i >= 0; --i) {
        if (r.digits.size() == 1 && r.digits[0] == 0) {
            r.digits[0] = digits[i];
        } else {
            r.digits.insert(r.digits.begin(), digits[i]);
        }
        r.trim();
        
        long long low = 0, high = BASE - 1, best = 0;
        while (low <= high) {
            long long mid = low + (high - low) / 2;
            int2048 temp(mid);
            temp *= other;
            if (r.compare_abs(temp) >= 0) {
                best = mid;
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        q.digits.push_back((int)best);
        int2048 sub_val(best);
        sub_val *= other;
        r = r.abs_sub(sub_val);
    }
    std::reverse(q.digits.begin(), q.digits.end());
    q.trim();
    rem = r;
    return q;
}

int2048 int2048::div_floor(const int2048 &other, int2048 &rem) const {
    int2048 a = *this; a.neg = false;
    int2048 b = other; b.neg = false;
    
    int2048 q = a.div_abs(b, rem);
    rem.neg = false;
    
    bool res_neg = neg != other.neg;
    
    if (rem.compare_abs(int2048(0)) > 0 && res_neg) {
        q = q.abs_add(int2048(1));
        int2048 q_signed = q; q_signed.neg = res_neg;
        int2048 y = other;
        int2048 prod = q_signed * y;
        int2048 res = *this;
        res.minus(prod);
        rem = res;
    } else {
        q.neg = res_neg;
        int2048 q_signed = q;
        int2048 y = other;
        int2048 prod = q_signed * y;
        int2048 res = *this;
        res.minus(prod);
        rem = res;
    }
    
    q.trim();
    return q;
}

int2048 &int2048::operator/=(const int2048 &other) {
    int2048 rem;
    *this = div_floor(other, rem);
    return *this;
}

int2048 &int2048::operator%=(const int2048 &other) {
    int2048 rem;
    div_floor(other, rem);
    *this = rem;
    return *this;
}

int2048 operator/(int2048 a, const int2048 &b) {
    return a /= b;
}

int2048 operator%(int2048 a, const int2048 &b) {
    return a %= b;
}

std::istream &operator>>(std::istream &is, int2048 &n) {
    std::string s;
    is >> s;
    n.read(s);
    return is;
}

std::ostream &operator<<(std::ostream &os, const int2048 &n) {
    if (n.neg && !(n.digits.size() == 1 && n.digits[0] == 0)) os << '-';
    os << n.digits.back();
    for (int i = (int)n.digits.size() - 2; i >= 0; --i) {
        os << std::setfill('0') << std::setw(9) << n.digits[i];
    }
    return os;
}

bool operator==(const int2048 &a, const int2048 &b) {
    return a.neg == b.neg && a.digits == b.digits;
}

bool operator!=(const int2048 &a, const int2048 &b) {
    return !(a == b);
}

bool operator<(const int2048 &a, const int2048 &b) {
    if (a.neg != b.neg) return a.neg;
    if (a.neg) return a.compare_abs(b) > 0;
    return a.compare_abs(b) < 0;
}

bool operator>(const int2048 &a, const int2048 &b) {
    return b < a;
}

bool operator<=(const int2048 &a, const int2048 &b) {
    return !(b < a);
}

bool operator>=(const int2048 &a, const int2048 &b) {
    return !(a < b);
}

} // namespace sjtu
