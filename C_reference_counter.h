/*
 * Copyright 2009 Olivier Aveline <wsgd@free.fr>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef C_REFERENCE_COUNTER_H
#define C_REFERENCE_COUNTER_H

//*****************************************************************************
// Includes
//*****************************************************************************

#include "byte_interpret_common.h"


//*****************************************************************************
// C_reference_counter ********************************************************
//*****************************************************************************
// Basic class for objects used with smart pointers.
//*****************************************************************************

class C_reference_counter
{
protected:

  //---------------------------------------------------------------------------
  // Constructor.
  //---------------------------------------------------------------------------
  C_reference_counter () : _ref_counter(0) {}

  //---------------------------------------------------------------------------
  // Copy constructor.
  // Special behavior, DO NOT COPY _ref_counter.
  //---------------------------------------------------------------------------
  C_reference_counter (const C_reference_counter& ) : _ref_counter(0) {}

  //---------------------------------------------------------------------------
  // Assignment operator.
  // Special behavior, DO NOT MODIFY _ref_counter.
  //---------------------------------------------------------------------------
  C_reference_counter&  operator= (const C_reference_counter&)
  { return *this; }

  //---------------------------------------------------------------------------
  // Destructor (verification of reference counter).
  //---------------------------------------------------------------------------
  virtual ~C_reference_counter ()
  {
    M_ASSERT (_ref_counter == 0);
  }


private:

#ifdef non_existent_mutable__

#define MUTABLE
#define CONST_IF_MUTABLE

  //---------------------------------------------------------------------------
  // Les methodes suivantes son necessaire si les donnees ne sont pas mutable.
  //---------------------------------------------------------------------------
  void           incr_ref () const
  { (const_cast<C_reference_counter*>(this))->incr_ref (); }
  void           decr_ref () const 
  { (const_cast<C_reference_counter*>(this))->decr_ref (); }

#else

#define MUTABLE           mutable
#define CONST_IF_MUTABLE  const

#endif

  //---------------------------------------------------------------------------
  // Reference counter (number of smart pointers which point on this object).
  // mutable means that a const method can modify the counter.
  // It's normal for this reference counter to be mutable, if not, smart
  //  pointer on a const object is not possible.
  //---------------------------------------------------------------------------
  MUTABLE
  int            _ref_counter;

  //---------------------------------------------------------------------------
  // Increments the reference counter.
  //---------------------------------------------------------------------------
  void           incr_ref () CONST_IF_MUTABLE
                            {
                              ++_ref_counter;
                            }

  //---------------------------------------------------------------------------
  // Decrements the reference counter (destroys itself if necessary).
  //---------------------------------------------------------------------------
  void           decr_ref () CONST_IF_MUTABLE
                            {
                              if (--_ref_counter <= 0)
                                  delete this;
                            }

  //---------------------------------------------------------------------------
  // Friends.
  //---------------------------------------------------------------------------
  friend void  C_reference_counter_ptr_add_ref (const C_reference_counter * );
  friend void  C_reference_counter_ptr_release (const C_reference_counter * );

};

#undef MUTABLE
#undef CONST_IF_MUTABLE


//-----------------------------------------------------------------------------
// Functions dedicated to C_reference_counter_ptr.
// Only the C_reference_counter_ptr class must use them.
//-----------------------------------------------------------------------------
inline
void  C_reference_counter_ptr_add_ref (const C_reference_counter *  ptr)
{
    ptr->incr_ref ();
}

inline
void  C_reference_counter_ptr_release (const C_reference_counter *  ptr)
{
    ptr->decr_ref ();
}


#endif /* C_REFERENCE_COUNTER_H */
