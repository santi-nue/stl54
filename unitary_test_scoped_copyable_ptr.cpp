/*
 * Copyright 2005-2020 Olivier Aveline <wsgd@free.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//*****************************************************************************
// Includes.
//*****************************************************************************

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

#include "unitary_tests_tools.h"
#include "scoped_copyable_ptr.h"


//*****************************************************************************
// test_scoped_copyable_ptr
//*****************************************************************************

class C_scoped_copyable_ptr_object
{
    C_scoped_copyable_ptr_object &  operator=(const C_scoped_copyable_ptr_object & rhs);

public:
    C_scoped_copyable_ptr_object()
        :id (++id_counter),
         generation (1)
    {
    }
    C_scoped_copyable_ptr_object(const C_scoped_copyable_ptr_object & rhs)
        :id (rhs.id),
         generation (rhs.generation + 1)
    {
    }
    ~C_scoped_copyable_ptr_object()
    {
        last_deleted_id = id;
        last_deleted_generation = generation;
    }

    static int    id_counter;
    static int    last_deleted_id;
    static int    last_deleted_generation;
    int           id;
    int           generation;
};
int   C_scoped_copyable_ptr_object::id_counter = 0;
int   C_scoped_copyable_ptr_object::last_deleted_id = 0;
int   C_scoped_copyable_ptr_object::last_deleted_generation = 0;

M_TEST_FCT(test_scoped_copyable_ptr)
{
    // Empty
    scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_a;

    M_TEST_NULL(scptr_a.get());

    // Empty reset 
    scptr_a.reset(nullptr);
    M_TEST_NULL(scptr_a.get());

    // reset initialize
    scptr_a.reset(new C_scoped_copyable_ptr_object);
    M_TEST_NOT_NULL(scptr_a.get());
    M_TEST_EQ(scptr_a->id, 1);
    M_TEST_EQ((*scptr_a).id, 1);
    M_TEST_EQ(scptr_a->generation, 1);

    // Create not empty
    scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_b(new C_scoped_copyable_ptr_object);

    M_TEST_NOT_NULL(scptr_b.get());
    M_TEST_EQ(scptr_b->id, 2);
    M_TEST_EQ(scptr_b->generation, 1);

    // Assign

    // Do not compile : forbidden
//	scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_c = new C_scoped_copyable_ptr_object;

    scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_c(nullptr);
    M_TEST_NULL(scptr_c.get());

    scptr_c = scptr_a;    // deep copy

    M_TEST_NOT_NULL(scptr_c.get());
    M_TEST_EQ(scptr_c->id, 1);
    M_TEST_EQ(scptr_c->generation, 2);    // deep copy

    M_TEST_NOT_NULL(scptr_a.get());
    M_TEST_EQ(scptr_a->id, 1);
    M_TEST_EQ(scptr_a->generation, 1);

    // Copy
    {
        scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_d(scptr_c);    // deep copy

        M_TEST_NOT_NULL(scptr_d.get());
        M_TEST_EQ(scptr_d->id, 1);
        M_TEST_EQ(scptr_d->generation, 3);    // deep copy
    }

    // Out of scope destruction
    M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_id, 1);
    M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_generation, 3);

    // swap
    swap(scptr_c, scptr_b);

    M_TEST_NOT_NULL(scptr_b.get());
    M_TEST_EQ(scptr_b->id, 1);
    M_TEST_EQ(scptr_b->generation, 2);

    M_TEST_NOT_NULL(scptr_c.get());
    M_TEST_EQ(scptr_c->id, 2);
    M_TEST_EQ(scptr_c->generation, 1);

    // swap on itself
    swap(scptr_c, scptr_c);

    M_TEST_NOT_NULL(scptr_c.get());
    M_TEST_EQ(scptr_c->id, 2);
    M_TEST_EQ(scptr_c->generation, 1);

    // Destruction reset 
    scptr_a.reset(nullptr);
    M_TEST_NULL(scptr_a.get());

    M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_id, 1);
    M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_generation, 1);

    // Destruction reset initialize
    scptr_a.reset(new C_scoped_copyable_ptr_object);

    M_TEST_NOT_NULL(scptr_a.get());
    M_TEST_EQ(scptr_a->id, 3);
    M_TEST_EQ(scptr_a->generation, 1);

    // Really bad idea : NO deep copy here !!! BAD, BAD, BAD
//	scptr_a.reset(scptr_b.get());
    scptr_a.reset(new C_scoped_copyable_ptr_object);

    M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_id, 3);
    M_TEST_EQ(C_scoped_copyable_ptr_object::last_deleted_generation, 1);

    M_TEST_NOT_NULL(scptr_a.get());
    M_TEST_EQ(scptr_a->id, 4);
    M_TEST_EQ(scptr_a->generation, 1);
}

M_TEST_FCT(test_scoped_copyable_ptr__equality)
{
    {
        scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_a;

        M_TEST(scptr_a == nullptr);
        M_TEST(!(scptr_a != nullptr));
        M_TEST_EQ(scptr_a, nullptr);
        M_TEST_NULL(scptr_a);
    }
    {
        scoped_copyable_ptr<C_scoped_copyable_ptr_object>  scptr_a(new C_scoped_copyable_ptr_object);

        M_TEST(!(scptr_a == nullptr));
        M_TEST(scptr_a != nullptr);
        M_TEST_NE(scptr_a, nullptr);
        M_TEST_NOT_NULL(scptr_a);
    }
}
