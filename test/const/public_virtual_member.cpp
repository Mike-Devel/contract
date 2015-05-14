
#include <iostream>
#include <boost/contract/base_types.hpp>
#include <boost/contract/public_member.hpp>
#include <boost/contract/introspect.hpp>
#include <boost/contract/oldof.hpp>
#include <boost/contract/assert.hpp>
#include <boost/contract/scoped.hpp>
#include <boost/contract/bind.hpp>

struct b {
    std::string y;
    b() : y("b") {}

    void invariant() const { std::clog << "b::inv" << std::endl; }
    static void static_invariant() { std::clog << "b::static_inv" << std::endl;}

    // virtual => ..._contract
    void f_contract(std::string const& s, boost::contract::call c) const {
        boost::shared_ptr<std::string const> old_y = BOOST_CONTRACT_OLDOF(c, y);
        boost::contract::public_member(c, this)
            .precondition([&] { std::clog << "b::f::pre" << std::endl; })
            .postcondition([&] {
                std::clog << "b::f::post" << std::endl;
                BOOST_CONTRACT_ASSERT(y == *old_y + s);
            })
        ;
    }
    virtual void f(std::string const& s) {
        boost::contract::scoped contract = boost::contract::bind(
                &b::f_contract, this, s);
        std::clog << "b::f::body" << std::endl;
        y = y + s;
    }
};

struct a
    #define BASES public b
    : BASES
{
    typedef BOOST_CONTRACT_BASE_TYPES(BASES) base_types;
    #undef BASES

    std::string x;
    a() : x("a") {}

    void invariant() const { std::clog << "a::inv" << std::endl; }
    static void static_invariant() { std::clog << "a::static_inv" << std::endl;}

    // override => introspect_...
    void f(std::string const& s) /* override */ {
        boost::shared_ptr<std::string const> old_x = BOOST_CONTRACT_OLDOF(x);
        boost::contract::scoped contract = boost::contract::public_member<
                introspect_f_contract>(this, s)
            .precondition([&] { std::clog << "a::f::pre" << std::endl; })
            .postcondition([&] {
                std::clog << "a::f::post" << std::endl;
                BOOST_CONTRACT_ASSERT(x == *old_x + s);
            })
        ;

        std::clog << "a::f::body" << std::endl;
        y = y + s;
        x = x + s;
    }
    BOOST_CONTRACT_INTROSPECT(f_contract)
};

int main() {
    a aa;
    aa.f("s");
    return 0;
}

