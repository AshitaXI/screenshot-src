/**
 * Ashita - Copyright (c) 2023 Ashita Development Team
 * Contact: https://www.ashitaxi.com/
 * Contact: https://discord.gg/Ashita
 *
 * This file is part of Ashita.
 *
 * Ashita is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ashita is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ashita.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef DEFINES_HPP_INCLUDED
#define DEFINES_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Compiler Warning Disables
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4200) // non-sized array
#pragma warning(disable : 4503) // decorated name length exceeded, name was truncated

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Windows Compiling Configurations
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Assert Macros
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#ifdef _DEBUG
#define ASSERT(x) assert(x)
#else
#define ASSERT(x) ((void)0)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Helper Macros
//
////////////////////////////////////////////////////////////////////////////////////////////////////
// clang-format off
#define SAFE_DELETE(p)         if (p) { delete p; p = nullptr; }
#define SAFE_DELETEARRAY(p)    if (p) { delete[] p; p = nullptr; }
#define SAFE_RELEASE(p)        if (p) { p->Release(); p = nullptr; }
#define SAFE_RELEASED2D(p)     if (p) { p.Release(); p = nullptr; }
#define SAFE_RELEASEDELETE(p)  if (p) { p->Release(); delete p; p = nullptr; }
#define SAFE_CALL(p, c)        if (p) { p->c; }

#define GET(t, n)              inline t get_##n(void) const { return this->##n##_; }
#define SET(t, n)              inline void set_##n(t tt) { this->##n##_ = tt; }
#define GET_SET(t, n)          GET(t, n) SET(t, n)
#define GET_STR(t, n)          inline t get_##n(void) const { return this->##n##_.c_str(); }
#define SET_STR(t, n)          inline void set_##n(t tt) { this->##n##_ = tt; }
#define GET_SET_STR(t, n)      GET_STR(t, n) SET_STR(t, n)
#define GET_O(t, n)            inline t get_##n(void) const override { return this->##n##_; }
#define SET_O(t, n)            inline void set_##n(t tt) override { this->##n##_ = tt; }
#define GET_SET_O(t, n)        GET_O(t, n) SET_O(t, n)
#define GET_STR_O(t, n)        inline t get_##n(void) const override { return this->##n##_.c_str(); }
#define SET_STR_O(t, n)        inline void set_##n(t tt) override { this->##n##_ = tt; }
#define GET_SET_STR_O(t, n)    GET_STR_O(t, n) SET_STR_O(t, n)
#define GET_REF(t, n)          inline t get_##n(void) { return this->##n##_; }
// clang-format on

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Headers and Libraries
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <filesystem>
#include <format>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Ashita SDK Headers and Libraries
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Ashita.h"

#endif // DEFINES_HPP_INCLUDED