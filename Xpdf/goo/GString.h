//========================================================================
//
// GString.h
//
// Simple variable-length string type.
//
// Copyright 1996-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef GSTRING_H
#define GSTRING_H

#include <aconf.h>
#include <windows.h>
#include <cwchar>

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

class GString {
public:

  // Create an empty string.
  GString();

  // Create a string from a C string.
  GString(const char *sA);

  // Create a string from <lengthA> chars at <sA>.  This string
  // can contain null characters.
  GString(const char *sA, int lengthA);

  // Create a string from <lengthA> chars at <idx> in <str>.
  GString(GString *str, int idx, int lengthA);

  // Copy a string.
  GString(GString *str);
  GString *copy() { return new GString(this); }

  // Concatenate two strings.
  GString(GString *str1, GString *str2);

  // Convert an integer to a string.
  static GString *fromInt(int x);

  // Destructor.
  ~GString();

  // Get length.
  int getLength() { return length; }

  // Get C string.
  char *getCString() { return s; }

  wchar_t *getCStringW() {
	  if (ws!=NULL){
		delete[] ws;
	  }
	  ws = new wchar_t[length+1];
	  MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,s,-1,ws, length+1);
	  return ws; 
  }

  // Get <i>th character.
  char getChar(int i) { return s[i]; }

  // Change <i>th character.
  void setChar(int i, char c) { s[i] = c; }

  // Clear string to zero length.
  GString *clear();

  // Append a character or string.
  GString *append(char c);
  GString *append(GString *str);
  GString *append(const char *str);
  GString *append(const char *str, int lengthA);

  // Insert a character or string.
  GString *insert(int i, char c);
  GString *insert(int i, GString *str);
  GString *insert(int i, const char *str);
  GString *insert(int i, const char *str, int lengthA);

  // Delete a character or range of characters.
  GString *del(int i, int n = 1);

  // Convert string to all-upper/all-lower case.
  GString *upperCase();
  GString *lowerCase();

  // Compare two strings:  -1:<  0:=  +1:>
  int cmp(GString *str);
  int cmpN(GString *str, int n);
  int cmp(const char *sA);
  int cmpN(const char *sA, int n);

private:

  int length;
  char *s;
  wchar_t *ws;

  void resize(int length1);
};

#endif
