#include <iostream>
#include <ginac/ginac.h>

int main(int, char**) {
    const GiNaC::numeric a   = 2 * 2 * 2  * 3 * 3  * 5               * 11;
    const GiNaC::numeric b   = 2          * 3      * 5 * 5 * 5  * 7;
    const GiNaC::numeric gcd = 2          * 3      * 5;
    const GiNaC::numeric lcm = 2          * 3      * 5  * 2*2*3*5*5*7*11;
    const GiNaC::numeric c_gcd = GiNaC::gcd(a, b);
    const GiNaC::numeric c_lcm = GiNaC::lcm(a, b);

    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << "gcd (mine)  = " << gcd << std::endl;
    std::cout << "gcd (GiNaC) = " << c_gcd << std::endl;
    std::cout << "lcm (mine)  = " << lcm << std::endl;
    std::cout << "lcm (GiNaC) = " << c_lcm << std::endl;
    return 0;
}
