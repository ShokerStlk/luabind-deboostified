// Copyright (c) 2004 Daniel Wallin and Arvid Norberg

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

#include "test.hpp"
#include <luabind/luabind.hpp>

namespace {

struct property_test : counted_type<property_test>
{  
    property_test(): o(6) {}

	std::string str_;
    int a_;
    float o;

    void set(int a) { a_ = a; }
    int get() const { return a_; }

    void set_str(const char* str) 
    { str_ = str; }

    const char* get_str() const 
    { return str_.c_str(); }
};

void free_setter(property_test& p, int a)
{ p.set(a); }

int free_getter(const property_test& p)
{ return p.get(); }

struct A
{
	int get() const 
	{ 
		return 5; 
	}
};

struct B : A
{
};

} // namespace unnamed

void test_attributes()
{
    COUNTER_GUARD(property_test);
	
	lua_state L;

    using namespace luabind;

    module(L)
    [
		class_<property_test>("property")
			.def(luabind::constructor<>())
			.def("get", &property_test::get)
			.property("a", &property_test::get, &property_test::set)
			.property(
                "str", &property_test::get_str, &property_test::set_str)
			.def_readonly("o", &property_test::o)
            .property("free", &free_getter, &free_setter),

		class_<A>("A")
			.def(constructor<>())
			.property("a", &A::get),

		class_<B, A>("B")
			.def(constructor<>())
			.property("x", &A::get)
	];

	DOSTRING(L, "test = property()\n");

    DOSTRING(L,
        "test.a = 5\n"
        "assert(test.a == 5)");

	DOSTRING(L, "assert(test.o == 6)");

	DOSTRING(L, 
		"test.str = 'foobar'\n"
		"assert(test.str == 'foobar')\n");

    DOSTRING(L,
        "test.free = 6\n"
        "assert(test.free == 6)\n");

	DOSTRING(L,
		"a = B()\n");

	DOSTRING(L,
		"assert(a.a == 5)\n");

	DOSTRING(L,
		"assert(a.x == 5)\n");
}

